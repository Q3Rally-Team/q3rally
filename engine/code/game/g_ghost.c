#include "g_local.h"

#define MAX_GHOST_RECORDS_PER_MAP 32
#define GHOST_FILE_EXTENSION ".ghost"
#define GHOST_DIRECTORY "ghosts"

static ghostRecord_t s_levelGhosts[MAX_GHOST_RECORDS_PER_MAP];
static int s_levelGhostCount = 0;

static int G_Ghost_Strlen( const char *text ) {
    int len = 0;

    if ( !text ) {
        return 0;
    }

    while ( text[len] ) {
        ++len;
    }

    return len;
}

static int G_Ghost_ParseInt( const char *text ) {
    int value = 0;

    if ( !text ) {
        return 0;
    }

    while ( *text == ' ' || *text == '\t' ) {
        ++text;
    }

    while ( *text >= '0' && *text <= '9' ) {
        value = value * 10 + ( *text - '0' );
        ++text;
    }

    return value;
}

static void G_Ghost_Reset(void) {
    Com_Memset( s_levelGhosts, 0, sizeof( s_levelGhosts ) );
    s_levelGhostCount = 0;
}

static qboolean G_Ghost_ParseHeader( char *buffer, const char *expectedMap, ghostRecord_t *outRecord ) {
    char *cursor;
    char mapName[MAX_QPATH] = "";
    qboolean hasVehicle = qfalse;
    qboolean hasTime = qfalse;

    if ( !buffer || !outRecord ) {
        return qfalse;
    }

    cursor = buffer;
    while ( cursor && *cursor ) {
        char *lineEnd = cursor;
        char savedChar;

        while ( *cursor == ' ' || *cursor == '\t' ) {
            ++cursor;
        }

        if ( cursor[0] == '\xEF' && cursor[1] == '\xBB' && cursor[2] == '\xBF' ) {
            cursor += 3;
        }

        while ( *lineEnd && *lineEnd != '\n' && *lineEnd != '\r' ) {
            ++lineEnd;
        }

        savedChar = *lineEnd;
        *lineEnd = '\0';

        if ( cursor[0] == '#' || cursor[0] == '\0' ) {
            // skip comments or empty lines
        } else if ( !Q_stricmpn( cursor, "map", 3 ) ) {
            const char *value = cursor + 3;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( mapName, value, sizeof( mapName ) );
        } else if ( !Q_stricmpn( cursor, "vehicle", 7 ) ) {
            const char *value = cursor + 7;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( outRecord->vehicleClass, value, sizeof( outRecord->vehicleClass ) );
            hasVehicle = outRecord->vehicleClass[0] != '\0';
        } else if ( !Q_stricmpn( cursor, "best_time_ms", 12 ) ) {
            const char *value = cursor + 12;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            outRecord->bestTimeMs = G_Ghost_ParseInt( value );
            hasTime = outRecord->bestTimeMs > 0;
        } else if ( !Q_stricmpn( cursor, "frames", 6 ) ) {
            *lineEnd = savedChar;
            break; // header parsed
        }

        *lineEnd = savedChar;

        if ( savedChar == '\0' ) {
            break;
        }

        cursor = lineEnd + 1;
        if ( savedChar == '\r' && *cursor == '\n' ) {
            cursor++;
        }
    }

    if ( expectedMap && expectedMap[0] && mapName[0] && Q_stricmp( expectedMap, mapName ) ) {
        return qfalse;
    }

    return hasVehicle && hasTime;
}

void G_Ghost_InitForMap( const char *mapname ) {
    char fileList[2048];
    int fileCount;
    int offset;
    int i;

    G_Ghost_Reset();

    if ( !mapname || !mapname[0] ) {
        G_Printf( "G_Ghost: No mapname provided, skipping ghost discovery\n" );
        return;
    }

    fileCount = trap_FS_GetFileList( GHOST_DIRECTORY, GHOST_FILE_EXTENSION, fileList, sizeof( fileList ) );
    if ( fileCount <= 0 ) {
        G_Printf( "G_Ghost: No ghost files found for map %s\n", mapname );
        return;
    }

    offset = 0;
    for ( i = 0; i < fileCount && s_levelGhostCount < MAX_GHOST_RECORDS_PER_MAP; i++ ) {
        const char *filename = fileList + offset;
        char cleanName[MAX_QPATH];
        fileHandle_t f;
        int length;
        char buffer[16 * 1024 + 1];

        offset += G_Ghost_Strlen( filename ) + 1;

        if ( !filename[0] ) {
            continue;
        }

        // Expect filenames like <map>_<vehicle>.ghost
        Q_strncpyz( cleanName, filename, sizeof( cleanName ) );
        COM_StripExtension( cleanName, cleanName, sizeof( cleanName ) );
        if ( Q_stricmpn( cleanName, mapname, G_Ghost_Strlen( mapname ) ) ) {
            continue;
        }

        length = trap_FS_FOpenFile( va( "%s/%s", GHOST_DIRECTORY, filename ), &f, FS_READ );
        if ( length <= 0 ) {
            continue;
        }

        if ( length >= sizeof( buffer ) ) {
            trap_FS_FCloseFile( f );
            continue;
        }

        trap_FS_Read( buffer, length, f );
        buffer[length] = '\0';
        trap_FS_FCloseFile( f );

        if ( G_Ghost_ParseHeader( buffer, mapname, &s_levelGhosts[s_levelGhostCount] ) ) {
            Q_strncpyz( s_levelGhosts[s_levelGhostCount].path, va( "%s/%s", GHOST_DIRECTORY, filename ), sizeof( s_levelGhosts[s_levelGhostCount].path ) );
            ++s_levelGhostCount;
        }
    }

    if ( s_levelGhostCount == 0 ) {
        G_Printf( "G_Ghost: No matching ghost files for map %s\n", mapname );
    } else {
        G_Printf( "G_Ghost: Loaded %d ghost record(s) for %s\n", s_levelGhostCount, mapname );
    }
}

const ghostRecord_t *G_Ghost_FindForVehicle( const char *vehicleClass ) {
    int i;

    if ( !vehicleClass || !vehicleClass[0] ) {
        return NULL;
    }

    for ( i = 0; i < s_levelGhostCount; ++i ) {
        if ( !Q_stricmp( s_levelGhosts[i].vehicleClass, vehicleClass ) ) {
            return &s_levelGhosts[i];
        }
    }

    return NULL;
}

void G_Ghost_AnnounceForClient( gentity_t *ent ) {
    const ghostRecord_t *record;

    if ( !ent || !ent->client || ent->client->pers.connected != CON_CONNECTED ) {
        return;
    }

    record = G_Ghost_FindForVehicle( ent->client->pers.vehicleClass );

    if ( record ) {
        trap_SendServerCommand( ent - g_entities, va( "ghostmeta %s %d %s", record->vehicleClass, record->bestTimeMs, record->path ) );
    } else {
        trap_SendServerCommand( ent - g_entities, "ghostmeta none 0" );
    }
}
