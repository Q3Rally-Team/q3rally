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

static qboolean CG_LoadGhostFile( const char *path, const char *expectedMap, const char *expectedVehicle, int declaredBestTime,
                ghostRecording_t *target, int *bestTimeOut, char *vehicleOut, int vehicleOutSize, char *pathOut, int pathOutSize );

static void QDECL CG_GhostDebugPrint( const char *fmt, ... ) {
	va_list argptr;
	char text[1024];

	if ( !cg_ghostDebug.integer ) {
		return;
	}

	va_start( argptr, fmt );
	Q_vsnprintf( text, sizeof( text ), fmt, argptr );
	va_end( argptr );

	CG_Printf( "CG_GhostDebug: %s\n", text );
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
        char mapname[MAX_QPATH];
        char vehicle[MAX_QPATH];
        char path[MAX_QPATH];

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

        Q_strncpyz( vehicle, cgs.clientinfo[cg.snap->ps.clientNum].modelName, sizeof( vehicle ) );
        if ( !vehicle[0] ) {
                return;
        }

        Com_sprintf( path, sizeof( path ), "ghosts/%s_%s.ghost", mapname, vehicle );

        if ( CG_LoadGhostFile( path, mapname, vehicle, 0, &cg.ghostPlayback, &cg.personalGhostBestTime, cg.personalGhostVehicle, sizeof( cg.personalGhostVehicle ), cg.personalGhostPath, sizeof( cg.personalGhostPath ) ) ) {
                cg.personalGhostAvailable = qtrue;
        }
}


static qboolean CG_LoadGhostFile( const char *path, const char *expectedMap, const char *expectedVehicle, int declaredBestTime, ghostRecording_t *target, int *bestTimeOut, char *vehicleOut, int vehicleOutSize, char *pathOut, int pathOutSize ) {
	fileHandle_t file;
	int length;
	static char buffer[MAX_GHOST_FILE_SIZE+1];
	char *line;
	char mapName[MAX_QPATH] = "";
	char vehicle[MAX_QPATH] = "";
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

	CG_GhostDebugPrint( "Loading ghost '%s' (expected map='%s' vehicle='%s')", path, expectedMap ? expectedMap : "", expectedVehicle ? expectedVehicle : "" );

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

		if ( !Q_stricmpn( cursor, "map", 3 ) ) {
			const char *value = cursor + 3;
			while ( *value == ' ' || *value == '\t' ) {
				++value;
			}
			Q_strncpyz( mapName, value, sizeof( mapName ) );
			CG_ChopNewline( mapName );
			CG_GhostDebugPrint( "%s line %d: map set to '%s'", path, lineNumber, mapName );
			goto nextLine;
		}

		if ( !Q_stricmpn( cursor, "vehicle", 7 ) ) {
			const char *value = cursor + 7;
			while ( *value == ' ' || *value == '\t' ) {
				++value;
			}
			Q_strncpyz( vehicle, value, sizeof( vehicle ) );
			CG_ChopNewline( vehicle );
			CG_GhostDebugPrint( "%s line %d: vehicle set to '%s'", path, lineNumber, vehicle );
			goto nextLine;
		}

		if ( !Q_stricmpn( cursor, "best_time_ms", 12 ) ) {
			const char *value = cursor + 12;
			while ( *value == ' ' || *value == '\t' ) {
				++value;
			}
			bestTimeMs = atoi( value );
			CG_GhostDebugPrint( "%s line %d: best_time_ms set to %d", path, lineNumber, bestTimeMs );
			goto nextLine;
		}

		if ( !Q_stricmpn( cursor, "frames", 6 ) ) {
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

	CG_GhostDebugPrint( "%s summary: map='%s' vehicle='%s' frames=%d duration=%d bestTimeMs=%d", path, mapName, vehicle, frameCount, lastOffset, bestTimeMs );

	if ( expectedMap && expectedMap[0] && mapName[0] && Q_stricmp( expectedMap, mapName ) ) {
			CG_Printf( "CG_Ghost: %s map '%s' does not match '%s'\n", path, mapName, expectedMap );
			return qfalse;
	}

	if ( expectedVehicle && expectedVehicle[0] && vehicle[0] && Q_stricmp( expectedVehicle, vehicle ) ) {
		CG_Printf( "CG_Ghost: %s vehicle '%s' does not match '%s'\n", path, vehicle, expectedVehicle );
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
        CG_ResetBaseGhost();

        cg.baseGhostAvailable = CG_LoadGhostFile( path, expectedMap, expectedVehicle, declaredBestTime, &cg.baseGhost, &cg.baseGhostBestTime, cg.baseGhostVehicle, sizeof( cg.baseGhostVehicle ), cg.baseGhostPath, sizeof( cg.baseGhostPath ) );

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

	if ( !( isRallyRace() || cgs.gametype == GT_DERBY || cgs.gametype == GT_LCS ) ) {
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
        fileHandle_t file;
        char mapname[MAX_QPATH];
        char vehicle[MAX_QPATH];
        char path[MAX_QPATH];
        int bestLapTime;
        int lapStartOffset;
        int lapEndOffset;
        int i;
        static ghostRecording_t lapRecording;
        ghostFrame_t *previousFrame;

        if ( !cg.snap || cg.snap->ps.clientNum >= MAX_CLIENTS ) {
                return;
        }

        if ( !cg.ghostRecording.valid || cg.ghostRecording.frameCount <= 1 ) {
                return;
        }

        if ( !cg_entities[cg.snap->ps.clientNum].startRaceTime || finishTime <= cg_entities[cg.snap->ps.clientNum].startRaceTime ) {
                return;
        }

        if ( !cgs.clientinfo[cg.snap->ps.clientNum].infoValid ) {
                return;
        }

        bestLapTime = cg_entities[cg.snap->ps.clientNum].bestLapTime;
        if ( bestLapTime <= 0 ) {
                return;
        }

        lapStartOffset = cg_entities[cg.snap->ps.clientNum].bestLapStartTime - cg_entities[cg.snap->ps.clientNum].startRaceTime;
        lapEndOffset = lapStartOffset + bestLapTime;

        if ( lapStartOffset < 0 || lapEndOffset > cg.ghostRecording.duration ) {
                return;
        }

        if ( cg.personalGhostBestTime > 0 && bestLapTime >= cg.personalGhostBestTime ) {
                return;
        }

        COM_StripExtension( COM_SkipPath( cgs.mapname ), mapname, sizeof( mapname ) );
        Q_strncpyz( vehicle, cgs.clientinfo[cg.snap->ps.clientNum].modelName, sizeof( vehicle ) );

        if ( !mapname[0] || !vehicle[0] ) {
                return;
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
                return;
        }

        lapRecording.startIndex = 0;
        lapRecording.writeIndex = lapRecording.frameCount % MAX_GHOST_FRAMES;
        lapRecording.duration = lapRecording.frames[lapRecording.frameCount - 1].timeOffset;
        lapRecording.valid = qtrue;

        Com_sprintf( path, sizeof( path ), "ghosts/%s_%s.ghost", mapname, vehicle );

        if ( trap_FS_FOpenFile( path, &file, FS_WRITE ) < 0 ) {
                CG_Printf( "CG_Ghost: failed to save %s\n", path );
                return;
        }

        {
                char header[64];

                Com_sprintf( header, sizeof( header ), "map %s\n", mapname );
                trap_FS_Write( header, strlen( header ), file );

                Com_sprintf( header, sizeof( header ), "vehicle %s\n", vehicle );
                trap_FS_Write( header, strlen( header ), file );

                Com_sprintf( header, sizeof( header ), "best_time_ms %d\n", bestLapTime );
                trap_FS_Write( header, strlen( header ), file );

                Com_sprintf( header, sizeof( header ), "frames %d\n", lapRecording.frameCount );
                trap_FS_Write( header, strlen( header ), file );
        }

        for ( i = 0; i < lapRecording.frameCount; i++ ) {
                int index = ( lapRecording.startIndex + i ) % MAX_GHOST_FRAMES;
                ghostFrame_t *frame = &lapRecording.frames[index];
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

        cg.ghostPlayback = lapRecording;
        cg.personalGhostAvailable = qtrue;
        cg.personalGhostBestTime = bestLapTime;
        Q_strncpyz( cg.personalGhostVehicle, vehicle, sizeof( cg.personalGhostVehicle ) );
        Q_strncpyz( cg.personalGhostPath, path, sizeof( cg.personalGhostPath ) );
}

static void CG_AddGhostWheels( clientInfo_t *ci, refEntity_t *body ) {
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
                wheel.shadowPlane = body->shadowPlane;
                wheel.renderfx = body->renderfx;
                VectorCopy( body->lightingOrigin, wheel.lightingOrigin );
                wheel.shaderRGBA[0] = 255;
                wheel.shaderRGBA[1] = 255;
                wheel.shaderRGBA[2] = 255;
                wheel.shaderRGBA[3] = 160;

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

        if ( cg_ghostPlayback.integer <= 0 ) {
                return;
        }

        if ( !( isRallyRace() || cgs.gametype == GT_DERBY || cgs.gametype == GT_LCS ) ) {
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

	memset( &ghost, 0, sizeof( ghost ) );
	ghost.hModel = ci->bodyModel;
	ghost.customSkin = ci->bodySkin;
	VectorCopy( origin, ghost.origin );
	VectorCopy( origin, ghost.lightingOrigin );
	ghost.renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;
	AnglesToAxis( angles, ghost.axis );
	ghost.shaderRGBA[0] = 255;
        ghost.shaderRGBA[1] = 255;
        ghost.shaderRGBA[2] = 255;
        ghost.shaderRGBA[3] = 160;

        trap_R_AddRefEntityToScene( &ghost );

        CG_AddGhostWheels( ci, &ghost );
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

        CG_LoadPersonalGhost();
        CG_BeginGhostRecording( time );

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
