#include "g_local.h"

#define MAX_GHOST_RECORDS_PER_MAP 32
#define GHOST_FILE_EXTENSION ".ghost"
#define GHOST_DIRECTORY "ghosts"
#define MAX_GHOST_FILE_SIZE ( 2 * 1024 * 1024 )

// Ghost frames are recorded at ~6ms intervals. At up to 200 km/h (~2200 units/s)
// consecutive frames are only ~13 units apart, which causes bots to crawl.
// Keep every Nth frame to get ~240 units between waypoints for smooth navigation.
#define GHOST_WAYPOINT_STRIDE 10

static ghostRecord_t s_levelGhosts[MAX_GHOST_RECORDS_PER_MAP];
static int s_levelGhostCount = 0;
static ghostBotRoute_t s_botRoute;
static ghostBotRoute_t s_botRouteScratch;

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

static float G_Ghost_ParseFloat( const char **text ) {
    float value = 0.0f;
    float frac = 0.0f;
    float divisor = 1.0f;
    int negative = 0;

    if ( !text || !*text ) {
        return 0.0f;
    }

    while ( **text == ' ' || **text == '\t' ) {
        ++( *text );
    }

    if ( **text == '-' ) {
        negative = 1;
        ++( *text );
    } else if ( **text == '+' ) {
        ++( *text );
    }

    while ( **text >= '0' && **text <= '9' ) {
        value = value * 10.0f + ( **text - '0' );
        ++( *text );
    }

    if ( **text == '.' ) {
        ++( *text );
        while ( **text >= '0' && **text <= '9' ) {
            frac = frac * 10.0f + ( **text - '0' );
            divisor *= 10.0f;
            ++( *text );
        }
        value += frac / divisor;
    }

    return negative ? -value : value;
}

static void G_Ghost_Reset( void ) {
    Com_Memset( s_levelGhosts, 0, sizeof( s_levelGhosts ) );
    Com_Memset( &s_botRoute, 0, sizeof( s_botRoute ) );
    Com_Memset( &s_botRouteScratch, 0, sizeof( s_botRouteScratch ) );
    s_levelGhostCount = 0;
}

static char *G_Ghost_NextLine( char **cursor ) {
    char *line;
    char *end;

    if ( !cursor || !*cursor || !( **cursor ) ) {
        return NULL;
    }

    line = *cursor;

    while ( *line == ' ' || *line == '\t' ) {
        ++line;
    }

    if ( line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF' ) {
        line += 3;
    }

    end = line;
    while ( *end && *end != '\n' && *end != '\r' ) {
        ++end;
    }

    if ( *end ) {
        char saved = *end;
        *end = '\0';
        *cursor = end + 1;
        if ( saved == '\r' && **cursor == '\n' ) {
            ++( *cursor );
        }
    } else {
        *cursor = end;
    }

    // Strip trailing \r for Windows line endings (\r\n read as single \n)
    {
        int trimLen = G_Ghost_Strlen( line );
        while ( trimLen > 0 && line[trimLen - 1] == '\r' ) {
            line[--trimLen] = '\0';
        }
    }

    return line;
}

static qboolean G_Ghost_ParseHeader( char *buffer, const char *expectedMap, qboolean allowNoMapHeader, ghostRecord_t *outRecord ) {
    char *cursor;
    char *line;
    char mapName[MAX_QPATH] = "";
    qboolean hasMapHeader = qfalse;

    if ( !buffer || !outRecord ) {
        return qfalse;
    }

    cursor = buffer;
    while ( ( line = G_Ghost_NextLine( &cursor ) ) != NULL ) {
        if ( line[0] == '#' || line[0] == '\0' ) {
            continue;
        }

        if ( !Q_stricmpn( line, "map", 3 ) ) {
            const char *value = line + 3;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( mapName, value, sizeof( mapName ) );
            hasMapHeader = qtrue;
        } else if ( !Q_stricmpn( line, "vehicle", 7 ) ) {
            const char *value = line + 7;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( outRecord->vehicleClass, value, sizeof( outRecord->vehicleClass ) );
        } else if ( !Q_stricmpn( line, "best_time_ms", 12 ) ) {
            const char *value = line + 12;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            outRecord->bestTimeMs = G_Ghost_ParseInt( value );
        } else if ( !Q_stricmpn( line, "frames", 6 ) ) {
            break;
        }
    }

    if ( expectedMap && expectedMap[0] ) {
        if ( hasMapHeader ) {
            if ( Q_stricmp( expectedMap, mapName ) ) {
                return qfalse;
            }
        } else if ( !allowNoMapHeader ) {
            return qfalse;
        }
    }

    return qtrue;
}

static qboolean G_Ghost_LoadBotRouteFromFile( const ghostRecord_t *record, ghostBotRoute_t *outRoute ) {
    fileHandle_t f;
    int length;
    static char buffer[MAX_GHOST_FILE_SIZE + 1];
    char *cursor;
    char *line;
    char mapName[MAX_QPATH] = "";
    int frameCount = 0;
    ghostWaypoint_t lastWp;
    qboolean hasLastWp = qfalse;

    if ( !record || !record->path[0] || !outRoute ) {
        return qfalse;
    }

    length = trap_FS_FOpenFile( record->path, &f, FS_READ );
    if ( length <= 0 ) {
        G_Printf( "G_Ghost: could not open %s for bot route\n", record->path );
        return qfalse;
    }

    if ( length > MAX_GHOST_FILE_SIZE ) {
        trap_FS_FCloseFile( f );
        G_Printf( "G_Ghost: %s too large for bot route (%d bytes)\n", record->path, length );
        return qfalse;
    }

    trap_FS_Read( buffer, length, f );
    trap_FS_FCloseFile( f );
    buffer[length] = '\0';

    Com_Memset( outRoute, 0, sizeof( *outRoute ) );
    Q_strncpyz( outRoute->path, record->path, sizeof( outRoute->path ) );

    cursor = buffer;
    while ( ( line = G_Ghost_NextLine( &cursor ) ) != NULL ) {
        if ( line[0] == '#' || line[0] == '\0' ) {
            continue;
        }

        if ( !Q_stricmpn( line, "map", 3 ) ) {
            const char *value = line + 3;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( mapName, value, sizeof( mapName ) );
            continue;
        }

        if ( !Q_stricmpn( line, "vehicle", 7 ) ) {
            const char *value = line + 7;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( outRoute->vehicleClass, value, sizeof( outRoute->vehicleClass ) );
            continue;
        }

        if ( !Q_stricmpn( line, "best_time_ms", 12 ) ) {
            const char *value = line + 12;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            outRoute->bestTimeMs = G_Ghost_ParseInt( value );
            continue;
        }

        if ( !Q_stricmpn( line, "frames", 6 ) ) {
            continue;
        }

        {
            const char *p = line;
            ghostWaypoint_t wp;

            // Require a digit to start (skip non-frame lines)
            while ( *p == ' ' || *p == '\t' ) {
                ++p;
            }
            if ( *p < '0' || *p > '9' ) {
                continue;
            }

            // Parse timeOffset
            wp.timeOffset = 0;
            while ( *p >= '0' && *p <= '9' ) {
                wp.timeOffset = wp.timeOffset * 10 + ( *p - '0' );
                ++p;
            }

            // Parse x, y, z (remaining fields are ignored)
            wp.origin[0] = G_Ghost_ParseFloat( &p );
            wp.origin[1] = G_Ghost_ParseFloat( &p );
            wp.origin[2] = G_Ghost_ParseFloat( &p );

            if ( *p != ' ' && *p != '\t' && *p != '\0' ) {
                continue;
            }

            // Keep every GHOST_WAYPOINT_STRIDE-th frame; always keep the first
            if ( frameCount == 0 || ( frameCount % GHOST_WAYPOINT_STRIDE ) == 0 ) {
                if ( outRoute->numWaypoints < MAX_GHOST_BOT_WAYPOINTS ) {
                    outRoute->waypoints[outRoute->numWaypoints] = wp;
                    outRoute->numWaypoints++;
                }
            }

            // Track the very last valid frame so we can append it as the final waypoint
            lastWp = wp;
            hasLastWp = qtrue;
            frameCount++;
        }
    }

    // Always append the last frame so the route ends exactly at the finish line
    if ( hasLastWp && outRoute->numWaypoints > 0 ) {
        ghostWaypoint_t *prev = &outRoute->waypoints[outRoute->numWaypoints - 1];
        if ( prev->timeOffset != lastWp.timeOffset ) {
            if ( outRoute->numWaypoints < MAX_GHOST_BOT_WAYPOINTS ) {
                outRoute->waypoints[outRoute->numWaypoints] = lastWp;
                outRoute->numWaypoints++;
            }
        }
    }

    if ( outRoute->numWaypoints < 2 ) {
        G_Printf( "G_Ghost: %s has no usable bot waypoints\n", record->path );
        Com_Memset( outRoute, 0, sizeof( *outRoute ) );
        return qfalse;
    }

    if ( outRoute->bestTimeMs <= 0 ) {
        int startTime = outRoute->waypoints[0].timeOffset;
        int endTime = outRoute->waypoints[outRoute->numWaypoints - 1].timeOffset;
        if ( endTime > startTime ) {
            outRoute->bestTimeMs = endTime - startTime;
        }
    }

    outRoute->valid = qtrue;
    G_Printf( "G_Ghost: Bot route ready from %s (%d waypoints, vehicle=%s, map=%s)\n",
        record->path,
        outRoute->numWaypoints,
        outRoute->vehicleClass[0] ? outRoute->vehicleClass : "any",
        mapName[0] ? mapName : "unknown" );

    return qtrue;
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

    {
        const char *ghostDirectories[] = { GHOST_DIRECTORY, "maps" };
        int dirIndex;
        int discoveredFiles = 0;

        for ( dirIndex = 0; dirIndex < (int)( sizeof( ghostDirectories ) / sizeof( ghostDirectories[0] ) ) && s_levelGhostCount < MAX_GHOST_RECORDS_PER_MAP; ++dirIndex ) {
            const char *ghostDir = ghostDirectories[dirIndex];

            fileCount = trap_FS_GetFileList( ghostDir, GHOST_FILE_EXTENSION, fileList, sizeof( fileList ) );
            if ( fileCount <= 0 ) {
                continue;
            }

            discoveredFiles += fileCount;
            offset = 0;
            for ( i = 0; i < fileCount && s_levelGhostCount < MAX_GHOST_RECORDS_PER_MAP; i++ ) {
                const char *filename = fileList + offset;
                char cleanName[MAX_QPATH];
                qboolean filenameLooksLikeMap;
                fileHandle_t f;
                int length;
                char buffer[16 * 1024 + 1];

                offset += G_Ghost_Strlen( filename ) + 1;

                if ( !filename[0] ) {
                    continue;
                }

                Q_strncpyz( cleanName, filename, sizeof( cleanName ) );
                COM_StripExtension( cleanName, cleanName, sizeof( cleanName ) );
                filenameLooksLikeMap = ( !Q_stricmp( cleanName, mapname ) ||
                    ( !Q_stricmpn( cleanName, mapname, G_Ghost_Strlen( mapname ) ) && cleanName[G_Ghost_Strlen( mapname )] == '_' ) );

                length = trap_FS_FOpenFile( va( "%s/%s", ghostDir, filename ), &f, FS_READ );
                if ( length <= 0 ) {
                    continue;
                }

                {
                    int readLen = length < (int)sizeof( buffer ) - 1 ? length : (int)sizeof( buffer ) - 1;
                    trap_FS_Read( buffer, readLen, f );
                    buffer[readLen] = '\0';
                }
                trap_FS_FCloseFile( f );

                if ( G_Ghost_ParseHeader( buffer, mapname, filenameLooksLikeMap, &s_levelGhosts[s_levelGhostCount] ) ) {
                    Q_strncpyz( s_levelGhosts[s_levelGhostCount].path, va( "%s/%s", ghostDir, filename ), sizeof( s_levelGhosts[s_levelGhostCount].path ) );
                    ++s_levelGhostCount;
                }
            }
        }

        if ( discoveredFiles <= 0 ) {
            G_Printf( "G_Ghost: No ghost files found for map %s\n", mapname );
            return;
        }
    }

    if ( s_levelGhostCount == 0 ) {
        G_Printf( "G_Ghost: No matching ghost files for map %s\n", mapname );
    } else {
        qboolean foundRoute = qfalse;
        int bestRouteTime = 0;

        G_Printf( "G_Ghost: Loaded %d ghost record(s) for %s\n", s_levelGhostCount, mapname );

        for ( i = 0; i < s_levelGhostCount; ++i ) {
            if ( !G_Ghost_LoadBotRouteFromFile( &s_levelGhosts[i], &s_botRouteScratch ) ) {
                continue;
            }

            if ( !foundRoute ) {
                s_botRoute = s_botRouteScratch;
                bestRouteTime = s_botRouteScratch.bestTimeMs;
                foundRoute = qtrue;
                continue;
            }

            if ( s_botRouteScratch.bestTimeMs > 0 && ( bestRouteTime <= 0 || s_botRouteScratch.bestTimeMs < bestRouteTime ) ) {
                s_botRoute = s_botRouteScratch;
                bestRouteTime = s_botRouteScratch.bestTimeMs;
            }
        }

        if ( foundRoute ) {
            G_Printf( "G_Ghost: Bot route source set to %s\n", s_botRoute.path );
        } else {
            G_Printf( "G_Ghost: Bot route unavailable for map %s\n", mapname );
        }
    }
}

const ghostRecord_t *G_Ghost_FindBestRecord( void ) {
    int i;
    const ghostRecord_t *best = NULL;

    for ( i = 0; i < s_levelGhostCount; ++i ) {
        const ghostRecord_t *candidate = &s_levelGhosts[i];

        if ( !best ) {
            best = candidate;
            continue;
        }

        if ( candidate->bestTimeMs > 0 ) {
            if ( best->bestTimeMs <= 0 || candidate->bestTimeMs < best->bestTimeMs ) {
                best = candidate;
            }
        }
    }

    return best;
}

qboolean G_Ghost_GetBotRoute( const ghostBotRoute_t **outRoute ) {
    if ( !outRoute || !s_botRoute.valid ) {
        return qfalse;
    }

    *outRoute = &s_botRoute;
    return qtrue;
}

void G_Ghost_AnnounceForClient( gentity_t *ent ) {
    const ghostRecord_t *record;

    if ( !ent || !ent->client || ent->client->pers.connected != CON_CONNECTED ) {
        return;
    }

    record = G_Ghost_FindBestRecord();

    if ( record ) {
        trap_SendServerCommand( ent - g_entities, va( "ghostmeta %s %d %s", record->vehicleClass[0] ? record->vehicleClass : "any", record->bestTimeMs, record->path ) );
    } else {
        trap_SendServerCommand( ent - g_entities, "ghostmeta none 0" );
    }
}
