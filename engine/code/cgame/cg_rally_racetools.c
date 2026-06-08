/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2021 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "cg_local.h"

#define MAX_GHOST_FILE_SIZE ( 2 * 1024 * 1024 )

static qboolean CG_LoadGhostFile( const char *path, const char *expectedMap, int expectedTrackLength, int expectedTrackReversed, const char *expectedVehicle, int declaredBestTime,
                ghostRecording_t *target, int *bestTimeOut, char *vehicleOut, int vehicleOutSize, char *pathOut, int pathOutSize );
static qboolean CG_WriteGhostFile( const char *path, const char *mapname, int trackLength, int trackReversed, const char *vehicle, int bestLapTime, const ghostRecording_t *recording );
static void CG_CleanupPersonalGhostsForVariant( const char *mapname, int trackLength, int trackReversed );
static qboolean CG_FindGhostRecyclePathForVariant( const char *mapname, int trackLength, int trackReversed, char *pathOut, int pathOutSize );

typedef struct ghostRetentionEntry_s {
        char path[MAX_QPATH];
        int bestTimeMs;
} ghostRetentionEntry_t;

static char s_ghostRetentionFileList[4096];
static ghostRetentionEntry_t s_ghostRetentionEntries[128];
static ghostRecording_t s_ghostRetentionScratchRecording;

static void QDECL CG_GhostDebugPrint( const char *fmt, ... ) {
	va_list argptr;
	char text[1024];

	if ( !cg_ghostDebug.integer ) {
		return;
	}

	va_start( argptr, fmt );
	Q_vsnprintf( text, sizeof( text ), fmt, argptr );
	va_end( argptr );

	if (cg_developer.integer) CG_Printf( "CG_GhostDebug: %s\n", text );
}

static void CG_ChopNewline( char *value ) {
	char *cursor;

	if ( !value ) {
		return;
	}

	for ( cursor = value; *cursor; cursor++ ) {
		if ( *cursor == '\n' || *cursor == '\r' ) {
			*cursor = '\0';
			break;
		}
	}
}

static qboolean CG_GhostLineMatchesKey( const char *line, const char *key ) {
        int keyLen;
        char delimiter;

        if ( !line || !key || !key[0] ) {
                return qfalse;
        }

        keyLen = strlen( key );
        if ( Q_stricmpn( line, key, keyLen ) ) {
                return qfalse;
        }

        delimiter = line[keyLen];
        return delimiter == '\0' || delimiter == ' ' || delimiter == '\t' || delimiter == '\n' || delimiter == '\r';
}


static void CG_GetGhostTrackVariant( int *trackLengthOut, int *trackReversedOut ) {
        const char *serverInfo = CG_ConfigString( CS_SERVERINFO );
        int trackLength = atoi( Info_ValueForKey( serverInfo, "g_trackLength" ) );
        int trackReversed = atoi( Info_ValueForKey( serverInfo, "g_trackReversed" ) );

        if ( trackLength < 0 || trackLength > 2 ) {
                trackLength = 0;
        }

        if ( trackReversed ) {
                trackReversed = 1;
        }

        if ( trackLengthOut ) {
                *trackLengthOut = trackLength;
        }

        if ( trackReversedOut ) {
                *trackReversedOut = trackReversed;
        }
}

static qboolean CG_GhostFilenameMatchesVariant( const char *filenameNoExt, const char *mapname, int trackLength, int trackReversed ) {
        int mapLen;
        char variantPrefix[32];

        if ( !filenameNoExt || !filenameNoExt[0] || !mapname || !mapname[0] ) {
                return qfalse;
        }

        mapLen = strlen( mapname );

        if ( Q_stricmpn( filenameNoExt, mapname, mapLen ) ) {
                return qfalse;
        }

        Com_sprintf( variantPrefix, sizeof( variantPrefix ), "_tl%d_rev%d_", trackLength, trackReversed );
        return !Q_stricmpn( filenameNoExt + mapLen, variantPrefix, strlen( variantPrefix ) );
}

static int CG_GhostRetentionCompare( const ghostRetentionEntry_t *a, const ghostRetentionEntry_t *b ) {
        if ( a->bestTimeMs <= 0 && b->bestTimeMs <= 0 ) {
                return Q_stricmp( a->path, b->path );
        }
        if ( a->bestTimeMs <= 0 ) {
                return 1;
        }
        if ( b->bestTimeMs <= 0 ) {
                return -1;
        }
        if ( a->bestTimeMs < b->bestTimeMs ) {
                return -1;
        }
        if ( a->bestTimeMs > b->bestTimeMs ) {
                return 1;
        }
        return Q_stricmp( a->path, b->path );
}

static int CG_CollectGhostRetentionEntriesForVariant( const char *mapname, int trackLength, int trackReversed ) {
        int fileCount;
        int offset;
        int i;
        int entryCount = 0;

        if ( !mapname || !mapname[0] ) {
                return 0;
        }

        fileCount = trap_FS_GetFileList( "ghosts", ".ghost", s_ghostRetentionFileList, sizeof( s_ghostRetentionFileList ) );
        offset = 0;

        for ( i = 0; i < fileCount; ++i ) {
                const char *filename = s_ghostRetentionFileList + offset;
                char cleanName[MAX_QPATH];
                char fullPath[MAX_QPATH];
                int bestTimeMs = 0;

                offset += strlen( filename ) + 1;
                if ( !filename[0] ) {
                        continue;
                }

                Q_strncpyz( cleanName, filename, sizeof( cleanName ) );
                COM_StripExtension( cleanName, cleanName, sizeof( cleanName ) );

                if ( !CG_GhostFilenameMatchesVariant( cleanName, mapname, trackLength, trackReversed ) ) {
                        continue;
                }

                if ( entryCount >= (int)( sizeof( s_ghostRetentionEntries ) / sizeof( s_ghostRetentionEntries[0] ) ) ) {
                        break;
                }

                Com_sprintf( fullPath, sizeof( fullPath ), "ghosts/%s", filename );
                if ( !CG_LoadGhostFile( fullPath, mapname, trackLength, trackReversed, NULL, 0, &s_ghostRetentionScratchRecording, &bestTimeMs, NULL, 0, NULL, 0 ) ) {
                        continue;
                }

                Q_strncpyz( s_ghostRetentionEntries[entryCount].path, fullPath, sizeof( s_ghostRetentionEntries[entryCount].path ) );
                s_ghostRetentionEntries[entryCount].bestTimeMs = bestTimeMs;
                entryCount++;
        }

        return entryCount;
}

static void CG_SortGhostRetentionEntries( int entryCount ) {
        int i;
        for ( i = 1; i < entryCount; ++i ) {
                int j = i;
                ghostRetentionEntry_t key = s_ghostRetentionEntries[i];
                while ( j > 0 && CG_GhostRetentionCompare( &key, &s_ghostRetentionEntries[j - 1] ) < 0 ) {
                        s_ghostRetentionEntries[j] = s_ghostRetentionEntries[j - 1];
                        --j;
                }
                s_ghostRetentionEntries[j] = key;
        }
}

static void CG_DeactivateGhostFile( const char *path, int bestTimeMs ) {
        fileHandle_t file;
        char line[128];

        if ( !path || !path[0] ) {
                return;
        }

        if ( trap_FS_FOpenFile( path, &file, FS_WRITE ) < 0 ) {
                CG_Printf( "CG_Ghost: failed to deactivate %s\n", path );
                return;
        }

        trap_FS_Write( "map __disabled__\n", 17, file );
        Com_sprintf( line, sizeof( line ), "best_time_ms %d\n", bestTimeMs > 0 ? bestTimeMs : 999999999 );
        trap_FS_Write( line, strlen( line ), file );
        trap_FS_Write( "frames 0\n", 9, file );
        trap_FS_FCloseFile( file );
}

static void CG_CleanupPersonalGhostsForVariant( const char *mapname, int trackLength, int trackReversed ) {
        int entryCount;
        int i;

        if ( !mapname || !mapname[0] ) {
                return;
        }

        entryCount = CG_CollectGhostRetentionEntriesForVariant( mapname, trackLength, trackReversed );
        CG_SortGhostRetentionEntries( entryCount );

        for ( i = 5; i < entryCount; ++i ) {
                CG_DeactivateGhostFile( s_ghostRetentionEntries[i].path, s_ghostRetentionEntries[i].bestTimeMs );
        }
}

static qboolean CG_FindGhostRecyclePathForVariant( const char *mapname, int trackLength, int trackReversed, char *pathOut, int pathOutSize ) {
        int entryCount;

        if ( !pathOut || pathOutSize <= 0 ) {
                return qfalse;
        }
        pathOut[0] = '\0';

        if ( !mapname || !mapname[0] ) {
                return qfalse;
        }

        entryCount = CG_CollectGhostRetentionEntriesForVariant( mapname, trackLength, trackReversed );

        if ( entryCount < 5 ) {
                return qfalse;
        }

        CG_SortGhostRetentionEntries( entryCount );

        Q_strncpyz( pathOut, s_ghostRetentionEntries[entryCount - 1].path, pathOutSize );
        return qtrue;
}

static void CG_ResetPersonalGhost( void ) {
        memset( &cg.ghostPlayback, 0, sizeof( cg.ghostPlayback ) );
        cg.personalGhostAvailable = qfalse;
        cg.personalGhostBestTime = 0;
        cg.personalGhostVehicle[0] = '\0';
        cg.personalGhostPath[0] = '\0';
}

static qboolean CG_SelectGhostFrames( ghostRecording_t *recording, int targetOffset, ghostFrame_t **previous, ghostFrame_t **next, float *lerp ) {
        int i;

        if ( !recording || !recording->valid || recording->frameCount <= 0 ) {
                return qfalse;
        }

        *previous = &recording->frames[recording->startIndex];
        *next = *previous;
        *lerp = 0.0f;

        if ( targetOffset <= (*previous)->timeOffset ) {
                return qtrue;
        }

        for ( i = 1; i < recording->frameCount; i++ ) {
                int index = ( recording->startIndex + i ) % MAX_GHOST_FRAMES;
                ghostFrame_t *candidate = &recording->frames[index];

                if ( targetOffset <= candidate->timeOffset ) {
                        *next = candidate;
                        if ( candidate->timeOffset != (*previous)->timeOffset ) {
                                *lerp = (float)( targetOffset - (*previous)->timeOffset ) /
                                        (float)( candidate->timeOffset - (*previous)->timeOffset );
                        }
                        return qtrue;
                }

                *previous = candidate;
        }

        return qtrue;
}

static ghostRecording_t *CG_GetActiveGhostRecording( void ) {
        switch ( cg_ghostPlayback.integer ) {
        case 1:
                return cg.ghostPlayback.valid ? &cg.ghostPlayback : NULL;
        case 2:
                return cg.baseGhost.valid ? &cg.baseGhost : NULL;
        default:
                return NULL;
        }
}

void CG_ResetBaseGhost( void ) {
        memset( &cg.baseGhost, 0, sizeof( cg.baseGhost ) );
        cg.baseGhostAvailable = qfalse;
        cg.baseGhostBestTime = 0;
        cg.baseGhostVehicle[0] = '\0';
        cg.baseGhostPath[0] = '\0';
}

void CG_LoadPersonalGhost( void ) {
        static ghostRecording_t candidateRecording;
        char mapname[MAX_QPATH];
        char fileList[4096];
        int fileCount;
        int offset;
        int i;
        int trackLength = 0;
        int trackReversed = 0;
        int bestTime = 0;
        qboolean foundRecording = qfalse;

        if ( !cg.snap || cg.snap->ps.clientNum >= MAX_CLIENTS ) {
                return;
        }

        if ( !cgs.clientinfo[cg.snap->ps.clientNum].infoValid ) {
                return;
        }

        CG_ResetPersonalGhost();

        COM_StripExtension( COM_SkipPath( cgs.mapname ), mapname, sizeof( mapname ) );
        if ( !mapname[0] ) {
                return;
        }


        CG_GetGhostTrackVariant( &trackLength, &trackReversed );

        fileCount = trap_FS_GetFileList( "ghosts", ".ghost", fileList, sizeof( fileList ) );
        offset = 0;

        for ( i = 0; i < fileCount; ++i ) {
                const char *filename = fileList + offset;
                char cleanName[MAX_QPATH];
                char path[MAX_QPATH];
                int candidateTime = 0;
                char candidateVehicle[MAX_QPATH] = "";
                char candidatePath[MAX_QPATH] = "";

                offset += strlen( filename ) + 1;

                if ( !filename[0] ) {
                        continue;
                }

                Q_strncpyz( cleanName, filename, sizeof( cleanName ) );
                COM_StripExtension( cleanName, cleanName, sizeof( cleanName ) );

                if ( !CG_GhostFilenameMatchesVariant( cleanName, mapname, trackLength, trackReversed ) ) {
                        continue;
                }

                Com_sprintf( path, sizeof( path ), "ghosts/%s", filename );

                if ( !CG_LoadGhostFile( path, mapname, trackLength, trackReversed, NULL, 0, &candidateRecording, &candidateTime, candidateVehicle, sizeof( candidateVehicle ), candidatePath, sizeof( candidatePath ) ) ) {
                        continue;
                }

                if ( !foundRecording || ( candidateTime > 0 && ( bestTime <= 0 || candidateTime < bestTime ) ) ) {
                        cg.ghostPlayback = candidateRecording;
                        bestTime = candidateTime;
                        Q_strncpyz( cg.personalGhostVehicle, candidateVehicle, sizeof( cg.personalGhostVehicle ) );
                        Q_strncpyz( cg.personalGhostPath, candidatePath, sizeof( cg.personalGhostPath ) );
                        foundRecording = qtrue;
                }
        }

        if ( foundRecording ) {
                cg.personalGhostAvailable = qtrue;
                cg.personalGhostBestTime = bestTime;
        }
}


static qboolean CG_LoadGhostFile( const char *path, const char *expectedMap, int expectedTrackLength, int expectedTrackReversed, const char *expectedVehicle, int declaredBestTime, ghostRecording_t *target, int *bestTimeOut, char *vehicleOut, int vehicleOutSize, char *pathOut, int pathOutSize ) {
	fileHandle_t file;
	int length;
	static char buffer[MAX_GHOST_FILE_SIZE+1];
	char *line;
	char mapName[MAX_QPATH] = "";
	char vehicle[MAX_QPATH] = "";
	int trackLength = -1;
	int trackReversed = -1;
	int bestTimeMs = 0;
//	int expectedFrames = 0;
	int frameCount = 0;
	int lastOffset = 0;
	int lineNumber = 1;

	if ( !target ) {
		return qfalse;
	}

	memset( target, 0, sizeof( *target ) );

	if ( !path || !path[0] ) {
		return qfalse;
	}

	length = trap_FS_FOpenFile( path, &file, FS_READ );
	if ( length <= 0 ) {
		CG_Printf( "CG_Ghost: could not open %s\n", path );
		return qfalse;
	}

	if ( length > MAX_GHOST_FILE_SIZE ) {
		trap_FS_FCloseFile( file );
		CG_Printf( "CG_Ghost: %s too large (%d bytes)\n", path, length );
		return qfalse;
	}

	trap_FS_Read( buffer, length, file );
	buffer[length] = '\0';
	trap_FS_FCloseFile( file );

	CG_GhostDebugPrint( "Loading ghost '%s' (expected map='%s' tl=%d rev=%d vehicle='%s')",
                path, expectedMap ? expectedMap : "", expectedTrackLength, expectedTrackReversed, expectedVehicle ? expectedVehicle : "" );

	line = buffer;

	while ( line && *line ) {
		char *cursor = line;
		char preview[256];

		while ( *cursor == ' ' || *cursor == '\t' ) {
			++cursor;
		}

		if ( cursor[0] == '\xEF' && cursor[1] == '\xBB' && cursor[2] == '\xBF' ) {
			cursor += 3;
		}

		Q_strncpyz( preview, cursor, sizeof( preview ) );
		CG_ChopNewline( preview );

		if ( cg_ghostDebug.integer >= 2 ) {
			CG_GhostDebugPrint( "%s line %d: %s", path, lineNumber, preview );
		}

		if ( cursor[0] == '\0' || cursor[0] == '#' ) {
			goto nextLine;
		}

		if ( CG_GhostLineMatchesKey( cursor, "map" ) ) {
			const char *value = cursor + 3;
			while ( *value == ' ' || *value == '\t' ) {
				++value;
			}
			Q_strncpyz( mapName, value, sizeof( mapName ) );
			CG_ChopNewline( mapName );
			CG_GhostDebugPrint( "%s line %d: map set to '%s'", path, lineNumber, mapName );
			goto nextLine;
		}

		if ( CG_GhostLineMatchesKey( cursor, "vehicle" ) ) {
			const char *value = cursor + 7;
			while ( *value == ' ' || *value == '\t' ) {
				++value;
			}
			Q_strncpyz( vehicle, value, sizeof( vehicle ) );
			CG_ChopNewline( vehicle );
			CG_GhostDebugPrint( "%s line %d: vehicle set to '%s'", path, lineNumber, vehicle );
			goto nextLine;
		}

		if ( CG_GhostLineMatchesKey( cursor, "track_length" ) ) {
			const char *value = cursor + 12;
			while ( *value == ' ' || *value == '\t' ) {
				++value;
			}
			trackLength = atoi( value );
			CG_GhostDebugPrint( "%s line %d: track_length set to %d", path, lineNumber, trackLength );
			goto nextLine;
		}

		if ( CG_GhostLineMatchesKey( cursor, "track_reversed" ) ) {
			const char *value = cursor + 14;
			while ( *value == ' ' || *value == '\t' ) {
				++value;
			}
			trackReversed = atoi( value ) ? 1 : 0;
			CG_GhostDebugPrint( "%s line %d: track_reversed set to %d", path, lineNumber, trackReversed );
			goto nextLine;
		}

		if ( CG_GhostLineMatchesKey( cursor, "best_time_ms" ) ) {
			const char *value = cursor + 12;
			while ( *value == ' ' || *value == '\t' ) {
				++value;
			}
			bestTimeMs = atoi( value );
			CG_GhostDebugPrint( "%s line %d: best_time_ms set to %d", path, lineNumber, bestTimeMs );
			goto nextLine;
		}

		if ( CG_GhostLineMatchesKey( cursor, "frames" ) ) {
			goto nextLine;
		}

                if ( ( cursor[0] >= '0' && cursor[0] <= '9' ) || cursor[0] == '-' ) {
                        ghostFrame_t *frame;
                        float ox = 0.0f, oy = 0.0f, oz = 0.0f;
                        float ax = 0.0f, ay = 0.0f, az = 0.0f;
                        float vx = 0.0f, vy = 0.0f, vz = 0.0f;
                        int buttons = 0, forwardmove = 0, upmove = 0;
                        int parsed = 0;
                        char *parseCursor;
                        qboolean extraTokens = qfalse;

                        if ( frameCount >= MAX_GHOST_FRAMES ) {
                                goto nextLine;
                        }

                        frame = &target->frames[frameCount];
                        parseCursor = cursor;

                        while ( parsed < 13 ) {
                                char *token = COM_ParseExt( &parseCursor, qfalse );

                                if ( !token[0] ) {
                                        break;
                                }

                                switch ( parsed ) {
                                case 0:
                                        frame->timeOffset = atoi( token );
                                        break;
                                case 1:
                                        ox = atof( token );
                                        break;
                                case 2:
                                        oy = atof( token );
                                        break;
                                case 3:
                                        oz = atof( token );
                                        break;
                                case 4:
                                        ax = atof( token );
                                        break;
                                case 5:
                                        ay = atof( token );
                                        break;
                                case 6:
                                        az = atof( token );
                                        break;
                                case 7:
                                        vx = atof( token );
                                        break;
                                case 8:
                                        vy = atof( token );
                                        break;
                                case 9:
                                        vz = atof( token );
                                        break;
                                case 10:
                                        buttons = atoi( token );
                                        break;
                                case 11:
                                        forwardmove = atoi( token );
                                        break;
                                case 12:
                                        upmove = atoi( token );
                                        break;
                                }

                                parsed++;
                        }

                        if ( parsed == 13 ) {
                                char *extra = COM_ParseExt( &parseCursor, qfalse );

                                if ( extra && extra[0] ) {
                                        extraTokens = qtrue;
                                        parsed++;
                                }
                        }

                        if ( parsed == 13 && !extraTokens ) {
                                VectorSet( frame->origin, ox, oy, oz );
                                VectorSet( frame->angles, ax, ay, az );
                                VectorSet( frame->velocity, vx, vy, vz );
                                frame->buttons = buttons;
                                frame->forwardmove = forwardmove;
                                frame->upmove = upmove;
                                lastOffset = frame->timeOffset;
                                frameCount++;

                                if ( cg_ghostDebug.integer >= 2 || ( cg_ghostDebug.integer && frameCount <= 5 ) ) {
                                        CG_GhostDebugPrint( "%s line %d: frame %d offset %d pos(%.3f %.3f %.3f) ang(%.3f %.3f %.3f) vel(%.3f %.3f %.3f)",
                                                path, lineNumber, frameCount, frame->timeOffset,
                                                frame->origin[0], frame->origin[1], frame->origin[2],
                                                frame->angles[0], frame->angles[1], frame->angles[2],
                                                frame->velocity[0], frame->velocity[1], frame->velocity[2] );
                                }
                        } else {
                                CG_GhostDebugPrint( "%s line %d: could not parse frame (fields=%d) from '%s'",
                                        path, lineNumber, parsed, preview );
                        }
                }
nextLine:
		if ( !line ) {
			break;
		}

		while ( *line && *line != '\n' && *line != '\r' ) {
			line++;
		}

		while ( *line == '\n' || *line == '\r' ) {
			*line = '\0';
			line++;
		}

		lineNumber++;
	}

	CG_GhostDebugPrint( "%s summary: map='%s' tl=%d rev=%d vehicle='%s' frames=%d duration=%d bestTimeMs=%d", path, mapName, trackLength, trackReversed, vehicle, frameCount, lastOffset, bestTimeMs );

	if ( expectedMap && expectedMap[0] && ( !mapName[0] || Q_stricmp( expectedMap, mapName ) ) ) {
			CG_Printf( "CG_Ghost: %s map '%s' does not match '%s'\n", path, mapName, expectedMap );
			return qfalse;
	}

	if ( expectedTrackLength >= 0 && ( trackLength < 0 || trackLength != expectedTrackLength ) ) {
		CG_Printf( "CG_Ghost: %s track_length '%d' does not match '%d'\n", path, trackLength, expectedTrackLength );
		return qfalse;
	}

	if ( expectedTrackReversed >= 0 && ( trackReversed < 0 || trackReversed != expectedTrackReversed ) ) {
		CG_Printf( "CG_Ghost: %s track_reversed '%d' does not match '%d'\n", path, trackReversed, expectedTrackReversed );
		return qfalse;
	}

	if ( frameCount <= 1 ) {
		CG_Printf( "CG_Ghost: %s has no usable frames\n", path );
		return qfalse;
	}

	target->frameCount = frameCount;
	target->startIndex = 0;
	target->writeIndex = frameCount % MAX_GHOST_FRAMES;
	target->duration = lastOffset;
	target->valid = qtrue;

	if ( bestTimeOut ) {
		*bestTimeOut = declaredBestTime > 0 ? declaredBestTime : bestTimeMs;
	}

	if ( vehicleOut && vehicleOutSize > 0 ) {
		Q_strncpyz( vehicleOut, vehicle[0] ? vehicle : expectedVehicle, vehicleOutSize );
	}

	if ( pathOut && pathOutSize > 0 ) {
		Q_strncpyz( pathOut, path, pathOutSize );
	}

	return qtrue;
}
qboolean CG_LoadGhostFromFile( const char *path, const char *expectedMap, const char *expectedVehicle, int declaredBestTime ) {
        int trackLength = 0;
        int trackReversed = 0;
        (void)expectedVehicle;

        CG_ResetBaseGhost();

        CG_GetGhostTrackVariant( &trackLength, &trackReversed );

        cg.baseGhostAvailable = CG_LoadGhostFile( path, expectedMap, trackLength, trackReversed, NULL, declaredBestTime, &cg.baseGhost, &cg.baseGhostBestTime, cg.baseGhostVehicle, sizeof( cg.baseGhostVehicle ), cg.baseGhostPath, sizeof( cg.baseGhostPath ) );

        return cg.baseGhostAvailable;
}

void CG_BeginGhostRecording( int startTime ) {
	memset( &cg.ghostRecording, 0, sizeof( cg.ghostRecording ) );
	cg.ghostRecording.startIndex = 0;
	cg.ghostRecording.writeIndex = 0;
	cg.ghostRecording.frameCount = 0;
	cg.ghostRecording.duration = 0;
	cg.ghostRecording.valid = qfalse;

	cg.ghostRecordingActive = qtrue;
	cg.ghostRecordingStartTime = startTime;
}

void CG_EndGhostRecording( int finishTime ) {
        if ( !cg.ghostRecordingActive ) {
                return;
        }

        cg.ghostRecordingActive = qfalse;

        if ( cg.ghostRecording.frameCount > 1 ) {
                int duration = finishTime > cg.ghostRecordingStartTime
                        ? finishTime - cg.ghostRecordingStartTime
                        : cg.ghostRecording.duration;

                cg.ghostRecording.duration = duration;
                cg.ghostRecording.valid = qtrue;
        }
}

void CG_RecordGhostFrame( void ) {
        usercmd_t cmd;
        ghostFrame_t *frame;

	if ( !cg.ghostRecordingActive ) {
		return;
	}

	if ( !isRallyRace() ) {
		return;
	}

	if ( !cg.snap || cg.snap->ps.clientNum >= MAX_CLIENTS ) {
		return;
	}

	if ( cg_entities[cg.snap->ps.clientNum].finishRaceTime ) {
		return;
	}

	if ( cg.time < cg.ghostRecordingStartTime ) {
		return;
	}

	frame = &cg.ghostRecording.frames[cg.ghostRecording.writeIndex];

	frame->timeOffset = cg.time - cg.ghostRecordingStartTime;
	VectorCopy( cg.predictedPlayerState.origin, frame->origin );
	VectorCopy( cg.predictedPlayerState.viewangles, frame->angles );
	VectorCopy( cg.predictedPlayerState.velocity, frame->velocity );

	trap_GetUserCmd( trap_GetCurrentCmdNumber(), &cmd );
	frame->buttons = cmd.buttons;
	frame->forwardmove = cmd.forwardmove;
	frame->upmove = cmd.upmove;

	cg.ghostRecording.writeIndex = ( cg.ghostRecording.writeIndex + 1 ) % MAX_GHOST_FRAMES;
	if ( cg.ghostRecording.frameCount < MAX_GHOST_FRAMES ) {
		cg.ghostRecording.frameCount++;
	} else {
		cg.ghostRecording.startIndex = cg.ghostRecording.writeIndex;
	}

        cg.ghostRecording.duration = frame->timeOffset;
        cg.ghostRecording.valid = cg.ghostRecording.frameCount > 1;
}

void CG_AttemptSavePersonalGhost( int finishTime ) {
        char mapname[MAX_QPATH];
        char vehicle[MAX_QPATH];
        char path[MAX_QPATH];
        char recyclePath[MAX_QPATH];
        char timestamp[32];
        qtime_t now;
        int bestLapTime;
        int retentionEntryCount;
        int worstQualifiedBestTime;
        int lapStartOffset;
        int lapEndOffset;
        int trackLength = 0;
        int trackReversed = 0;
        int i;
        static ghostRecording_t lapRecording;
        ghostFrame_t *previousFrame;
        qboolean qualifiesTop5 = qtrue;
        const char *top5Result = "saved";
        const char *top5Reason = "eligible";

        if ( !cg.snap || cg.snap->ps.clientNum >= MAX_CLIENTS ) {
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: invalid snapshot/client (bestLapTime=%d personalBest=%d variant=unknown/unknown/unknown top5=skipped:invalid-client)",
                        0, cg.personalGhostBestTime );
                return;
        }

        if ( !cg.ghostRecording.valid || cg.ghostRecording.frameCount <= 1 ) {
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: recording invalid (bestLapTime=%d personalBest=%d variant=unknown/unknown/unknown top5=skipped:invalid-recording)",
                        0, cg.personalGhostBestTime );
                return;
        }

        if ( !cg_entities[cg.snap->ps.clientNum].startRaceTime || finishTime <= cg_entities[cg.snap->ps.clientNum].startRaceTime ) {
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: missing/invalid race time (bestLapTime=%d personalBest=%d variant=unknown/unknown/unknown top5=skipped:invalid-race-time)",
                        0, cg.personalGhostBestTime );
                return;
        }

        if ( !cgs.clientinfo[cg.snap->ps.clientNum].infoValid ) {
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: client info invalid (bestLapTime=%d personalBest=%d variant=unknown/unknown/unknown top5=skipped:invalid-clientinfo)",
                        0, cg.personalGhostBestTime );
                return;
        }

        bestLapTime = cg_entities[cg.snap->ps.clientNum].bestLapTime;
        if ( bestLapTime <= 0 ) {
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: best lap <= 0 (bestLapTime=%d personalBest=%d variant=unknown/unknown/unknown top5=skipped:invalid-bestlap)",
                        bestLapTime, cg.personalGhostBestTime );
                return;
        }

        lapStartOffset = cg_entities[cg.snap->ps.clientNum].bestLapStartTime - cg_entities[cg.snap->ps.clientNum].startRaceTime;
        lapEndOffset = lapStartOffset + bestLapTime;

        if ( lapStartOffset < 0 || lapEndOffset > cg.ghostRecording.duration ) {
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: lap offsets out of range (bestLapTime=%d personalBest=%d variant=unknown/unknown/unknown top5=skipped:invalid-lap-window)",
                        bestLapTime, cg.personalGhostBestTime );
                return;
        }

        COM_StripExtension( COM_SkipPath( cgs.mapname ), mapname, sizeof( mapname ) );
        Q_strncpyz( vehicle, cgs.clientinfo[cg.snap->ps.clientNum].modelName, sizeof( vehicle ) );
        CG_GetGhostTrackVariant( &trackLength, &trackReversed );

        if ( !mapname[0] || !vehicle[0] ) {
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: map/vehicle missing (bestLapTime=%d personalBest=%d variant=%s/tl%d/rev%d top5=skipped:missing-map-or-vehicle)",
                        bestLapTime, cg.personalGhostBestTime, mapname[0] ? mapname : "<empty>", trackLength, trackReversed );
                return;
        }

        retentionEntryCount = CG_CollectGhostRetentionEntriesForVariant( mapname, trackLength, trackReversed );
        if ( retentionEntryCount >= 5 ) {
                worstQualifiedBestTime = 0;

                CG_SortGhostRetentionEntries( retentionEntryCount );

                for ( i = retentionEntryCount - 1; i >= 0; --i ) {
                        if ( s_ghostRetentionEntries[i].bestTimeMs > 0 ) {
                                worstQualifiedBestTime = s_ghostRetentionEntries[i].bestTimeMs;
                                break;
                        }
                }

                if ( worstQualifiedBestTime > 0 && bestLapTime >= worstQualifiedBestTime ) {
                        qualifiesTop5 = qfalse;
                        top5Result = "skipped";
                        top5Reason = "top5-gate";
                        CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: top-5 qualification failed (bestLapTime=%d personalBest=%d variant=%s/tl%d/rev%d top5=%s:%s worstQualified=%d retentionEntries=%d)",
                                bestLapTime, cg.personalGhostBestTime, mapname, trackLength, trackReversed, top5Result, top5Reason,
                                worstQualifiedBestTime, retentionEntryCount );
                        return;
                }
        }

        memset( &lapRecording, 0, sizeof( lapRecording ) );
        previousFrame = NULL;

        for ( i = 0; i < cg.ghostRecording.frameCount; i++ ) {
                int index = ( cg.ghostRecording.startIndex + i ) % MAX_GHOST_FRAMES;
                ghostFrame_t *frame = &cg.ghostRecording.frames[index];
                qboolean addedStartFromCurrent = qfalse;
                int j;

                if ( frame->timeOffset < lapStartOffset ) {
                        previousFrame = frame;
                        continue;
                }

                if ( lapRecording.frameCount == 0 ) {
                        if ( previousFrame && previousFrame->timeOffset < lapStartOffset && frame->timeOffset > lapStartOffset ) {
                                float lerp = (float)( lapStartOffset - previousFrame->timeOffset ) /
                                        (float)( frame->timeOffset - previousFrame->timeOffset );
                                ghostFrame_t *target = &lapRecording.frames[lapRecording.frameCount++];

                                target->timeOffset = 0;
                                for ( j = 0; j < 3; j++ ) {
                                        target->origin[j] = previousFrame->origin[j] + lerp * ( frame->origin[j] - previousFrame->origin[j] );
                                        target->angles[j] = previousFrame->angles[j] + lerp * AngleSubtract( frame->angles[j], previousFrame->angles[j] );
                                        target->velocity[j] = previousFrame->velocity[j] + lerp * ( frame->velocity[j] - previousFrame->velocity[j] );
                                }
                                target->buttons = previousFrame->buttons;
                                target->forwardmove = previousFrame->forwardmove;
                                target->upmove = previousFrame->upmove;
                        } else {
                                ghostFrame_t *target = &lapRecording.frames[lapRecording.frameCount++];
                                *target = *frame;
                                target->timeOffset = frame->timeOffset - lapStartOffset;
                                addedStartFromCurrent = qtrue;
                        }
                }

                if ( lapRecording.frameCount >= MAX_GHOST_FRAMES ) {
                        break;
                }

                if ( lapRecording.frameCount > 0 ) {
                        if ( addedStartFromCurrent && frame->timeOffset == lapStartOffset ) {
                                previousFrame = frame;
                                continue;
                        }

                        if ( frame->timeOffset > lapEndOffset ) {
                                if ( previousFrame && lapRecording.frameCount < MAX_GHOST_FRAMES ) {
                                        ghostFrame_t *target = &lapRecording.frames[lapRecording.frameCount++];
                                        float lerp = 0.0f;

                                        if ( frame->timeOffset != previousFrame->timeOffset ) {
                                                lerp = (float)( lapEndOffset - previousFrame->timeOffset ) /
                                                        (float)( frame->timeOffset - previousFrame->timeOffset );
                                        }

                                        target->timeOffset = bestLapTime;
                                        for ( j = 0; j < 3; j++ ) {
                                                target->origin[j] = previousFrame->origin[j] + lerp * ( frame->origin[j] - previousFrame->origin[j] );
                                                target->angles[j] = previousFrame->angles[j] + lerp * AngleSubtract( frame->angles[j], previousFrame->angles[j] );
                                                target->velocity[j] = previousFrame->velocity[j] + lerp * ( frame->velocity[j] - previousFrame->velocity[j] );
                                        }
                                        target->buttons = lerp >= 0.5f ? frame->buttons : previousFrame->buttons;
                                        target->forwardmove = lerp >= 0.5f ? frame->forwardmove : previousFrame->forwardmove;
                                        target->upmove = lerp >= 0.5f ? frame->upmove : previousFrame->upmove;
                                }
                                break;
                        }

                        {
                                ghostFrame_t *target = &lapRecording.frames[lapRecording.frameCount++];
                                *target = *frame;
                                target->timeOffset = frame->timeOffset - lapStartOffset;
                        }
                }

                previousFrame = frame;
        }

        if ( lapRecording.frameCount > 0 && lapRecording.frames[lapRecording.frameCount - 1].timeOffset < bestLapTime && lapRecording.frameCount < MAX_GHOST_FRAMES ) {
                ghostFrame_t *source = previousFrame ? previousFrame : &cg.ghostRecording.frames[cg.ghostRecording.startIndex];
                ghostFrame_t *target = &lapRecording.frames[lapRecording.frameCount++];

                *target = *source;
                target->timeOffset = bestLapTime;
        }

        if ( lapRecording.frameCount <= 1 ) {
                top5Result = qualifiesTop5 ? "skipped" : top5Result;
                top5Reason = qualifiesTop5 ? "lap-frame-validation" : top5Reason;
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: lap recording frame validation failed (bestLapTime=%d personalBest=%d variant=%s/tl%d/rev%d top5=%s:%s)",
                        bestLapTime, cg.personalGhostBestTime, mapname, trackLength, trackReversed, top5Result, top5Reason );
                return;
        }

        lapRecording.startIndex = 0;
        lapRecording.writeIndex = lapRecording.frameCount % MAX_GHOST_FRAMES;
        lapRecording.duration = lapRecording.frames[lapRecording.frameCount - 1].timeOffset;
        lapRecording.valid = qtrue;

        trap_RealTime( &now );
        Com_sprintf( timestamp, sizeof( timestamp ), "%04d%02d%02d-%02d%02d%02d",
                1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday,
                now.tm_hour, now.tm_min, now.tm_sec );

        if ( CG_FindGhostRecyclePathForVariant( mapname, trackLength, trackReversed, recyclePath, sizeof( recyclePath ) ) ) {
                Q_strncpyz( path, recyclePath, sizeof( path ) );
        } else {
                Com_sprintf( path, sizeof( path ), "ghosts/%s_tl%d_rev%d_%s.ghost", mapname, trackLength, trackReversed, timestamp );
        }

        if ( !CG_WriteGhostFile( path, mapname, trackLength, trackReversed, vehicle, bestLapTime, &lapRecording ) ) {
                top5Result = qualifiesTop5 ? "skipped" : top5Result;
                top5Reason = qualifiesTop5 ? "file-write-failed" : top5Reason;
                CG_GhostDebugPrint( "AttemptSavePersonalGhost skipped: file write failed (bestLapTime=%d personalBest=%d variant=%s/tl%d/rev%d top5=%s:%s path=%s)",
                        bestLapTime, cg.personalGhostBestTime, mapname, trackLength, trackReversed, top5Result, top5Reason, path );
                return;
        }

        top5Result = "saved";
        top5Reason = qualifiesTop5 ? "qualified" : top5Reason;
        CG_GhostDebugPrint( "AttemptSavePersonalGhost saved (bestLapTime=%d personalBest=%d variant=%s/tl%d/rev%d top5=%s:%s path=%s)",
                bestLapTime, cg.personalGhostBestTime, mapname, trackLength, trackReversed, top5Result, top5Reason, path );

        CG_CleanupPersonalGhostsForVariant( mapname, trackLength, trackReversed );

        cg.ghostPlayback = lapRecording;
        cg.personalGhostAvailable = qtrue;
        cg.personalGhostBestTime = bestLapTime;
        Q_strncpyz( cg.personalGhostVehicle, vehicle, sizeof( cg.personalGhostVehicle ) );
        Q_strncpyz( cg.personalGhostPath, path, sizeof( cg.personalGhostPath ) );
}

static qboolean CG_WriteGhostFile( const char *path, const char *mapname, int trackLength, int trackReversed, const char *vehicle, int bestLapTime, const ghostRecording_t *recording ) {
        fileHandle_t file;
        int i;

        if ( trap_FS_FOpenFile( path, &file, FS_WRITE ) < 0 ) {
                CG_Printf( "CG_Ghost: failed to save %s\n", path );
                return qfalse;
        }

        {
                char header[64];

                Com_sprintf( header, sizeof( header ), "map %s\n", mapname );
                trap_FS_Write( header, strlen( header ), file );

                Com_sprintf( header, sizeof( header ), "vehicle %s\n", vehicle );
                trap_FS_Write( header, strlen( header ), file );

                Com_sprintf( header, sizeof( header ), "track_length %d\n", trackLength );
                trap_FS_Write( header, strlen( header ), file );

                Com_sprintf( header, sizeof( header ), "track_reversed %d\n", trackReversed );
                trap_FS_Write( header, strlen( header ), file );

                Com_sprintf( header, sizeof( header ), "best_time_ms %d\n", bestLapTime );
                trap_FS_Write( header, strlen( header ), file );

                Com_sprintf( header, sizeof( header ), "frames %d\n", recording->frameCount );
                trap_FS_Write( header, strlen( header ), file );
        }

        for ( i = 0; i < recording->frameCount; i++ ) {
                int index = ( recording->startIndex + i ) % MAX_GHOST_FRAMES;
                const ghostFrame_t *frame = &recording->frames[index];
                char line[256];

                Com_sprintf( line, sizeof( line ), "%d %f %f %f %f %f %f %f %f %f %d %d %d\n",
                        frame->timeOffset,
                        frame->origin[0], frame->origin[1], frame->origin[2],
                        frame->angles[0], frame->angles[1], frame->angles[2],
                        frame->velocity[0], frame->velocity[1], frame->velocity[2],
                        frame->buttons, frame->forwardmove, frame->upmove );
                trap_FS_Write( line, strlen( line ), file );
        }

        trap_FS_FCloseFile( file );

        return qtrue;
}

static byte CG_GetGhostAlpha( void ) {
	trap_Cvar_Update( &cg_ghostAlpha );

	return (byte)Com_Clamp( 0, 255, cg_ghostAlpha.integer );
}

static void CG_AddGhostWheels( clientInfo_t *ci, refEntity_t *body, int ghostAlpha ) {
        int i;
        char tags[4][12] = { "tag_wheelfl", "tag_wheelfr", "tag_wheelrl", "tag_wheelrr" };

        if ( !ci || !body || !body->hModel || !ci->wheelModel ) {
                return;
        }

        for ( i = 0; i < 4; i++ ) {
                refEntity_t wheel;
                vec3_t wheelAngles;

                if ( !CG_TagExists( body->hModel, tags[i] ) ) {
                        continue;
                }

                memset( &wheel, 0, sizeof( wheel ) );
                VectorClear( wheelAngles );

		wheel.hModel = ci->wheelModel;
		wheel.customSkin = CG_TagExists( wheel.hModel, "tag_polygonwheel" ) ? 0 : ci->wheelSkin;
		wheel.customShader = cgs.media.ghostShader;
		wheel.shadowPlane = body->shadowPlane;
		wheel.renderfx = body->renderfx;
		VectorCopy( body->lightingOrigin, wheel.lightingOrigin );
                wheel.shaderRGBA[0] = 255;
                wheel.shaderRGBA[1] = 255;
                wheel.shaderRGBA[2] = 255;
                wheel.shaderRGBA[3] = ghostAlpha;

                AnglesToAxis( wheelAngles, wheel.axis );
                CG_PositionRotatedEntityOnTag( &wheel, body, body->hModel, tags[i] );

                trap_R_AddRefEntityToScene( &wheel );
        }
}

void CG_AddGhostEntity( void ) {
        ghostFrame_t *from, *to;
        ghostRecording_t *recording;
        float lerp;
        int offset;
        refEntity_t ghost;
        clientInfo_t *ci;
        vec3_t origin;
        vec3_t angles;
	int i;
	byte ghostAlpha;

        if ( cg_ghostPlayback.integer <= 0 ) {
                return;
        }

        if ( !isRallyRace() ) {
                return;
        }

        if ( cg_ghostPlayback.integer == 1 && !cg.personalGhostAvailable ) {
                CG_LoadPersonalGhost();
        }

        recording = CG_GetActiveGhostRecording();
        if ( !recording || recording->frameCount <= 0 ) {
                return;
        }

        if ( !cg.snap || cg.snap->ps.clientNum >= MAX_CLIENTS ) {
                return;
	}

	if ( !cg_entities[cg.snap->ps.clientNum].startLapTime || cg_entities[cg.snap->ps.clientNum].finishRaceTime ) {
		return;
	}

	offset = cg.time - cg_entities[cg.snap->ps.clientNum].startLapTime;
        if ( offset < 0 ) {
                return;
        }

        if ( !CG_SelectGhostFrames( recording, offset, &from, &to, &lerp ) ) {
                return;
        }

	ci = &cgs.clientinfo[cg.snap->ps.clientNum];
	if ( !ci->bodyModel ) {
		return;
	}

        for ( i = 0; i < 3; i++ ) {
                origin[i] = from->origin[i] + lerp * ( to->origin[i] - from->origin[i] );
                angles[i] = from->angles[i] + lerp * AngleSubtract( to->angles[i], from->angles[i] );
        }

	ghostAlpha = CG_GetGhostAlpha();

        memset( &ghost, 0, sizeof( ghost ) );
	ghost.hModel = ci->bodyModel;
	ghost.customSkin = ci->bodySkin;
	ghost.customShader = cgs.media.ghostShader;
	VectorCopy( origin, ghost.origin );
	VectorCopy( origin, ghost.lightingOrigin );
        ghost.renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;
        AnglesToAxis( angles, ghost.axis );
        ghost.shaderRGBA[0] = 255;
        ghost.shaderRGBA[1] = 255;
        ghost.shaderRGBA[2] = 255;
        ghost.shaderRGBA[3] = ghostAlpha;

        trap_R_AddRefEntityToScene( &ghost );

        CG_AddGhostWheels( ci, &ghost, ghostAlpha );
}



static qboolean CG_PointInsideCheckpointBounds( const centity_t *checkpoint, const vec3_t point ) {
        vec3_t mins, maxs;
        int i;

        if ( !checkpoint || checkpoint->currentState.eType != ET_CHECKPOINT ) {
                return qfalse;
        }

        trap_R_ModelBounds( cgs.inlineDrawModel[checkpoint->currentState.modelindex], mins, maxs );

        if ( checkpoint->currentState.frame == 0 ) {
                VectorAdd( mins, checkpoint->currentState.origin, mins );
                VectorAdd( maxs, checkpoint->currentState.origin, maxs );
        }

        for ( i = 0; i < 3; i++ ) {
                if ( point[i] < mins[i] || point[i] > maxs[i] ) {
                        return qfalse;
                }
        }

        return qtrue;
}

static centity_t *CG_FindCheckpointEntity( int checkpointNumber ) {
        int i;

        for ( i = 0; i < MAX_GENTITIES; i++ ) {
                centity_t *cent = &cg_entities[i];

                if ( cent->currentState.eType != ET_CHECKPOINT ) {
                        continue;
                }

                if ( cent->currentState.weapon == checkpointNumber ) {
                        return cent;
                }
        }

        return NULL;
}

static qboolean CG_GetGhostCheckpointSplitMs( ghostRecording_t *recording, int checkpointNumber, int *splitMsOut ) {
        int i;
        centity_t *checkpoint;

        if ( !recording || !recording->valid || recording->frameCount <= 0 || !splitMsOut ) {
                return qfalse;
        }

        checkpoint = CG_FindCheckpointEntity( checkpointNumber );
        if ( !checkpoint ) {
                return qfalse;
        }

        for ( i = 0; i < recording->frameCount; i++ ) {
                int index = ( recording->startIndex + i ) % MAX_GHOST_FRAMES;
                ghostFrame_t *frame = &recording->frames[index];

                if ( CG_PointInsideCheckpointBounds( checkpoint, frame->origin ) ) {
                        *splitMsOut = frame->timeOffset;
                        return qtrue;
                }
        }

        return qfalse;
}

void CG_UpdateGhostSplitDelta( void ) {
        centity_t *cent;
        int nextCheckpoint;
        int startLapTime;
        int crossedCheckpoint;
        ghostRecording_t *recording;
        int ghostSplitMs;
        int localElapsed;
        int deltaMs;

        if ( !cg.snap || cg.snap->ps.clientNum >= MAX_CLIENTS ) {
                return;
        }

        cent = &cg_entities[cg.snap->ps.clientNum];
        nextCheckpoint = cg.snap->ps.stats[STAT_NEXT_CHECKPOINT];
        startLapTime = cent->startLapTime;

        if ( !startLapTime || cent->finishRaceTime || nextCheckpoint <= 0 ) {
                cg.ghostSplitLastNextCheckpoint = nextCheckpoint;
                cg.ghostSplitLastLapStartTime = startLapTime;
                return;
        }

        if ( cg.ghostSplitLastLapStartTime != startLapTime || cg.ghostSplitLastNextCheckpoint <= 0 ) {
                cg.ghostSplitLastLapStartTime = startLapTime;
                cg.ghostSplitLastNextCheckpoint = nextCheckpoint;
                return;
        }

        if ( nextCheckpoint == cg.ghostSplitLastNextCheckpoint ) {
                return;
        }

        if ( nextCheckpoint < cg.ghostSplitLastNextCheckpoint ) {
                cg.ghostSplitLastNextCheckpoint = nextCheckpoint;
                return;
        }

        crossedCheckpoint = nextCheckpoint - 1;

        recording = CG_GetActiveGhostRecording();
        if ( !recording || !recording->valid ) {
                cg.ghostSplitLastNextCheckpoint = nextCheckpoint;
                return;
        }

        if ( !CG_GetGhostCheckpointSplitMs( recording, crossedCheckpoint, &ghostSplitMs ) ) {
                cg.ghostSplitLastNextCheckpoint = nextCheckpoint;
                return;
        }

        localElapsed = cg.time - startLapTime;
        if ( localElapsed < 0 ) {
                localElapsed = 0;
        }

        deltaMs = localElapsed - ghostSplitMs;
        cg.ghostSplitDeltaMs = deltaMs;
        cg.ghostSplitDeltaTime = cg.time;
        cg.ghostSplitDeltaValid = qtrue;

        trap_Cvar_Update( &cg_ghostSplitAudio );
        if ( cg_ghostSplitAudio.integer && deltaMs < 0 && cgs.media.excellentSound ) {
                trap_S_StartLocalSound( cgs.media.excellentSound, CHAN_ANNOUNCER );
        }

        cg.ghostSplitLastNextCheckpoint = nextCheckpoint;
}

void CG_NewLapTime( int client, int lap, int time ) {
	centity_t	*cent;
	char		*t;

	cent = &cg_entities[client];

        if ((time - cent->startLapTime) < cent->bestLapTime || cent->bestLapTime == 0){
                // New bestlap
                cent->bestLapTime = (time - cent->startLapTime);
                cent->bestLap = cent->currentLap;
                cent->bestLapStartTime = cent->startLapTime;

		if ( client == cg.snap->ps.clientNum ) {
			t = getStringForTime( cent->bestLapTime );

			Com_Printf("You got a personal record lap time of %s!\n", t);
		}
	}

	cent->currentLap = lap;
	cent->lastStartLapTime = cent->startLapTime;
	cent->startLapTime = time;
}

void CG_FinishedRace( int client, int time ) {
	centity_t	*cent;
	char		*t;

	cent = &cg_entities[client];

        if ( client == cg.snap->ps.clientNum
                && ((time - cent->startLapTime) < cent->bestLapTime || cent->bestLapTime == 0) ){
                // New bestlap
                cent->bestLapTime = (time - cent->startLapTime);
                cent->bestLap = cent->currentLap;
                cent->bestLapStartTime = cent->startLapTime;

		t = getStringForTime( cent->bestLapTime );

		Com_Printf("You got a personal record lap time of %s!\n", t);
	}

        cent->finishRaceTime = time;

        if ( client == cg.snap->ps.clientNum ) {
                CG_EndGhostRecording( time );
                CG_AttemptSavePersonalGhost( time );
        }

        if ( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_LCS ) {
		int lastClient;
		int remaining;

		remaining = CG_GetPlayersRemaining( &lastClient );
		CG_CheckEliminationWarning( remaining );
	}
}

void CG_StartRace( int time ) {
	int			i;
	centity_t	*player;

        for (i = 0; i < MAX_CLIENTS; i++){
                player = &cg_entities[i];
                if (!player) continue;

                player->startRaceTime = time;
                player->finishRaceTime = 0;
                player->startLapTime = time;
                player->currentLap = 1;
                player->bestLapTime = 0;
                player->bestLapStartTime = 0;
                player->lastStartLapTime = 0;
        }

        if ( isRallyRace() ) {
                CG_LoadPersonalGhost();
                CG_BeginGhostRecording( time );
        } else {
                CG_ResetPersonalGhost();
                cg.ghostRecordingActive = qfalse;
                cg.ghostRecording.valid = qfalse;
        }

        cg.ghostSplitLastNextCheckpoint = 0;
        cg.ghostSplitLastLapStartTime = 0;
        cg.ghostSplitDeltaMs = 0;
        cg.ghostSplitDeltaTime = 0;
        cg.ghostSplitDeltaValid = qfalse;

        cg.eliminationWarningActive = qfalse;
        cg.eliminationWarningTime = 0;
        cg.eliminationPlayersRemaining = CG_GetPlayersRemaining( NULL );
}

void CG_DrawRaceCountDown( void ){
	float	f, scale;
	int		x, y, w, h;
	vec4_t	color;

	if (cg.countDownEnd + 1000 < cg.time || cg.countDownPrint[0] == 0)
		return;

	f = cg.countDownEnd < cg.time ? 0.0f : (cg.countDownEnd - cg.time) / 3000.0f;

	color[0] = 1.0f * f;
	color[1] = 1.0f * (1-f);
	color[2] = 0;
	color[3] = 1.0f;

	scale = cg.countDownEnd < cg.time ? 0.8f : ((cg.countDownEnd - cg.time) % 1000) / 1000.0f;
	w = 3*GIANTCHAR_WIDTH * scale;
	h = 3*GIANTCHAR_HEIGHT * scale;
	x = 320 - (strlen(cg.countDownPrint) * w) / 2;
	y = 240 - h/2;
	CG_DrawStringExt( x, y, cg.countDownPrint, color, qfalse, qtrue, w, h, 0 );
}

void CG_RaceCountDown( const char *str, int secondsLeft ){
	cg.centerPrintTime = 0;
	cg.countDownEnd = cg.time + secondsLeft * 1000;
	Q_strncpyz( cg.countDownPrint, str, sizeof(cg.countDownPrint) );
}
