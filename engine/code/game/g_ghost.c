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
static ghostBotRoute_t s_botRoutePool[MAX_GHOST_BOT_ROUTE_VARIANTS];
static int s_botRoutePoolCount = 0;
static int s_bestBotRouteIndex = -1;
static ghostBotRoute_t s_botRouteScratch;

// Shared read buffer for ghost file loading. Declared once at module level to
// avoid a 2 MB static allocation inside each function that reads ghost files.
// Safe to share: G_Ghost_ParseHeader, G_Ghost_LoadBotRouteFromFile, and the
// scan loop in G_Ghost_LoadForMap are never called concurrently (single-thread QVM).
static char s_ghostFileBuffer[MAX_GHOST_FILE_SIZE + 1];
static int G_Ghost_Strlen( const char *text );
static qboolean G_Ghost_IsRouteBetter( const ghostBotRoute_t *candidate, const ghostBotRoute_t *currentBest );
static int G_Ghost_FindRoutePoolIndexByVariant( const char *variantKey );
static int G_Ghost_SelectRoutePoolSlot( const ghostBotRoute_t *route );
static qboolean G_Ghost_RecordTimeIsBetter( int lhsTimeMs, int rhsTimeMs );
static qboolean G_Ghost_AddRecordTop5PerVariant( const ghostRecord_t *record );

static int G_Ghost_GetTrackLengthVariant( void ) {
    if ( g_trackLength.integer < 0 || g_trackLength.integer > 2 ) {
        return 0;
    }

    return g_trackLength.integer;
}

static int G_Ghost_GetTrackReversedVariant( void ) {
    return g_trackReversed.integer ? 1 : 0;
}

static qboolean G_Ghost_FilenameMatchesVariant( const char *filenameNoExt, const char *mapname, int trackLength, int trackReversed ) {
    int mapLen;
    char variantPrefix[32];

    if ( !filenameNoExt || !filenameNoExt[0] || !mapname || !mapname[0] ) {
        return qfalse;
    }

    mapLen = G_Ghost_Strlen( mapname );
    if ( Q_stricmpn( filenameNoExt, mapname, mapLen ) ) {
        return qfalse;
    }

    Com_sprintf( variantPrefix, sizeof( variantPrefix ), "_tl%d_rev%d_", trackLength, trackReversed );
    return !Q_stricmpn( filenameNoExt + mapLen, variantPrefix, G_Ghost_Strlen( variantPrefix ) );
}

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
    Com_Memset( s_botRoutePool, 0, sizeof( s_botRoutePool ) );
    Com_Memset( &s_botRouteScratch, 0, sizeof( s_botRouteScratch ) );
    s_levelGhostCount = 0;
    s_botRoutePoolCount = 0;
    s_bestBotRouteIndex = -1;
}

static qboolean G_Ghost_IsRouteBetter( const ghostBotRoute_t *candidate, const ghostBotRoute_t *currentBest ) {
    if ( !candidate || !candidate->valid ) {
        return qfalse;
    }

    if ( !currentBest || !currentBest->valid ) {
        return qtrue;
    }

    if ( candidate->bestTimeMs > 0 ) {
        if ( currentBest->bestTimeMs <= 0 || candidate->bestTimeMs < currentBest->bestTimeMs ) {
            return qtrue;
        }
    }

    return qfalse;
}

static int G_Ghost_FindRoutePoolIndexByVariant( const char *variantKey ) {
    int i;
    const char *lookupKey = variantKey && variantKey[0] ? variantKey : "any";

    for ( i = 0; i < s_botRoutePoolCount; ++i ) {
        const char *candidateKey = s_botRoutePool[i].vehicleClass[0] ? s_botRoutePool[i].vehicleClass : "any";
        if ( !Q_stricmp( lookupKey, candidateKey ) ) {
            return i;
        }
    }

    return -1;
}

static int G_Ghost_SelectRoutePoolSlot( const ghostBotRoute_t *route ) {
    int existingIndex;

    if ( !route || !route->valid ) {
        return -1;
    }

    existingIndex = G_Ghost_FindRoutePoolIndexByVariant( route->vehicleClass );
    if ( existingIndex >= 0 ) {
        return existingIndex;
    }

    if ( s_botRoutePoolCount < MAX_GHOST_BOT_ROUTE_VARIANTS ) {
        return s_botRoutePoolCount++;
    }

    return -1;
}

static qboolean G_Ghost_RecordTimeIsBetter( int lhsTimeMs, int rhsTimeMs ) {
    if ( lhsTimeMs > 0 ) {
        if ( rhsTimeMs <= 0 || lhsTimeMs < rhsTimeMs ) {
            return qtrue;
        }
    }
    return qfalse;
}

static qboolean G_Ghost_AddRecordTop5PerVariant( const ghostRecord_t *record ) {
    int i;
    int variantCount = 0;
    int worstIndex = -1;
    const char *recordVariant;

    if ( !record ) {
        return qfalse;
    }

    recordVariant = record->vehicleClass[0] ? record->vehicleClass : "any";

    for ( i = 0; i < s_levelGhostCount; ++i ) {
        const char *candidateVariant = s_levelGhosts[i].vehicleClass[0] ? s_levelGhosts[i].vehicleClass : "any";
        if ( Q_stricmp( recordVariant, candidateVariant ) ) {
            continue;
        }

        ++variantCount;
        if ( worstIndex < 0 || G_Ghost_RecordTimeIsBetter( s_levelGhosts[worstIndex].bestTimeMs, s_levelGhosts[i].bestTimeMs ) ) {
            worstIndex = i;
        }
    }

    if ( variantCount < 5 ) {
        if ( s_levelGhostCount >= MAX_GHOST_RECORDS_PER_MAP ) {
            return qfalse;
        }
        s_levelGhosts[s_levelGhostCount] = *record;
        ++s_levelGhostCount;
        return qtrue;
    }

    if ( worstIndex < 0 || !G_Ghost_RecordTimeIsBetter( record->bestTimeMs, s_levelGhosts[worstIndex].bestTimeMs ) ) {
        return qfalse;
    }

    s_levelGhosts[worstIndex] = *record;
    return qtrue;
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

static qboolean G_Ghost_ParseHeader( char *buffer, const char *expectedMap, int expectedTrackLength, int expectedTrackReversed, qboolean allowNoMapHeader, ghostRecord_t *outRecord ) {
    char *cursor;
    char *line;
    char mapName[MAX_QPATH] = "";
    int trackLength = -1;
    int trackReversed = -1;
    qboolean hasMapHeader = qfalse;
    qboolean hasTrackLength = qfalse;
    qboolean hasTrackReversed = qfalse;

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
        } else if ( !Q_stricmpn( line, "track_length", 12 ) ) {
            const char *value = line + 12;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            trackLength = G_Ghost_ParseInt( value );
            hasTrackLength = qtrue;
        } else if ( !Q_stricmpn( line, "track_reversed", 14 ) ) {
            const char *value = line + 14;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            trackReversed = G_Ghost_ParseInt( value ) ? 1 : 0;
            hasTrackReversed = qtrue;
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

    if ( expectedTrackLength >= 0 && ( trackLength < 0 || trackLength != expectedTrackLength ) ) {
        return qfalse;
    }

    if ( expectedTrackReversed >= 0 && ( trackReversed < 0 || trackReversed != expectedTrackReversed ) ) {
        return qfalse;
    }

    outRecord->hasVariantData = hasTrackLength && hasTrackReversed;
    outRecord->ambiguousLegacy = qfalse;
    return qtrue;
}

static qboolean G_Ghost_LoadBotRouteFromFile( const ghostRecord_t *record, ghostBotRoute_t *outRoute ) {
    fileHandle_t f;
    int length;
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

    trap_FS_Read( s_ghostFileBuffer, length, f );
    trap_FS_FCloseFile( f );
    s_ghostFileBuffer[length] = '\0';

    Com_Memset( outRoute, 0, sizeof( *outRoute ) );
    Q_strncpyz( outRoute->path, record->path, sizeof( outRoute->path ) );

    cursor = s_ghostFileBuffer;
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
    const char *ghostDirectories[] = { GHOST_DIRECTORY, "maps" };
    char fileList[2048];
    int fileCount;
    int offset;
    int i;
    int dirIndex;
    int trackLength = G_Ghost_GetTrackLengthVariant();
    int trackReversed = G_Ghost_GetTrackReversedVariant();

    G_Ghost_Reset();

    if ( !mapname || !mapname[0] ) {
        G_Printf( "G_Ghost: No mapname provided, skipping ghost discovery\n" );
        return;
    }

    {
        int discoveredFiles = 0;

        for ( dirIndex = 0; dirIndex < (int)( sizeof( ghostDirectories ) / sizeof( ghostDirectories[0] ) ); ++dirIndex ) {
            const char *ghostDir = ghostDirectories[dirIndex];

            fileCount = trap_FS_GetFileList( ghostDir, GHOST_FILE_EXTENSION, fileList, sizeof( fileList ) );
            if ( fileCount <= 0 ) {
                continue;
            }

            discoveredFiles += fileCount;
            offset = 0;
            for ( i = 0; i < fileCount; i++ ) {
                const char *filename = fileList + offset;
                char cleanName[MAX_QPATH];
                qboolean filenameLooksLikeVariant;
                fileHandle_t f;
                int length;
                ghostRecord_t parsedRecord;

                offset += G_Ghost_Strlen( filename ) + 1;

                if ( !filename[0] ) {
                    continue;
                }

                Q_strncpyz( cleanName, filename, sizeof( cleanName ) );
                COM_StripExtension( cleanName, cleanName, sizeof( cleanName ) );
                filenameLooksLikeVariant = G_Ghost_FilenameMatchesVariant( cleanName, mapname, trackLength, trackReversed );
                if ( !filenameLooksLikeVariant ) {
                    continue;
                }

                length = trap_FS_FOpenFile( va( "%s/%s", ghostDir, filename ), &f, FS_READ );
                if ( length <= 0 ) {
                    continue;
                }

                {
                    int readLen = length < (int)sizeof( s_ghostFileBuffer ) - 1 ? length : (int)sizeof( s_ghostFileBuffer ) - 1;
                    trap_FS_Read( s_ghostFileBuffer, readLen, f );
                    s_ghostFileBuffer[readLen] = '\0';
                }
                trap_FS_FCloseFile( f );

                if ( G_Ghost_ParseHeader( s_ghostFileBuffer, mapname, trackLength, trackReversed, qfalse, &s_levelGhosts[s_levelGhostCount] ) ) {
                    parsedRecord = s_levelGhosts[s_levelGhostCount];
                    Q_strncpyz( parsedRecord.path, va( "%s/%s", ghostDir, filename ), sizeof( parsedRecord.path ) );
                    G_Ghost_AddRecordTop5PerVariant( &parsedRecord );
                }
            }
        }

        if ( discoveredFiles <= 0 ) {
            G_Printf( "G_Ghost: No ghost files found for map %s\n", mapname );
            return;
        }
    }

    if ( s_levelGhostCount == 0 ) {
        int legacyCandidates = 0;
        int legacyAnyAmbiguous = 0;

        for ( dirIndex = 0; dirIndex < (int)( sizeof( ghostDirectories ) / sizeof( ghostDirectories[0] ) ); ++dirIndex ) {
            const char *ghostDir = ghostDirectories[dirIndex];

            fileCount = trap_FS_GetFileList( ghostDir, GHOST_FILE_EXTENSION, fileList, sizeof( fileList ) );
            if ( fileCount <= 0 ) {
                continue;
            }

            offset = 0;
            for ( i = 0; i < fileCount; i++ ) {
                const char *filename = fileList + offset;
                char cleanName[MAX_QPATH];
                fileHandle_t f;
                int length;
                ghostRecord_t parsedRecord;
                int mapLen;

                offset += G_Ghost_Strlen( filename ) + 1;
                if ( !filename[0] ) {
                    continue;
                }

                Q_strncpyz( cleanName, filename, sizeof( cleanName ) );
                COM_StripExtension( cleanName, cleanName, sizeof( cleanName ) );
                mapLen = G_Ghost_Strlen( mapname );
                if ( Q_stricmpn( cleanName, mapname, mapLen ) ) {
                    continue;
                }
                if ( G_Ghost_FilenameMatchesVariant( cleanName, mapname, trackLength, trackReversed ) ) {
                    continue;
                }

                length = trap_FS_FOpenFile( va( "%s/%s", ghostDir, filename ), &f, FS_READ );
                if ( length <= 0 ) {
                    continue;
                }
                {
                    int readLen = length < (int)sizeof( s_ghostFileBuffer ) - 1 ? length : (int)sizeof( s_ghostFileBuffer ) - 1;
                    trap_FS_Read( s_ghostFileBuffer, readLen, f );
                    s_ghostFileBuffer[readLen] = '\0';
                }
                trap_FS_FCloseFile( f );

                if ( !G_Ghost_ParseHeader( s_ghostFileBuffer, mapname, -1, -1, qtrue, &parsedRecord ) ) {
                    continue;
                }
                if ( parsedRecord.hasVariantData ) {
                    continue;
                }

                Q_strncpyz( parsedRecord.path, va( "%s/%s", ghostDir, filename ), sizeof( parsedRecord.path ) );
                if ( G_Ghost_AddRecordTop5PerVariant( &parsedRecord ) ) {
                    ++legacyCandidates;
                }
            }
        }

        if ( s_levelGhostCount > 0 ) {
            for ( i = 0; i < s_levelGhostCount; ++i ) {
                int j;
                int variantMatches = 0;
                const char *variantKey = s_levelGhosts[i].vehicleClass[0] ? s_levelGhosts[i].vehicleClass : "any";

                for ( j = 0; j < s_levelGhostCount; ++j ) {
                    const char *otherKey = s_levelGhosts[j].vehicleClass[0] ? s_levelGhosts[j].vehicleClass : "any";
                    if ( !Q_stricmp( variantKey, otherKey ) ) {
                        ++variantMatches;
                    }
                }
                if ( variantMatches > 1 ) {
                    s_levelGhosts[i].ambiguousLegacy = qtrue;
                    legacyAnyAmbiguous = 1;
                }
            }

            G_Printf( "G_Ghost: Legacy fallback loaded %d ghost(s)%s for %s\n",
                legacyCandidates,
                legacyAnyAmbiguous ? " (ambiguous variants marked)" : "",
                mapname );
        } else {
            G_Printf( "G_Ghost: No matching ghost files for map %s\n", mapname );
        }
    } else {
        G_Printf( "G_Ghost: Loaded %d ghost record(s) for %s\n", s_levelGhostCount, mapname );

        for ( i = 0; i < s_levelGhostCount; ++i ) {
            int poolSlot;

            if ( s_levelGhosts[i].ambiguousLegacy ) {
                continue;
            }

            if ( !G_Ghost_LoadBotRouteFromFile( &s_levelGhosts[i], &s_botRouteScratch ) ) {
                continue;
            }

            poolSlot = G_Ghost_SelectRoutePoolSlot( &s_botRouteScratch );
            if ( poolSlot < 0 ) {
                G_Printf( "G_Ghost: route pool full, dropping variant %s from %s\n",
                    s_botRouteScratch.vehicleClass[0] ? s_botRouteScratch.vehicleClass : "any",
                    s_botRouteScratch.path );
                continue;
            }

            if ( G_Ghost_IsRouteBetter( &s_botRouteScratch, &s_botRoutePool[poolSlot] ) ) {
                s_botRoutePool[poolSlot] = s_botRouteScratch;
            }

            if ( s_bestBotRouteIndex < 0 || G_Ghost_IsRouteBetter( &s_botRoutePool[poolSlot], &s_botRoutePool[s_bestBotRouteIndex] ) ) {
                s_bestBotRouteIndex = poolSlot;
            }
        }

        if ( s_bestBotRouteIndex >= 0 ) {
            G_Printf( "G_Ghost: Bot route pool ready (%d variant(s), fallback=%s)\n",
                s_botRoutePoolCount,
                s_botRoutePool[s_bestBotRouteIndex].path );
        } else {
            G_Printf( "G_Ghost: Bot route unavailable for map %s\n", mapname );
        }
    }
}

int G_Ghost_SelectClosestWaypoint( const ghostBotRoute_t *route, const vec3_t origin, int hintIndex, int hintWindow ) {
    int i;
    int bestIndex = -1;
    float bestDistSq = 0.0f;
    int searchStart = 0;
    int searchEnd;

    if ( !route || !route->valid || route->numWaypoints <= 0 || !origin ) {
        return -1;
    }

    searchEnd = route->numWaypoints - 1;
    if ( hintWindow <= 0 ) {
        hintWindow = 24;
    }

    if ( hintIndex >= 0 && hintIndex < route->numWaypoints ) {
        searchStart = hintIndex - hintWindow;
        searchEnd = hintIndex + hintWindow;
        if ( searchStart < 0 ) {
            searchStart = 0;
        }
        if ( searchEnd >= route->numWaypoints ) {
            searchEnd = route->numWaypoints - 1;
        }
    }

    for ( i = searchStart; i <= searchEnd; ++i ) {
        vec3_t deltaToWaypoint;
        float distSq;
        VectorSubtract( route->waypoints[i].origin, origin, deltaToWaypoint );
        distSq = VectorLengthSquared( deltaToWaypoint );
        if ( bestIndex < 0 || distSq < bestDistSq ) {
            bestIndex = i;
            bestDistSq = distSq;
        }
    }

    if ( hintIndex >= 0 && bestIndex >= 0 ) {
        int minBackwardIndex = hintIndex - 3;
        if ( minBackwardIndex < 0 ) {
            minBackwardIndex = 0;
        }
        if ( bestIndex < minBackwardIndex ) {
            bestIndex = minBackwardIndex;
        }
    }

    return bestIndex;
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
    return G_Ghost_GetBotRouteForVariant( NULL, outRoute );
}

qboolean G_Ghost_GetBotRouteForVariant( const char *variantKey, const ghostBotRoute_t **outRoute ) {
    int poolIndex = -1;

    if ( !outRoute ) {
        return qfalse;
    }

    if ( variantKey && variantKey[0] ) {
        poolIndex = G_Ghost_FindRoutePoolIndexByVariant( variantKey );
    }

    if ( poolIndex < 0 ) {
        poolIndex = s_bestBotRouteIndex;
    }

    if ( poolIndex < 0 || poolIndex >= s_botRoutePoolCount || !s_botRoutePool[poolIndex].valid ) {
        return qfalse;
    }

    *outRoute = &s_botRoutePool[poolIndex];
    return qtrue;
}

#ifdef UNIT_TEST
int G_Ghost_Test_GetLevelGhostCount( void ) {
    return s_levelGhostCount;
}

const ghostRecord_t *G_Ghost_Test_GetLevelGhost( int index ) {
    if ( index < 0 || index >= s_levelGhostCount ) {
        return NULL;
    }
    return &s_levelGhosts[index];
}
#endif

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
