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

#include "g_local.h"

static void G_RecordClientLapTime( gclient_t *client, int lapNumber, int lapTime ) {
	if ( !client || lapNumber <= 0 || lapTime <= 0 ) {
		return;
	}

	if ( client->ladderBestLapMs == 0 || lapTime < client->ladderBestLapMs ) {
		client->ladderBestLapMs = lapTime;
	}

	if ( lapNumber > client->ladderLapCount ) {
		client->ladderLapCount = lapNumber;
	}

	if ( lapNumber > 0 && lapNumber <= RACE_MAX_RECORDED_LAPS ) {
		client->ladderLapTimes[ lapNumber - 1 ] = lapTime;
	}
}

static void G_UpdateClientTotalRaceTime( gclient_t *client, int finishTime ) {
	if ( !client ) {
		return;
	}

	if ( level.startRaceTime > 0 && finishTime >= level.startRaceTime ) {
		client->ladderTotalRaceMs = finishTime - level.startRaceTime;
	} else {
		client->ladderTotalRaceMs = 0;
	}
}

// *********************** Race Entities ************************
// *********************** Race Entities ************************
// *********************** Race Entities ************************
// *********************** Race Entities ************************
// *********************** Race Entities ************************

#define CHECKPOINT_SOUNDS		1
#define CHECKPOINT_MESSAGES		2


void Touch_Start( gentity_t *self, gentity_t *other, trace_t *trace ) {
	if ( !other->client ) {
		return;
	}

	if ( other->client->lastCheckpointTime + 300 > level.time ) {
		return;
	}

	if ( g_developer.integer )
		G_Printf( "Client %i touched the start line.\n", other->s.clientNum );

	other->client->lastCheckpointTime = level.time;
	other->number = 1;
	other->client->ps.stats[STAT_NEXT_CHECKPOINT] = other->number;
	other->client->ps.stats[STAT_FRAC_TO_NEXT_CHECKPOINT] = FLOAT2SHORT( 0.1f );

	if ( level.startRaceTime && other->client->ladderLastLapStartMs <= 0 ) {
		other->client->ladderLastLapStartMs = level.time;
	}

	trap_SendServerCommand( -1, va( "newLapTime %i %i %i", other->s.clientNum, 1, level.time ) );

	Rally_Sound( self, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex( "sound/rally/race/checkpoint.wav" ) );
}

void Touch_Finish( gentity_t *self, gentity_t *other, trace_t *trace ) {
	char *place;
	int lapTime = 0;
	int lapNumber;

	if ( !other->client ) {
		return;
	}

	if ( other->client->lastCheckpointTime + 300 > level.time ) {
		return;
	}

	if ( g_developer.integer )
		G_Printf( "Client %i touched the finish line.\n", other->s.clientNum );

	if ( self->number != other->number ) {
		return;
	}

	if ( level.startRaceTime && other->client->ladderLastLapStartMs > 0
			&& level.time >= other->client->ladderLastLapStartMs ) {
		lapTime = level.time - other->client->ladderLastLapStartMs;
	}

	lapNumber = other->currentLap > 0 ? other->currentLap : other->client->ladderLapCount + 1;
	if ( lapTime > 0 ) {
		G_RecordClientLapTime( other->client, lapNumber, lapTime );
	}

	other->client->lastCheckpointTime = level.time;
	other->client->finishRaceTime = level.time;
	other->s.weapon = WP_NONE;
	other->takedamage = qfalse;

	G_UpdateClientTotalRaceTime( other->client, level.time );

	trap_SendServerCommand( -1, va( "raceFinishTime %i %i", other->s.clientNum, other->client->finishRaceTime ) );

	if ( !level.finishRaceTime ) {
		other->client->ps.stats[STAT_POSITION] = 1;

		level.winnerNumber = other->s.clientNum;
		level.finishRaceTime = level.time;
		trap_SendServerCommand( -1, va( "print \"%s won the race!\n\"", other->client->pers.netname ) );
		trap_SendServerCommand( level.winnerNumber, "cp \"You won the race!\n\"" );
	} else {
		switch ( other->client->ps.stats[STAT_POSITION] ) {
		case 1:
			place = "first";
			break;
		case 2:
			place = "second";
			break;
		case 3:
			place = "third";
			break;
		case 4:
			place = "forth";
			break;
		case 5:
			place = "fifth";
			break;
		case 6:
			place = "sixth";
			break;
		case 7:
			place = "seventh";
			break;
		case 8:
			place = "eighth";
			break;
		default:
			place = NULL;
			Com_Printf( "Unknown placing: %i\n", other->client->ps.stats[STAT_POSITION] );
			break;
		}

		if ( other->client->ps.stats[STAT_POSITION] <= 8 ) {
			trap_SendServerCommand( -1, va( "print \"%s finished the race in %s place!\n\"", other->client->pers.netname, place ) );
		} else {
			trap_SendServerCommand( -1, va( "print \"%s finished the race!\n\"", other->client->pers.netname ) );
		}
	}
}

void Touch_StartFinish( gentity_t *self, gentity_t *other, trace_t *trace ) {
	char	*place;
	int lapTime = 0;
	int completedLap;

	if ( !other->client ) {
		return;
	}

	// Debounce: prevent triggering too quickly
	if ( other->client->lastCheckpointTime + 300 > level.time ) {
		return;
	}

	if ( g_developer.integer )
		G_Printf( "Client %i touched the startfinish line.  Checkpoint number %i\n", other->s.clientNum, self->number );

	if ( other->currentLap > level.numberOfLaps && level.numberOfLaps ) {
		return;
	}

	if ( self->number == other->number ) {
		completedLap = other->currentLap;
		if ( level.startRaceTime && other->client->ladderLastLapStartMs > 0
				&& level.time >= other->client->ladderLastLapStartMs
				&& completedLap > 0 ) {
			lapTime = level.time - other->client->ladderLastLapStartMs;
			if ( lapTime > 0 ) {
				G_RecordClientLapTime( other->client, completedLap, lapTime );
			}
		} else if ( other->client->ladderLastLapStartMs <= 0 ) {
			other->client->ladderLastLapStartMs = level.time;
		}

		other->client->lastCheckpointTime = level.time;
		other->currentLap++;
		// increment lap
		if ( other->currentLap > level.numberOfLaps && level.numberOfLaps ) {
			other->client->finishRaceTime = level.time;
			other->s.weapon = WP_NONE;
			other->takedamage = qfalse;

			G_UpdateClientTotalRaceTime( other->client, level.time );

			trap_SendServerCommand( -1, va( "raceFinishTime %i %i", other->s.clientNum, other->client->finishRaceTime ) );

			if ( !level.finishRaceTime ) {
				other->client->ps.stats[STAT_POSITION] = 1; // make sure the player is first

				level.winnerNumber = other->s.clientNum;
				level.finishRaceTime = level.time;
				trap_SendServerCommand( -1, va( "print \"%s won the race!\n\"", other->client->pers.netname ) );
				trap_SendServerCommand( level.winnerNumber, "cp \"You won the race!\n\"" );
			} else {
				switch ( other->client->ps.stats[STAT_POSITION] ) {
				case 1:
					place = "first";
					break;
				case 2:
					place = "second";
					break;
				case 3:
					place = "third";
					break;
				case 4:
					place = "forth";
					break;
				case 5:
					place = "fifth";
					break;
				case 6:
					place = "sixth";
					break;
				case 7:
					place = "seventh";
					break;
				case 8:
					place = "eighth";
					break;
				default:
					place = NULL;
					Com_Printf( "Unknown placing: %i\n", other->client->ps.stats[STAT_POSITION] );
					break;
				}

				if ( other->client->ps.stats[STAT_POSITION] <= 8 ) {
					trap_SendServerCommand( -1, va( "print \"%s finished the race in %s place!\n\"", other->client->pers.netname, place ) );
				} else {
					trap_SendServerCommand( -1, va( "print \"%s finished the race!\n\"", other->client->pers.netname ) );
				}
			}
		} else {
			other->number = 1;
			other->client->ps.stats[STAT_NEXT_CHECKPOINT] = other->number;
			other->client->ps.stats[STAT_FRAC_TO_NEXT_CHECKPOINT] = FLOAT2SHORT( 0.1f );
//			Com_Printf( "resetting frac, sf\n" );
			trap_SendServerCommand( -1, va( "newLapTime %i %i %i", other->s.clientNum, other->currentLap, level.time ) );
			other->client->ladderLastLapStartMs = level.time;
		}


		if ( other->currentLap == level.numberOfLaps ) {
			trap_SendServerCommand( other->s.number, "cp \"Final lap\n\"" );
			Rally_Sound( self, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex( "sound/rally/race/finallap.wav" ) );
		} else {
			Rally_Sound( self, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex( "sound/rally/race/checkpoint.wav" ) );
		}
	}
}
void Think_StartFinish( gentity_t *self ){
	gentity_t		*ent;
	int		checkpoints;

	// FIXME: only do this a couple times after a client joins
	// send checkpoint to clients
/*
	if ((level.time / 2000) % 2)
		self->r.svFlags |= SVF_BROADCAST;
	else
		self->r.svFlags |= SVF_NOCLIENT;

	self->nextthink = level.time + 2000;
*/
	// if there is a target use its origin and angles instead
	if ( self->target ){
		ent = G_PickTarget( self->target );
		if (ent){
			VectorCopy(ent->s.origin, self->s.origin);
			VectorCopy(ent->s.angles, self->s.angles);
			self->s.frame = 1;

			G_FreeEntity( ent );
		}
		self->target = 0;
	}

	if( self->s.origin2[0] == 0.0f &&
		self->s.origin2[1] == 0.0f &&
		self->s.origin2[2] == 0.0f &&
		( self->s.origin[0] != 0.0f ||
		self->s.origin[1] != 0.0f ||
		self->s.origin[2] != 0.0f ) )
		VectorCopy( self->s.origin, self->s.origin2 );

	if ( self->touch == Touch_Start ) {
		VectorCopy( self->s.origin, level.startOrigin );
		level.hasStart = qtrue;
	}
	else if ( self->touch == Touch_Finish ) {
		VectorCopy( self->s.origin, level.finishOrigin );
		level.hasFinish = qtrue;
	}
	else if ( self->touch == Touch_StartFinish ) {
		VectorCopy( self->s.origin, level.startOrigin );
		VectorCopy( self->s.origin, level.finishOrigin );
		level.hasStart = qtrue;
		level.hasFinish = qtrue;
	}

	checkpoints = 0;

        ent = NULL;
        while ((ent = G_Find (ent, FOFS(classname), "rally_checkpoint")) != NULL) checkpoints++;
        level.numCheckpoints = checkpoints;
        if (g_trackReversed.integer && level.trackIsReversable){
                ent = NULL;
                while ((ent = G_Find (ent, FOFS(classname), "rally_checkpoint")) != NULL) {
                        ent->number = level.numCheckpoints - ent->number;
                }
        }

       memset( level.checkpoints, 0, sizeof( level.checkpoints ) );
       memset( level.cpDist, 0, sizeof( level.cpDist ) );
       ent = NULL;
       while ( ( ent = G_Find( ent, FOFS(classname), "rally_checkpoint" ) ) != NULL ) {
               if ( ent->number > 0 && ent->number <= level.numCheckpoints ) {
                       level.checkpoints[ ent->number - 1 ] = ent;
               }
       }

       {
               qboolean missing = qfalse;
               int i;

               for ( i = 0; i < level.numCheckpoints; i++ ) {
                       if ( !level.checkpoints[i] ) {
                               missing = qtrue;
                               if ( g_developer.integer ) {
                                       Com_Printf( "rally_checkpoint %d not found\n", i + 1 );
                               }
                       }
               }

               level.trackLength = 0.0f;
               if ( !missing && level.numCheckpoints > 0 ) {
                       vec3_t last, first, delta;
                       float startDist;
                       qboolean loopTrack;

                       VectorCopy( level.checkpoints[0]->s.origin, first );
                       VectorCopy( first, last );

                       startDist = 0.0f;
                       if ( level.hasStart ) {
                               startDist = Distance( level.startOrigin, first );
                               level.trackLength += startDist;
                       }
                       level.cpDist[0] = startDist;
                       for ( i = 1; i < level.numCheckpoints; i++ ) {
                               float segmentLength;

                               VectorSubtract( last, level.checkpoints[i]->s.origin, delta );
                               segmentLength = VectorLength( delta );
                               level.trackLength += segmentLength;
                               level.cpDist[i] = level.cpDist[i-1] + segmentLength;
                               if ( g_developer.integer && level.cpDist[i] <= level.cpDist[i-1] ) {
                                       Com_Printf( "Checkpoint distance %d is non-increasing\n", i + 1 );
                               }
                               VectorCopy( level.checkpoints[i]->s.origin, last );
                       }

                       loopTrack = ( !level.hasFinish ||
                               ( level.hasStart && VectorCompare( level.startOrigin, level.finishOrigin ) ) );
                       if ( level.hasFinish && !loopTrack ) {
                               level.trackLength += Distance( last, level.finishOrigin );
                       } else {
                               VectorSubtract( last, first, delta );
                               level.trackLength += VectorLength( delta );
                       }
               } else if ( g_developer.integer ) {
                       Com_Printf( "Track length not calculated due to missing checkpoints\n" );
               }
       }

       if ( level.hasFinish ) {
               if ( level.numCheckpoints == 0 ) {
                       level.trackLength = Distance( level.startOrigin, level.finishOrigin );
               }
       }

       trap_SetConfigstring( CS_TRACKLENGTH, va( "%i", (int)( level.trackLength / CP_M_2_QU ) ) );

	self->number = level.numCheckpoints;
	self->s.weapon = self->number;
}

void Think_Finish( gentity_t *self ){
Think_StartFinish( self );
self->number++;
self->s.weapon = self->number;
}


void SP_rally_startfinish( gentity_t *ent ) {
	ent->classname = "rally_checkpoint";

	trap_SetBrushModel( ent, ent->model );

	if (!g_laplimit.integer){
		level.numberOfLaps = ent->laps;
		trap_Cvar_Set( "laplimit", va("%d", level.numberOfLaps) );
	}
	else
		level.numberOfLaps = g_laplimit.integer;

// STONELANCE - April 23, 2002 temp for testing bezier curve stuff
	ent->r.svFlags |= SVF_BROADCAST;
//
	ent->s.eType = ET_CHECKPOINT;

	ent->touch = Touch_StartFinish;
	ent->think = Think_StartFinish;
	ent->nextthink = level.time + 300;
	ent->s.frame = 0;

	trap_LinkEntity (ent);
}

void SP_rally_start( gentity_t *ent ) {
trap_SetBrushModel( ent, ent->model );

level.numberOfLaps = 1;
trap_Cvar_Set( "laplimit", "1" );

ent->r.svFlags |= SVF_BROADCAST;
ent->s.eType = ET_CHECKPOINT;

ent->touch = Touch_Start;
ent->think = Think_StartFinish;
ent->nextthink = level.time + 300;
ent->s.frame = 0;

trap_LinkEntity (ent);
}

void SP_rally_finish( gentity_t *ent ) {
trap_SetBrushModel( ent, ent->model );

ent->r.svFlags |= SVF_BROADCAST;
ent->s.eType = ET_CHECKPOINT;

ent->touch = Touch_Finish;
ent->think = Think_Finish;
ent->nextthink = level.time + 300;
ent->s.frame = 0;

level.finishLine = ent;
VectorCopy( ent->s.origin, level.finishOrigin );
level.hasFinish = qtrue;

trap_LinkEntity (ent);
}

//
// rally_checkpoint
//

void Touch_Checkpoint (gentity_t *self, gentity_t *other, trace_t *trace ){
	if ( !other->client ) {
		return;
	}

	// Debounce: prevent triggering too quickly
	if ( other->client->lastCheckpointTime + 300 > level.time ) {
		return;
	}

	if (g_developer.integer)
		G_Printf( "Client %i touched checkpoint number %i\n", other->s.clientNum, self->number );

	if (self->number == other->number){
		other->client->lastCheckpointTime = level.time;
		other->number++;	// FIXME: get rid of number? use s.weapon instead?
		other->client->ps.stats[STAT_NEXT_CHECKPOINT] = other->number;
		other->client->ps.stats[STAT_FRAC_TO_NEXT_CHECKPOINT] = FLOAT2SHORT(0.1f);
//		Com_Printf( "resetting frac, cp\n" );

		if (self->spawnflags & CHECKPOINT_SOUNDS)
			Rally_Sound( self, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex("sound/rally/race/checkpoint.wav") );

		if ( self->spawnflags & CHECKPOINT_MESSAGES && self->s.otherEntityNum != -1 &&
			self->s.otherEntityNum != other->s.number )
		{
			if ( g_entities[self->s.otherEntityNum].client->ps.stats[STAT_POSITION] < other->client->ps.stats[STAT_POSITION] )
			{
				trap_SendServerCommand( other->s.number,
					va("print \"%s is ahead by %i seconds\n\"",
					g_entities[self->s.otherEntityNum].client->pers.netname,
					(level.time - self->updateTime) / 1000) );
			}
		}

		self->s.otherEntityNum = other->s.number;
		self->updateTime = level.time;
	}
}


void Think_Checkpoint( gentity_t *self ){
	gentity_t		*ent;

/*
	// FIXME: only do this a couple times after a client joins
	// send checkpoint to clients
	if ((level.time / 2000) % 2){
		Com_Printf("Broadcast %d\n", self->s.number);
		self->r.svFlags |= SVF_BROADCAST;
		trap_LinkEntity (ent);
	}
	else{
		Com_Printf("Noclient\n");
		self->r.svFlags |= SVF_NOCLIENT;
		trap_LinkEntity (ent);
	}

	self->nextthink = level.time + 2000;
*/
	// if there is a target use its origin and angles instead
	if ( self->target ){
		ent = G_PickTarget( self->target );
		if (ent){
			VectorCopy(ent->s.origin, self->s.origin);
			VectorCopy(ent->s.angles, self->s.angles);
			self->s.frame = 1;

			G_FreeEntity( ent );
		}
		self->target = 0;
	}

	if( self->s.origin2[0] == 0.0f &&
		self->s.origin2[1] == 0.0f &&
		self->s.origin2[2] == 0.0f && 
		( self->s.origin[0] != 0.0f || 
		self->s.origin[1] != 0.0f || 
		self->s.origin[2] != 0.0f ) )
		VectorCopy( self->s.origin, self->s.origin2 );

	self->s.weapon = self->number;
}

//	spawnflag 1 enable messages, spawn flag 2 enable sound, 3 is enable both
void SP_rally_checkpoint( gentity_t *ent ) {
	trap_SetBrushModel( ent, ent->model );
	G_SetOrigin( ent, ent->s.origin );

// STONELANCE - April 23, 2002 temp for testing bezier curve stuff
	ent->r.svFlags |= SVF_BROADCAST;
//
	ent->s.eType = ET_CHECKPOINT;

	ent->think = Think_Checkpoint;
	ent->nextthink = level.time + 200;

	ent->touch = Touch_Checkpoint;
	ent->s.frame = 0;

	trap_LinkEntity (ent);
}

void SP_rally_sun( gentity_t *ent ){
//	ent->s.eType = ET_LIGHT;

	G_SetOrigin(ent, ent->s.origin);

	trap_LinkEntity (ent);
}


// FIXME: improve these so they only need to be send to the client once?
void SP_rally_weather_rain( gentity_t *ent ){
	trap_SetBrushModel( ent, ent->model );
	ent->s.eType = ET_WEATHER;

	ent->s.powerups = ent->number;
	ent->s.weapon = 0;
	ent->s.legsAnim = ent->spawnflags;

	trap_LinkEntity (ent);
}


void SP_rally_weather_snow( gentity_t *ent ){
	trap_SetBrushModel( ent, ent->model );
	ent->s.eType = ET_WEATHER;

	ent->s.powerups = ent->number;
	ent->s.weapon = 1;
	ent->s.legsAnim = 0;

	trap_LinkEntity (ent);
}

