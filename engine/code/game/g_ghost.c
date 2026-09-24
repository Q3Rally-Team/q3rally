#include "g_local.h"

#define MAX_GHOST_RECORDS_PER_MAP 32
#define GHOST_FILE_EXTENSION ".ghost"
#define GHOST_DIRECTORY "ghosts"
#define MAX_GHOST_FILE_SIZE ( 2 * 1024 * 1024 )

#define GHOST_WAYPOINT_MIN_DISTANCE 96.0f
#define GHOST_WAYPOINT_MAX_INTERVAL 250
#define GHOST_WAYPOINT_TURN_DEGREES 10.0f
#define MAX_GHOST_CLIENT_SAMPLES 512
#define GHOST_CLIENT_SAMPLES_PER_COMMAND 16

static ghostRecord_t s_levelGhosts[MAX_GHOST_RECORDS_PER_MAP];
static int s_levelGhostCount = 0;
static ghostBotRoute_t s_botRoute;
static qboolean s_botRoutesBuilt = qfalse;
static ghostBotRoute_t s_botRouteScratch;
static qboolean s_clientGhostTransferPending[MAX_CLIENTS];
static qboolean s_clientGhostTransferStarted[MAX_CLIENTS];
static qboolean s_clientGhostTransferSent[MAX_CLIENTS];
static int s_clientGhostTransferNext[MAX_CLIENTS];
static int s_clientGhostTransferQueuedAt[MAX_CLIENTS];
static int s_clientGhostSampleCount;
static int s_clientGhostSampleIndices[MAX_GHOST_CLIENT_SAMPLES];
static qboolean s_clientGhostSamplesBuilt;
static byte s_clientGhostSampleSelected[MAX_GHOST_BOT_WAYPOINTS];

// Shared read buffer for ghost file loading. Declared once at module level to
// avoid a 2 MB static allocation inside each function that reads ghost files.
// Safe to share: G_Ghost_ParseHeader, G_Ghost_LoadBotRouteFromFile, and the
// scan loop in G_Ghost_LoadForMap are never called concurrently (single-thread QVM).
static char s_ghostFileBuffer[MAX_GHOST_FILE_SIZE + 1];
static int G_Ghost_Strlen( const char *text );
static qboolean G_Ghost_IsRouteBetter( const ghostBotRoute_t *candidate, const ghostBotRoute_t *currentBest );
static qboolean G_Ghost_RecordTimeIsBetter( int lhsTimeMs, int rhsTimeMs );
static qboolean G_Ghost_AddRecordTop5ForTrackVariant( const ghostRecord_t *record );
static void G_Ghost_BuildRouteSegments( ghostBotRoute_t *route );

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

static qboolean G_Ghost_LineMatchesKey( const char *line, const char *key ) {
    int keyLen;
    char delimiter;

    if ( !line || !key || !key[0] ) {
        return qfalse;
    }

    keyLen = G_Ghost_Strlen( key );
    if ( Q_stricmpn( line, key, keyLen ) ) {
        return qfalse;
    }

    delimiter = line[keyLen];
    return delimiter == '\0' || delimiter == ' ' || delimiter == '\t';
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
    Com_Memset( s_clientGhostTransferPending, 0, sizeof( s_clientGhostTransferPending ) );
    Com_Memset( s_clientGhostTransferStarted, 0, sizeof( s_clientGhostTransferStarted ) );
    Com_Memset( s_clientGhostTransferSent, 0, sizeof( s_clientGhostTransferSent ) );
    Com_Memset( s_clientGhostTransferNext, 0, sizeof( s_clientGhostTransferNext ) );
    Com_Memset( s_clientGhostTransferQueuedAt, 0, sizeof( s_clientGhostTransferQueuedAt ) );
    s_clientGhostSampleCount = 0;
    s_clientGhostSamplesBuilt = qfalse;
    s_levelGhostCount = 0;
    s_botRoutesBuilt = qfalse;
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

static qboolean G_Ghost_RecordTimeIsBetter( int lhsTimeMs, int rhsTimeMs ) {
    if ( lhsTimeMs > 0 ) {
        if ( rhsTimeMs <= 0 || lhsTimeMs < rhsTimeMs ) {
            return qtrue;
        }
    }
    return qfalse;
}

static qboolean G_Ghost_AddRecordTop5ForTrackVariant( const ghostRecord_t *record ) {
    int i;
    int worstIndex = -1;

    if ( !record ) {
        return qfalse;
    }

    for ( i = 0; i < s_levelGhostCount; ++i ) {
        if ( worstIndex < 0 || G_Ghost_RecordTimeIsBetter( s_levelGhosts[worstIndex].bestTimeMs, s_levelGhosts[i].bestTimeMs ) ) {
            worstIndex = i;
        }
    }

    if ( s_levelGhostCount < 5 ) {
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

    Com_Memset( outRecord, 0, sizeof( *outRecord ) );

    cursor = buffer;
    while ( ( line = G_Ghost_NextLine( &cursor ) ) != NULL ) {
        if ( line[0] == '#' || line[0] == '\0' ) {
            continue;
        }

        if ( G_Ghost_LineMatchesKey( line, "map" ) ) {
            const char *value = line + 3;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( mapName, value, sizeof( mapName ) );
            hasMapHeader = qtrue;
        } else if ( G_Ghost_LineMatchesKey( line, "vehicle" ) ) {
            const char *value = line + 7;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( outRecord->vehicleClass, value, sizeof( outRecord->vehicleClass ) );
        } else if ( G_Ghost_LineMatchesKey( line, "best_time_ms" ) ) {
            const char *value = line + 12;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            outRecord->bestTimeMs = G_Ghost_ParseInt( value );
        } else if ( G_Ghost_LineMatchesKey( line, "track_length" ) ) {
            const char *value = line + 12;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            trackLength = G_Ghost_ParseInt( value );
            hasTrackLength = qtrue;
        } else if ( G_Ghost_LineMatchesKey( line, "track_reversed" ) ) {
            const char *value = line + 14;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            trackReversed = G_Ghost_ParseInt( value ) ? 1 : 0;
            hasTrackReversed = qtrue;
        } else if ( G_Ghost_LineMatchesKey( line, "frames" ) ) {
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

static void G_Ghost_CompactBotRoute( ghostBotRoute_t *route ) {
    int readIndex;
    int writeIndex = 0;
    int ordinaryIndex = 0;
    int originalCount;

    if ( !route || route->numWaypoints < 4 ) {
        return;
    }

    originalCount = route->numWaypoints;
    for ( readIndex = 0; readIndex < originalCount; ++readIndex ) {
        ghostWaypoint_t *waypoint = &route->waypoints[readIndex];
        qboolean keep = waypoint->required || readIndex == 0 || readIndex == originalCount - 1;

        if ( !keep ) {
            keep = ( ordinaryIndex % 2 == 0 ) ? qtrue : qfalse;
            ordinaryIndex++;
        }

        if ( keep ) {
            if ( writeIndex != readIndex ) {
                route->waypoints[writeIndex] = *waypoint;
            }
            writeIndex++;
        }
    }

    route->numWaypoints = writeIndex;
}

static qboolean G_Ghost_AppendBotWaypoint( ghostBotRoute_t *route, const ghostWaypoint_t *waypoint, qboolean required ) {
    ghostWaypoint_t candidate;
    ghostWaypoint_t *lastWaypoint;
    float distance;

    if ( !route || !waypoint ) {
        return qfalse;
    }

    candidate = *waypoint;
    candidate.required = required;

    if ( route->numWaypoints > 0 ) {
        lastWaypoint = &route->waypoints[route->numWaypoints - 1];
        distance = Distance( candidate.origin, lastWaypoint->origin );
        if ( distance < 0.5f && candidate.timeOffset <= lastWaypoint->timeOffset ) {
            if ( required ) {
                lastWaypoint->required = qtrue;
            }
            return qtrue;
        }

        if ( candidate.timeOffset <= lastWaypoint->timeOffset ) {
            candidate.timeOffset = lastWaypoint->timeOffset + 1;
        }
    }

    if ( route->numWaypoints >= MAX_GHOST_BOT_WAYPOINTS ) {
        G_Ghost_CompactBotRoute( route );
    }
    if ( route->numWaypoints >= MAX_GHOST_BOT_WAYPOINTS ) {
        return qfalse;
    }

    route->waypoints[route->numWaypoints++] = candidate;
    return qtrue;
}

static qboolean G_Ghost_PointInsideBounds( const vec3_t point, const vec3_t mins, const vec3_t maxs ) {
    int axis;

    for ( axis = 0; axis < 3; ++axis ) {
        if ( point[axis] < mins[axis] || point[axis] > maxs[axis] ) {
            return qfalse;
        }
    }
    return qtrue;
}

static qboolean G_Ghost_CheckpointCrossingFraction( const ghostWaypoint_t *from, const ghostWaypoint_t *to,
    const gentity_t *checkpoint, float *fractionOut ) {
    float enterFraction = 0.0f;
    float exitFraction = 1.0f;
    int axis;

    if ( !from || !to || !checkpoint || !checkpoint->inuse || !fractionOut ||
        G_Ghost_PointInsideBounds( from->origin, checkpoint->r.absmin, checkpoint->r.absmax ) ) {
        return qfalse;
    }

    for ( axis = 0; axis < 3; ++axis ) {
        float start = from->origin[axis];
        float delta = to->origin[axis] - start;
        float first;
        float second;

        if ( fabs( delta ) < 0.001f ) {
            if ( start < checkpoint->r.absmin[axis] || start > checkpoint->r.absmax[axis] ) {
                return qfalse;
            }
            continue;
        }

        first = ( checkpoint->r.absmin[axis] - start ) / delta;
        second = ( checkpoint->r.absmax[axis] - start ) / delta;
        if ( first > second ) {
            float swap = first;
            first = second;
            second = swap;
        }
        if ( first > enterFraction ) {
            enterFraction = first;
        }
        if ( second < exitFraction ) {
            exitFraction = second;
        }
        if ( enterFraction > exitFraction ) {
            return qfalse;
        }
    }

    if ( enterFraction < 0.0f || enterFraction > 1.0f || exitFraction < 0.0f ) {
        return qfalse;
    }

    *fractionOut = enterFraction;
    return qtrue;
}

static void G_Ghost_AppendCheckpointCrossings( ghostBotRoute_t *route,
    const ghostWaypoint_t *from, const ghostWaypoint_t *to ) {
    float previousFraction = -0.001f;
    int crossingCount;

    if ( !route || !from || !to || level.numCheckpoints <= 0 ) {
        return;
    }

    for ( crossingCount = 0; crossingCount < level.numCheckpoints; ++crossingCount ) {
        int checkpointIndex;
        int nearestCheckpoint = -1;
        float nearestFraction = 2.0f;

        for ( checkpointIndex = 0; checkpointIndex < level.numCheckpoints; ++checkpointIndex ) {
            float fraction;
            gentity_t *checkpoint = level.checkpoints[checkpointIndex];

            if ( G_Ghost_CheckpointCrossingFraction( from, to, checkpoint, &fraction ) &&
                fraction > previousFraction + 0.0001f && fraction < nearestFraction ) {
                nearestFraction = fraction;
                nearestCheckpoint = checkpointIndex;
            }
        }

        if ( nearestCheckpoint < 0 ) {
            break;
        }

        {
            ghostWaypoint_t checkpointWaypoint;
            int axis;

            checkpointWaypoint.required = qtrue;
            checkpointWaypoint.timeOffset = from->timeOffset +
                (int)( ( to->timeOffset - from->timeOffset ) * nearestFraction + 0.5f );
            for ( axis = 0; axis < 3; ++axis ) {
                checkpointWaypoint.origin[axis] = from->origin[axis] +
                    nearestFraction * ( to->origin[axis] - from->origin[axis] );
            }
            G_Ghost_AppendBotWaypoint( route, &checkpointWaypoint, qtrue );
        }

        previousFraction = nearestFraction;
    }
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

        if ( G_Ghost_LineMatchesKey( line, "map" ) ) {
            const char *value = line + 3;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            Q_strncpyz( mapName, value, sizeof( mapName ) );
            continue;
        }

        if ( G_Ghost_LineMatchesKey( line, "vehicle" ) ) {
            continue;
        }

        if ( G_Ghost_LineMatchesKey( line, "best_time_ms" ) ) {
            const char *value = line + 12;
            while ( *value == ' ' || *value == '\t' ) {
                ++value;
            }
            outRoute->bestTimeMs = G_Ghost_ParseInt( value );
            continue;
        }

        if ( G_Ghost_LineMatchesKey( line, "frames" ) ) {
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
            wp.required = qfalse;

            if ( *p != ' ' && *p != '\t' && *p != '\0' ) {
                continue;
            }

            if ( hasLastWp && wp.timeOffset < lastWp.timeOffset ) {
                continue;
            }

            if ( hasLastWp ) {
                vec3_t rawDirection;
                vec3_t selectedDirection;
                float rawDistance;
                float selectedDistance;
                float turnDegrees = 0.0f;
                int timeSinceSelected;

                G_Ghost_AppendCheckpointCrossings( outRoute, &lastWp, &wp );

                VectorSubtract( wp.origin, lastWp.origin, rawDirection );
                rawDirection[2] = 0.0f;
                rawDistance = VectorNormalize( rawDirection );

                if ( rawDistance > 1.0f && outRoute->numWaypoints > 1 ) {
                    vec3_t rawAngles;
                    vec3_t selectedAngles;

                    VectorSubtract( outRoute->waypoints[outRoute->numWaypoints - 1].origin,
                        outRoute->waypoints[outRoute->numWaypoints - 2].origin, selectedDirection );
                    selectedDirection[2] = 0.0f;
                    if ( VectorNormalize( selectedDirection ) > 1.0f ) {
                        vectoangles( rawDirection, rawAngles );
                        vectoangles( selectedDirection, selectedAngles );
                        turnDegrees = fabs( AngleSubtract( rawAngles[YAW], selectedAngles[YAW] ) );
                    }
                }

                if ( outRoute->numWaypoints > 0 ) {
                    ghostWaypoint_t *lastRouteWaypoint = &outRoute->waypoints[outRoute->numWaypoints - 1];

                    selectedDistance = Distance( wp.origin, lastRouteWaypoint->origin );
                    timeSinceSelected = wp.timeOffset - lastRouteWaypoint->timeOffset;
                    if ( selectedDistance >= GHOST_WAYPOINT_MIN_DISTANCE ||
                        timeSinceSelected >= GHOST_WAYPOINT_MAX_INTERVAL ||
                        turnDegrees >= GHOST_WAYPOINT_TURN_DEGREES ) {
                        G_Ghost_AppendBotWaypoint( outRoute, &wp, qfalse );
                    }
                }
            } else {
                G_Ghost_AppendBotWaypoint( outRoute, &wp, qtrue );
            }

            lastWp = wp;
            hasLastWp = qtrue;
            frameCount++;
        }
    }

    // Keep the exact finish anchor even when it falls below the normal sampling thresholds.
    if ( hasLastWp ) {
        G_Ghost_AppendBotWaypoint( outRoute, &lastWp, qtrue );
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

    G_Ghost_BuildRouteSegments( outRoute );
    outRoute->valid = qtrue;
    G_Printf( "G_Ghost: Bot route candidate from %s (%d waypoints from %d samples, map=%s)\n",
        record->path,
        outRoute->numWaypoints,
        frameCount,
        mapName[0] ? mapName : "unknown" );

    return qtrue;
}

static void G_Ghost_BuildRouteSegments( ghostBotRoute_t *route ) {
    int i;
    int segmentCount;

    if ( !route || route->numWaypoints < 2 ) {
        return;
    }

    segmentCount = route->numWaypoints - 1;
    if ( segmentCount > ( MAX_GHOST_BOT_WAYPOINTS - 1 ) ) {
        segmentCount = MAX_GHOST_BOT_WAYPOINTS - 1;
    }

    route->numSegments = segmentCount;

    for ( i = 0; i < segmentCount; ++i ) {
        vec3_t seg;
        float dt;
        float segSpeed = 0.0f;
        float curvature = 0.0f;
        float straightness;

        VectorSubtract( route->waypoints[i + 1].origin, route->waypoints[i].origin, seg );
        dt = (float)( route->waypoints[i + 1].timeOffset - route->waypoints[i].timeOffset );
        if ( dt > 0.0f ) {
            segSpeed = 1000.0f * VectorLength( seg ) / dt;
        }

        if ( i > 0 && i + 1 < segmentCount ) {
            vec3_t prevSeg, nextSeg;
            float prevLen, nextLen;
            float segDot;

            VectorSubtract( route->waypoints[i].origin, route->waypoints[i - 1].origin, prevSeg );
            VectorSubtract( route->waypoints[i + 2].origin, route->waypoints[i + 1].origin, nextSeg );
            prevLen = VectorLength( prevSeg );
            nextLen = VectorLength( nextSeg );
            if ( prevLen > 1.0f && nextLen > 1.0f ) {
                segDot = DotProduct( prevSeg, nextSeg ) / ( prevLen * nextLen );
                if ( segDot > 1.0f ) {
                    segDot = 1.0f;
                } else if ( segDot < -1.0f ) {
                    segDot = -1.0f;
                }
                curvature = ( 1.0f - segDot );
            }
        }

        straightness = 1.0f - curvature * 0.6f;
        if ( straightness < 0.45f ) {
            straightness = 0.45f;
        } else if ( straightness > 1.05f ) {
            straightness = 1.05f;
        }
        route->segments[i].recommendedSpeed = segSpeed * straightness;
        route->segments[i].curvature = curvature;

        route->segments[i].lines[GHOST_LINE_BASE].speedScale = 1.0f;

        route->segments[i].lines[GHOST_LINE_RACE].speedScale = 1.04f - curvature * 0.16f;

        route->segments[i].lines[GHOST_LINE_DEFENSIVE].speedScale = 0.96f - curvature * 0.05f;

        route->segments[i].lines[GHOST_LINE_SAFE].speedScale = 0.88f - curvature * 0.10f;
    }
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

                /* Parse into the local record, not into s_levelGhosts[s_levelGhostCount]
                   directly: if s_levelGhostCount has reached MAX_GHOST_RECORDS_PER_MAP
                   the direct array write would be one element past the end of the array.
                   G_Ghost_AddRecordTop5ForTrackVariant handles the bounds check internally. */
                if ( G_Ghost_ParseHeader( s_ghostFileBuffer, mapname, trackLength, trackReversed, qfalse, &parsedRecord ) ) {
                    Q_strncpyz( parsedRecord.path, va( "%s/%s", ghostDir, filename ), sizeof( parsedRecord.path ) );
                    G_Ghost_AddRecordTop5ForTrackVariant( &parsedRecord );
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
                if ( G_Ghost_AddRecordTop5ForTrackVariant( &parsedRecord ) ) {
                    ++legacyCandidates;
                }
            }
        }

        if ( s_levelGhostCount > 0 ) {
            G_Printf( "G_Ghost: Legacy fallback loaded %d ghost(s) for %s\n",
                legacyCandidates,
                mapname );
        } else {
            G_Printf( "G_Ghost: No matching ghost files for map %s\n", mapname );
        }
    } else {
        G_Printf( "G_Ghost: Loaded %d ghost record(s) for %s\n", s_levelGhostCount, mapname );
    }
}

void G_Ghost_BuildBotRoutes( void ) {
    int i;
    const botPathRoute_t *mapRoute;

    if ( s_botRoutesBuilt ) {
        return;
    }
    s_botRoutesBuilt = qtrue;

    for ( i = 0; i < s_levelGhostCount; ++i ) {
        if ( s_levelGhosts[i].ambiguousLegacy ) {
            continue;
        }

        if ( !G_Ghost_LoadBotRouteFromFile( &s_levelGhosts[i], &s_botRouteScratch ) ) {
            continue;
        }

        if ( G_Ghost_IsRouteBetter( &s_botRouteScratch, &s_botRoute ) ) {
            s_botRoute = s_botRouteScratch;
        }
    }

    mapRoute = G_BotPath_GetRouteByIndex( 0 );
    if ( s_botRoute.valid ) {
        G_Printf( "G_Ghost: usable fallback loaded from %s (%d ms, track length=%d, reversed=%d)\n",
            s_botRoute.path,
            s_botRoute.bestTimeMs,
            G_Ghost_GetTrackLengthVariant(),
            G_Ghost_GetTrackReversedVariant() );
    } else {
        G_Printf( "G_Ghost: no usable fallback route found for track length=%d, reversed=%d\n",
            G_Ghost_GetTrackLengthVariant(),
            G_Ghost_GetTrackReversedVariant() );
    }

    if ( mapRoute && mapRoute->numNodes > 1 ) {
        G_Printf( "G_RouteSelect: active=map:%s usable=yes; ghostFallback=%s\n",
            mapRoute->name,
            s_botRoute.valid ? s_botRoute.path : "unavailable" );
    } else if ( s_botRoute.valid ) {
        G_Printf( "G_RouteSelect: active=ghost:%s usable=yes; mapRoute=unavailable\n", s_botRoute.path );
    } else {
        G_Printf( "G_RouteSelect: active=none usable=no; mapRoute=unavailable ghostRoute=unavailable\n" );
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
    if ( !outRoute || !s_botRoute.valid ) {
        return qfalse;
    }

    *outRoute = &s_botRoute;
    return qtrue;
}

static void G_Ghost_BuildClientTransferSamples( void ) {
    int i;
    int sampleCount = 0;

    if ( s_clientGhostSamplesBuilt ) {
        return;
    }
    s_clientGhostSamplesBuilt = qtrue;
    s_clientGhostSampleCount = 0;

    if ( !s_botRoute.valid || s_botRoute.numWaypoints < 2 ) {
        return;
    }

    if ( s_botRoute.numWaypoints <= MAX_GHOST_CLIENT_SAMPLES ) {
        for ( i = 0; i < s_botRoute.numWaypoints; ++i ) {
            s_clientGhostSampleIndices[sampleCount++] = i;
        }
    } else {
        int target;

        Com_Memset( s_clientGhostSampleSelected, 0, sizeof( s_clientGhostSampleSelected ) );
        s_clientGhostSampleSelected[0] = 1;
        s_clientGhostSampleSelected[s_botRoute.numWaypoints - 1] = 1;
        sampleCount = 2;

        /* Keep checkpoint crossings and the route endpoints whenever possible. */
        for ( i = 1; i < s_botRoute.numWaypoints - 1 && sampleCount < MAX_GHOST_CLIENT_SAMPLES; ++i ) {
            if ( s_botRoute.waypoints[i].required ) {
                s_clientGhostSampleSelected[i] = 1;
                sampleCount++;
            }
        }

        /* Fill remaining slots with evenly spaced points for smooth playback. */
        for ( target = 0; target < MAX_GHOST_CLIENT_SAMPLES && sampleCount < MAX_GHOST_CLIENT_SAMPLES; ++target ) {
            int index = target * ( s_botRoute.numWaypoints - 1 ) / ( MAX_GHOST_CLIENT_SAMPLES - 1 );
            if ( !s_clientGhostSampleSelected[index] ) {
                s_clientGhostSampleSelected[index] = 1;
                sampleCount++;
            }
        }
        for ( i = 0; i < s_botRoute.numWaypoints && sampleCount < MAX_GHOST_CLIENT_SAMPLES; ++i ) {
            if ( !s_clientGhostSampleSelected[i] ) {
                s_clientGhostSampleSelected[i] = 1;
                sampleCount++;
            }
        }
        sampleCount = 0;
        for ( i = 0; i < s_botRoute.numWaypoints; ++i ) {
            if ( s_clientGhostSampleSelected[i] ) {
                s_clientGhostSampleIndices[sampleCount++] = i;
            }
        }
    }

    s_clientGhostSampleCount = sampleCount;
}

void G_Ghost_ProcessClientTransfers( void ) {
    int clientNum;

    for ( clientNum = 0; clientNum < level.maxclients; ++clientNum ) {
        gclient_t *client = &level.clients[clientNum];

        if ( client->pers.connected != CON_CONNECTED ) {
            s_clientGhostTransferPending[clientNum] = qfalse;
            s_clientGhostTransferStarted[clientNum] = qfalse;
            s_clientGhostTransferSent[clientNum] = qfalse;
            s_clientGhostTransferNext[clientNum] = 0;
            continue;
        }
        if ( !s_clientGhostTransferPending[clientNum] ) {
            continue;
        }
        if ( !s_botRoutesBuilt ) {
            if ( level.time - s_clientGhostTransferQueuedAt[clientNum] > 5000 ) {
                trap_SendServerCommand( clientNum, "ghostmeta none 0" );
                s_clientGhostTransferPending[clientNum] = qfalse;
                s_clientGhostTransferSent[clientNum] = qtrue;
            }
            continue;
        }
        if ( !s_clientGhostTransferStarted[clientNum] ) {
            G_Ghost_BuildClientTransferSamples();
            if ( !s_botRoute.valid || s_clientGhostSampleCount < 2 ) {
                trap_SendServerCommand( clientNum, "ghostmeta none 0" );
                s_clientGhostTransferPending[clientNum] = qfalse;
                s_clientGhostTransferSent[clientNum] = qtrue;
                continue;
            }

            trap_SendServerCommand( clientNum, va( "ghostmeta base %d %d", s_botRoute.bestTimeMs, s_clientGhostSampleCount ) );
            s_clientGhostTransferStarted[clientNum] = qtrue;
            continue;
        }

        if ( s_clientGhostTransferNext[clientNum] < s_clientGhostSampleCount ) {
            char command[MAX_STRING_CHARS];
            int first = s_clientGhostTransferNext[clientNum];
            int count = s_clientGhostSampleCount - first;
            int offset;
            int i;

            if ( count > GHOST_CLIENT_SAMPLES_PER_COMMAND ) {
                count = GHOST_CLIENT_SAMPLES_PER_COMMAND;
            }
            offset = Com_sprintf( command, sizeof( command ), "ghostdata %d %d", first, count );
            for ( i = 0; i < count && offset > 0 && offset < (int)sizeof( command ); ++i ) {
                const ghostWaypoint_t *waypoint = &s_botRoute.waypoints[s_clientGhostSampleIndices[first + i]];
                int written = Com_sprintf( command + offset, sizeof( command ) - offset,
                    " %d %.1f %.1f %.1f", waypoint->timeOffset,
                    waypoint->origin[0], waypoint->origin[1], waypoint->origin[2] );
                if ( written <= 0 || written >= (int)( sizeof( command ) - offset ) ) {
                    break;
                }
                offset += written;
            }
            if ( i != count ) {
                s_clientGhostTransferPending[clientNum] = qfalse;
                trap_SendServerCommand( clientNum, "ghostmeta none 0" );
                s_clientGhostTransferSent[clientNum] = qtrue;
                continue;
            }
            trap_SendServerCommand( clientNum, command );
            s_clientGhostTransferNext[clientNum] += count;
            continue;
        }

        trap_SendServerCommand( clientNum, "ghostdone" );
        s_clientGhostTransferPending[clientNum] = qfalse;
        s_clientGhostTransferSent[clientNum] = qtrue;
    }
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
    int clientNum;

    if ( !ent || !ent->client || ent->client->pers.connected != CON_CONNECTED ) {
        return;
    }

    clientNum = ent - g_entities;
    if ( clientNum < 0 || clientNum >= MAX_CLIENTS || s_clientGhostTransferSent[clientNum] || s_clientGhostTransferPending[clientNum] ) {
        return;
    }

    s_clientGhostTransferPending[clientNum] = qtrue;
    s_clientGhostTransferStarted[clientNum] = qfalse;
    s_clientGhostTransferNext[clientNum] = 0;
    s_clientGhostTransferQueuedAt[clientNum] = level.time;
}
