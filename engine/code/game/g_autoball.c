/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/*
===========================================================================
Autoball: car football.

The ball is an ordinary dynamic rally_scripted_object with a sphere-shaped
Bullet body that never sleeps and has a speed cap. In GT_AUTOBALL the match
flow below adds kick-offs, goals (autoball_goal), the score and the
publishing of the match state to clients (CS_AUTOBALLSTATUS).

Ways to get a ball:
  - map entity "autoball_ball" (all rally_scripted_object keys apply), or
  - cheat commands on any map: ball_spawn, ball_reset, ball_remove.
Set g_autoballDebug 1 to print car and ball speed for every touch.

Live tuning (applied to every ball as soon as a value changes, overriding
map keys): g_autoballImpactScale, g_autoballVerticalScale, g_autoballLift,
g_autoballMass, g_autoballElasticity. Mass and elasticity rebuild the Bullet
body; the ball keeps its position and velocity.
===========================================================================
*/

#include "g_local.h"

#define AUTOBALL_CLASSNAME			"autoball_ball"

/* made by tools/autoball/make_sounds.py */
#define AUTOBALL_SOUND_HIT_SOFT		"sound/autoball/hit_soft.ogg"
#define AUTOBALL_SOUND_HIT_HARD		"sound/autoball/hit_hard.ogg"
#define AUTOBALL_SOUND_BOUNCE		"sound/autoball/bounce.ogg"
#define AUTOBALL_SOUND_GOAL_HORN	"sound/autoball/goal_horn.ogg"
#define AUTOBALL_SOUND_WHISTLE		"sound/autoball/whistle.ogg"
#define AUTOBALL_SOUND_CROWD		"sound/autoball/crowd_cheer.ogg"	/* crowds.ogg, faded out */
#define AUTOBALL_BOUNCE_SOUND_DV	280.0f	/* u/s velocity change that counts as a bounce */
#define AUTOBALL_MODEL				"models/autoball/ball.md3"	/* radius 75 */
#define AUTOBALL_DEFAULT_RADIUS		75.0f
#define AUTOBALL_DEFAULT_MAX_SPEED	3000.0f
#define AUTOBALL_SPAWN_DISTANCE		400.0f
#define AUTOBALL_MAX_TEST_BALLS		4
#define AUTOBALL_MIN_SCALE			0.5f
#define AUTOBALL_MAX_SCALE			2.0f
#define AUTOBALL_MAX_MATCH_BALLS	3
#define AUTOBALL_INTRO_MSEC			4000	/* extra wait at the first kick-off (bg_public.h mirrors it) */
#define AUTOBALL_EXTRA_BALL_SPACING	520.0f	/* extra balls sit beside the centre spot */

static gentity_t *G_Autoball_MainBall( void );

static void G_Autoball_ClearTouches( gentity_t *ball ) {
	int i;

	ball->ballLastToucher = -1;
	ball->ballLastTouchTime = 0;
	ball->s.generic1 &= ~SCRIPTED_GENERIC1_TEAM_MASK;
	for ( i = 0; i < 4; i++ ) {
		ball->ballTouchClient[i] = -1;
		ball->ballTouchTime[i] = 0;
	}
	ball->ballPendingClient = -1;
	ball->ballPendingTime = 0;
	ball->ballPendingThreat = TEAM_FREE;
}

/* The five values players tune most, taken from the g_autoball* cvars. */
static void G_Autoball_ApplyTuning( gentity_t *ent ) {
	ent->vehicleImpactScale = Com_Clamp( 0.0f, 2.0f, g_autoballImpactScale.value );
	ent->vehicleVerticalScale = Com_Clamp( 0.0f, 1.0f, g_autoballVerticalScale.value );
	ent->vehicleLift = Com_Clamp( 0.0f, 0.9f, g_autoballLift.value );
	ent->mass = (int)Com_Clamp( 1.0f, 100000.0f, g_autoballMass.value );
	ent->elasticity = Com_Clamp( 0.0f, 1.0f, g_autoballElasticity.value );
}

/*
Starting values from the design document. Map keys on autoball_ball
override them (radius, mass, elasticity, friction, rolling_friction,
spinning_friction, vehicle_impact_scale, vehicle_vertical_scale,
vehicle_lift, max_speed, never_sleep).
Order on a car contact: vertical_scale flattens the contact normal first,
then vehicle_lift guarantees a minimum upward share.
*/
static void G_Autoball_ApplyDefaults( gentity_t *ent ) {
	const float radius = AUTOBALL_DEFAULT_RADIUS;

	ent->moveable = qtrue;
	ent->inertiaShape = RALLY_OBJECT_INERTIA_SPHERE;
	ent->collisionShape = RALLY_PHYSICS_SHAPE_SPHERE;
	ent->ballRadius = radius;
	VectorSet( ent->r.mins, -radius, -radius, -radius );
	VectorSet( ent->r.maxs, radius, radius, radius );
	ent->friction = 0.4f;
	ent->rollingFriction = 0.02f;
	ent->spinningFriction = 0.02f;
	ent->weaponImpactScale = 1.0f;
	ent->neverSleep = qtrue;
	ent->maxSpeed = AUTOBALL_DEFAULT_MAX_SPEED;
	ent->health = 0;
	ent->maxHealth = 0;
	ent->takedamage = qfalse;
	G_Autoball_ClearTouches( ent );
	/* impact scale 1.6, vertical scale 0.45 (contacts point 35-45 deg up,
	 * this flattens them to ~20 deg), lift 0.15, mass 400, elasticity 0.6 */
	G_Autoball_ApplyTuning( ent );
	/* the client must not predict the car against the ball (see cg_predict.c) */
	ent->s.generic1 |= SCRIPTED_GENERIC1_NO_PREDICT;
	/* always in every snapshot: the HUD indicator and ball camera need it */
	ent->r.svFlags |= SVF_BROADCAST;
}

/*
Mutator g_autoballBallScale: scales the ball (physics radius and model).
Lifts the spawn point so a bigger ball does not start inside the floor.
Clients read the radius from s.time2 and the model scale from s.angles2[0].
*/
static float G_Autoball_BallScale( void ) {
	return Com_Clamp( AUTOBALL_MIN_SCALE, AUTOBALL_MAX_SCALE,
		g_autoballBallScale.value > 0.0f ? g_autoballBallScale.value : 1.0f );
}

static void G_Autoball_ApplyScale( gentity_t *ent ) {
	float scale = G_Autoball_BallScale();
	float radius = ent->ballRadius > 0.0f ? ent->ballRadius : AUTOBALL_DEFAULT_RADIUS;
	vec3_t origin;

	if ( scale != 1.0f ) {
		VectorCopy( ent->s.pos.trBase, origin );
		origin[2] += radius * ( scale - 1.0f );
		G_SetOrigin( ent, origin );
		VectorCopy( origin, ent->s.origin );
		radius *= scale;
		ent->ballRadius = radius;
		VectorSet( ent->r.mins, -radius, -radius, -radius );
		VectorSet( ent->r.maxs, radius, radius, radius );
	}
	ent->s.time2 = (int)( radius + 0.5f );
	ent->s.angles2[0] = scale;
}

/* Mutator g_autoballBallGravity: Bullet has one world gravity, so a lighter
   or heavier ball gets the difference as an impulse every frame. */
static void G_Autoball_BallGravity( gentity_t *ball ) {
	float scale, frameSec;
	vec3_t impulse;

	scale = Com_Clamp( 0.1f, 2.0f, g_autoballBallGravity.value );
	if ( scale == 1.0f || !G_RallyPhysics_Enabled() || ball->mass <= 0 )
		return;
	frameSec = ( level.time - level.previousTime ) * 0.001f;
	if ( frameSec <= 0.0f )
		return;
	if ( frameSec > 0.1f )
		frameSec = 0.1f;	/* G_RallyPhysics_RunFrame simulates at most 100 ms per frame */
	VectorSet( impulse, 0.0f, 0.0f, ball->mass * g_gravity.value * ( 1.0f - scale ) * frameSec );
	trap_RallyPhysicsApplyImpulse( ball->s.number, ball->r.currentOrigin, impulse );
}

static void G_Autoball_WarnLegacySolver( void ) {
	if ( !G_RallyPhysics_Enabled() ) {
		G_Printf( S_COLOR_YELLOW "autoball: Bullet backend is off (g_scriptedObjectBullet 0); "
			"the ball falls back to the legacy box solver and will not roll like a sphere.\n" );
	}
}

/*
QUAKED autoball_ball (1 .5 0) (-75 -75 -75) (75 75 75)
Autoball game ball. Its origin is the kick-off spot.
Keys: same as rally_scripted_object; defaults are tuned for Autoball.
"model" defaults to models/autoball/ball.md3 (made for radius 75).
*/
void SP_autoball_ball( gentity_t *ent ) {
	if ( !ent->model || !ent->model[0] )
		ent->model = AUTOBALL_MODEL;

	if ( !G_ParseScriptedObject( ent ) ) {
		G_FreeEntity( ent );
		return;
	}
	G_Autoball_ApplyDefaults( ent );
	/* map keys override the ball defaults and register the model */
	G_ApplyScriptedObjectMapProperties( ent );
	G_Autoball_ApplyScale( ent );
	G_Autoball_WarnLegacySolver();
	G_ScriptedObject_FinishSpawn( ent );
	VectorCopy( ent->s.pos.trBase, ent->ballHome );
}

gentity_t *G_Autoball_SpawnBall( const vec3_t origin ) {
	gentity_t *ent;
	vec3_t spawnOrigin;

	VectorCopy( origin, spawnOrigin );
	ent = G_Spawn();
	ent->classname = AUTOBALL_CLASSNAME;
	ent->model = AUTOBALL_MODEL;
	G_SetOrigin( ent, spawnOrigin );
	VectorCopy( spawnOrigin, ent->s.origin );

	if ( !G_ParseScriptedObject( ent ) ) {
		G_FreeEntity( ent );
		return NULL;
	}
	G_Autoball_ApplyDefaults( ent );
	ent->s.modelindex2 = G_ModelIndex( ent->model );
	G_Autoball_ApplyScale( ent );
	G_Autoball_WarnLegacySolver();
	G_ScriptedObject_FinishSpawn( ent );
	VectorCopy( ent->s.pos.trBase, ent->ballHome );
	return ent;
}

/* Puts the ball back on its home spot at rest (kick-off, debug reset). */
void G_Autoball_ResetBall( gentity_t *ball ) {
	if ( !ball || !ball->inuse )
		return;

	if ( G_RallyPhysics_Enabled() )
		trap_RallyPhysicsResetBody( ball->s.number, ball->ballHome, vec3_origin, vec3_origin );

	VectorCopy( ball->ballHome, ball->s.pos.trBase );
	VectorCopy( ball->ballHome, ball->s.origin );
	VectorCopy( ball->ballHome, ball->r.currentOrigin );
	VectorClear( ball->s.apos.trBase );
	VectorClear( ball->s.angles );
	VectorClear( ball->r.currentAngles );
	VectorClear( ball->s.pos.trDelta );
	VectorClear( ball->s.apos.trDelta );
	VectorClear( ball->angularMomentum );
	VectorClear( ball->lastNonZeroVelocity );
	ball->s.pos.trTime = level.time;
	ball->s.apos.trTime = level.time;
	ball->updateTime = level.time;
	ball->physicsAccumulatorMsec = 0;
	ball->physicsQuietSince = -1;
	ball->physicsSleeping = qfalse;
	G_Autoball_ClearTouches( ball );
	VectorClear( ball->ballPrevVelocity );
	ball->ballHitSoundTime = level.time + 500;
	ball->ballStillSince = 0;
	ball->ballSolidSince = 0;
	/* clients snap to the new spot instead of interpolating across the map */
	ball->s.eFlags ^= EF_TELEPORT_BIT;
	trap_LinkEntity( ball );
}

/*
Applies changed g_autoball* cvars to every ball, once per change.
*/
static void G_Autoball_MatchFrame( void );
static void G_Autoball_BounceSounds( void );

void G_Autoball_RunFrame( void ) {
	static int tuningStamp = -1;
	static int bodyStamp = -1;
	int newTuningStamp, newBodyStamp, count = 0;
	qboolean rebuild;
	gentity_t *ball = NULL;

	newBodyStamp = g_autoballMass.modificationCount + g_autoballElasticity.modificationCount;
	newTuningStamp = newBodyStamp + g_autoballImpactScale.modificationCount +
		g_autoballVerticalScale.modificationCount + g_autoballLift.modificationCount;
	G_Autoball_MatchFrame();
	G_Autoball_BounceSounds();
	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL )
		G_Autoball_BallGravity( ball );

	if ( tuningStamp < 0 ) {
		/* balls spawned this level already used the current values */
		tuningStamp = newTuningStamp;
		bodyStamp = newBodyStamp;
		return;
	}
	if ( newTuningStamp == tuningStamp )
		return;
	rebuild = ( newBodyStamp != bodyStamp ) ? qtrue : qfalse;
	tuningStamp = newTuningStamp;
	bodyStamp = newBodyStamp;

	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		G_Autoball_ApplyTuning( ball );
		if ( rebuild && G_RallyPhysics_Enabled() ) {
			vec3_t velocity;
			VectorCopy( ball->s.pos.trDelta, velocity );
			if ( G_RallyPhysics_CreateEntity( ball ) )
				trap_RallyPhysicsResetBody( ball->s.number, ball->r.currentOrigin,
					ball->r.currentAngles, velocity );
		}
		count++;
	}
	if ( count ) {
		G_Printf( "autoball: tuning applied to %d ball(s): impact %.2f, vertical %.2f, lift %.2f, mass %d, elasticity %.2f\n",
			count, Com_Clamp( 0.0f, 2.0f, g_autoballImpactScale.value ),
			Com_Clamp( 0.0f, 1.0f, g_autoballVerticalScale.value ),
			Com_Clamp( 0.0f, 0.9f, g_autoballLift.value ),
			(int)Com_Clamp( 1.0f, 100000.0f, g_autoballMass.value ),
			Com_Clamp( 0.0f, 1.0f, g_autoballElasticity.value ) );
	}
}

/* G_FreeEntity does not know about Bullet, so the body must go first. */
void G_Autoball_RemoveBall( gentity_t *ball ) {
	if ( !ball || !ball->inuse )
		return;
	if ( G_RallyPhysics_Enabled() )
		trap_RallyPhysicsRemoveBody( ball->s.number );
	G_FreeEntity( ball );
}

static int G_Autoball_CountBalls( void ) {
	gentity_t *ball = NULL;
	int count = 0;

	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL )
		count++;
	return count;
}

/*
==================
Cmd_BallSpawn_f

Drops a test ball AUTOBALL_SPAWN_DISTANCE units ahead of the player's car.
==================
*/
void Cmd_BallSpawn_f( gentity_t *ent ) {
	vec3_t angles, forward, start, end, mins, maxs;
	trace_t trace;
	gentity_t *ball;
	int clientNum;

	if ( !ent || !ent->client )
		return;
	clientNum = ent - g_entities;

	if ( G_Autoball_CountBalls() >= AUTOBALL_MAX_TEST_BALLS ) {
		trap_SendServerCommand( clientNum, va( "print \"There are already %d test balls; use ball_remove first.\n\"",
			AUTOBALL_MAX_TEST_BALLS ) );
		return;
	}

	VectorSet( angles, 0.0f, ent->client->ps.viewangles[YAW], 0.0f );
	AngleVectors( angles, forward, NULL, NULL );

	/* Lift the start above the car so the ball box does not begin inside the
	 * floor, then sweep forward and stop short of any wall in the way. */
	VectorSet( mins, -AUTOBALL_DEFAULT_RADIUS, -AUTOBALL_DEFAULT_RADIUS, -AUTOBALL_DEFAULT_RADIUS );
	VectorSet( maxs, AUTOBALL_DEFAULT_RADIUS, AUTOBALL_DEFAULT_RADIUS, AUTOBALL_DEFAULT_RADIUS );
	VectorCopy( ent->client->ps.origin, start );
	start[2] += AUTOBALL_DEFAULT_RADIUS + 16.0f;
	VectorMA( start, AUTOBALL_SPAWN_DISTANCE, forward, end );
	trap_Trace( &trace, start, mins, maxs, end, clientNum, MASK_PLAYERSOLID );
	if ( trace.startsolid || trace.allsolid ) {
		trap_SendServerCommand( clientNum, "print \"No room for a ball here; drive into the open and try again.\n\"" );
		return;
	}

	ball = G_Autoball_SpawnBall( trace.endpos );
	if ( !ball ) {
		trap_SendServerCommand( clientNum, "print \"ball_spawn: could not create the ball.\n\"" );
		return;
	}
	trap_SendServerCommand( clientNum, va( "print \"Ball %d spawned (%s). ball_reset puts it back here, ball_remove deletes all test balls.\n\"",
		ball->s.number, G_RallyPhysics_Enabled() ? "Bullet sphere" : "legacy solver" ) );
}

void Cmd_BallReset_f( gentity_t *ent ) {
	gentity_t *ball = NULL;
	int count = 0;

	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		G_Autoball_ResetBall( ball );
		count++;
	}
	if ( ent && ent->client )
		trap_SendServerCommand( ent - g_entities, va( "print \"%d ball(s) reset.\n\"", count ) );
	else
		G_Printf( "%d ball(s) reset.\n", count );
}

void Cmd_BallRemove_f( gentity_t *ent ) {
	gentity_t *ball = NULL;
	int count = 0;

	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		G_Autoball_RemoveBall( ball );
		count++;
	}
	if ( ent && ent->client )
		trap_SendServerCommand( ent - g_entities, va( "print \"%d ball(s) removed.\n\"", count ) );
	else
		G_Printf( "%d ball(s) removed.\n", count );
}

/*
==================
Server console / rcon commands (no cheat protection; the admin owns these)

  ball_spawn_at <x> <y> <z>     spawn a test ball at a world position
  ball_kick <vx> <vy> <vz>      give every ball this velocity (units/s)
  ball_info                     position, speed and state of every ball
  ball_reset / ball_remove      as the client commands
==================
*/
static qboolean G_Autoball_ReadVector( int firstArg, vec3_t out ) {
	char buffer[MAX_TOKEN_CHARS];
	int i;

	if ( trap_Argc() < firstArg + 3 )
		return qfalse;
	for ( i = 0; i < 3; i++ ) {
		trap_Argv( firstArg + i, buffer, sizeof( buffer ) );
		out[i] = atof( buffer );
	}
	return qtrue;
}

void Svcmd_BallSpawnAt_f( void ) {
	vec3_t origin;
	gentity_t *ball;

	if ( !G_Autoball_ReadVector( 1, origin ) ) {
		G_Printf( "usage: ball_spawn_at <x> <y> <z>\n" );
		return;
	}
	if ( G_Autoball_CountBalls() >= AUTOBALL_MAX_TEST_BALLS ) {
		G_Printf( "There are already %d test balls; use ball_remove first.\n", AUTOBALL_MAX_TEST_BALLS );
		return;
	}
	ball = G_Autoball_SpawnBall( origin );
	if ( ball )
		G_Printf( "Ball %d spawned at (%.0f %.0f %.0f).\n", ball->s.number,
			ball->s.pos.trBase[0], ball->s.pos.trBase[1], ball->s.pos.trBase[2] );
}

void Svcmd_BallKick_f( void ) {
	vec3_t velocity;
	gentity_t *ball = NULL;
	int count = 0;

	if ( !G_Autoball_ReadVector( 1, velocity ) ) {
		G_Printf( "usage: ball_kick <vx> <vy> <vz>  (units/s; %.2f units = 1 m)\n", CP_M_2_QU );
		return;
	}
	if ( !G_RallyPhysics_Enabled() ) {
		G_Printf( "ball_kick needs the Bullet backend (g_scriptedObjectBullet 1).\n" );
		return;
	}
	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		trap_RallyPhysicsResetBody( ball->s.number, ball->r.currentOrigin,
			ball->r.currentAngles, velocity );
		count++;
	}
	G_Printf( "%d ball(s) kicked with %.0f km/h.\n", count,
		VectorLength( velocity ) / CP_M_2_QU * 3.6f );
}

/* ball_turbo <client> <ms>: lights that car's turbo (for demolition tests) */
void Svcmd_BallTurbo_f( void ) {
	char buffer[MAX_TOKEN_CHARS];
	gentity_t *ent;
	int clientNum, ms;

	if ( trap_Argc() < 3 ) {
		G_Printf( "usage: ball_turbo <clientnum> <ms>\n" );
		return;
	}
	trap_Argv( 1, buffer, sizeof( buffer ) );
	clientNum = atoi( buffer );
	trap_Argv( 2, buffer, sizeof( buffer ) );
	ms = atoi( buffer );
	if ( clientNum < 0 || clientNum >= level.maxclients || ms <= 0 )
		return;
	ent = &g_entities[clientNum];
	if ( !ent->inuse || !ent->client )
		return;
	ent->client->ps.powerups[PW_TURBO] = level.time + ms;
	G_Printf( "ball_turbo: %s burns turbo for %i ms\n", ent->client->pers.netname, ms );
}

/* ball_touch <client>: counts as a touch of the match ball by that client
   (for testing shot/save/assist without driving) */
void Svcmd_BallTouch_f( void ) {
	char buffer[MAX_TOKEN_CHARS];
	gentity_t *ball, *ent;
	int clientNum;

	if ( trap_Argc() < 2 ) {
		G_Printf( "usage: ball_touch <clientnum>\n" );
		return;
	}
	trap_Argv( 1, buffer, sizeof( buffer ) );
	clientNum = atoi( buffer );
	ball = G_Autoball_MainBall();
	if ( !ball || clientNum < 0 || clientNum >= level.maxclients ) {
		G_Printf( "ball_touch: no ball or bad client\n" );
		return;
	}
	ent = &g_entities[clientNum];
	if ( !ent->inuse || !ent->client ) {
		G_Printf( "ball_touch: client %i not in game\n", clientNum );
		return;
	}
	G_Autoball_BallTouched( ball, ent );
	G_Printf( "ball_touch: %s touched the ball\n", ent->client->pers.netname );
}

void Svcmd_BallInfo_f( void ) {
	gentity_t *ball = NULL;
	int count = 0;

	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		G_Printf( "ball %d: pos (%.1f %.1f %.1f)  speed %.1f km/h (%.0f u/s)  spin %.0f deg/s  %s%s\n",
			ball->s.number, ball->r.currentOrigin[0], ball->r.currentOrigin[1],
			ball->r.currentOrigin[2],
			VectorLength( ball->s.pos.trDelta ) / CP_M_2_QU * 3.6f,
			VectorLength( ball->s.pos.trDelta ), VectorLength( ball->s.apos.trDelta ),
			ball->physicsSleeping ? "sleeping" : "awake",
			G_RallyPhysics_Enabled() ? "" : " (legacy solver)" );
		count++;
	}
	if ( !count )
		G_Printf( "No balls. Use ball_spawn (client, cheats) or ball_spawn_at <x> <y> <z>.\n" );
	if ( g_autoballDebug.integer ) {
		int i;
		for ( i = 0; i < level.maxclients; i++ ) {
			gentity_t *ent = &g_entities[i];
			if ( !ent->inuse || !ent->client || ent->client->pers.connected != CON_CONNECTED )
				continue;
			G_Printf( "car %i %s: pos (%.0f %.0f %.0f) speed %.0f km/h\n", i, ent->client->pers.netname,
				ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2],
				VectorLength( ent->client->ps.velocity ) / CP_M_2_QU * 3.6f );
		}
	}
	if ( g_gametype.integer == GT_AUTOBALL ) {
		static const char *stateNames[] = { "waiting", "kick-off", "live", "goal" };
		int state = level.autoballState;
		G_Printf( "match: %s, red %i : %i blue, ball %i, kick-off ends in %i ms\n",
			( state >= 0 && state <= AUTOBALL_STATE_GOAL ) ? stateNames[state] : "?",
			level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE], level.autoballBallNum,
			level.autoballState == AUTOBALL_STATE_KICKOFF ? level.autoballKickoffEnd - level.time : 0 );
	}
}


/*
===========================================================================

Match flow (GT_AUTOBALL)

  WAITING  -> KICKOFF   warmup over, a ball exists and somebody plays
  KICKOFF  -> LIVE      countdown done; cars were frozen on team_CTF_*player
  LIVE     -> GOAL      ball centre inside an autoball_goal volume
  GOAL     -> KICKOFF   after g_autoballGoalDelay seconds

Timelimit, capturelimit and sudden death (golden goal) come from the regular
team-game rules in CheckExitRules; G_Autoball_HoldMatchEnd only delays the
end while the ball is in the air or a goal is being celebrated.
===========================================================================
*/

#define AUTOBALL_GOAL_CLASSNAME		"autoball_goal"
#define AUTOBALL_GOAL_POINTS		100		/* personal score for a goal */
#define AUTOBALL_SCORER_WINDOW		10000	/* a touch older than this scores no one */
#define AUTOBALL_AIRBORNE_HEIGHT	40.0f	/* above its rest height the ball is "in play" */
#define AUTOBALL_HOLD_MAX			10000	/* ms the time limit waits for a ball in the air */
#define AUTOBALL_RESCUE_DROP		1500.0f	/* this far below its spot the ball left the map */
#define AUTOBALL_RESCUE_FAR			20000.0f	/* ... or this far away in any direction */
#define AUTOBALL_ASSIST_POINTS		50
#define AUTOBALL_SAVE_POINTS		50
#define AUTOBALL_SHOT_POINTS		20
#define AUTOBALL_TOUCH_POINTS		2
#define AUTOBALL_ASSIST_WINDOW		5000	/* teammate touch this close before the goal assists */
#define AUTOBALL_PREDICT_MSEC		3000	/* "heading for a goal" looks this far ahead */
#define AUTOBALL_PREDICT_STEP		0.05f
#define AUTOBALL_TOUCH_POINT_DELAY	2000	/* touch points at most this often per player */
#define AUTOBALL_SHOT_DELAY			2000
#define AUTOBALL_SAVE_DELAY			1000
#define AUTOBALL_DEMO_TURBO_BONUS	1000	/* ms of turbo for a demolition */
#define AUTOBALL_DEMO_RESPAWN		3000
#define AUTOBALL_GOAL_PUSH_RADIUS	1200.0f	/* goal explosion pushes cars within this */

static gentity_t *G_Autoball_GoalContaining( const vec3_t point );

static int G_Autoball_OpposingTeam( int team ) {
	return team == TEAM_RED ? TEAM_BLUE : TEAM_RED;
}

static gentity_t *G_Autoball_MainBall( void ) {
	return G_Find( NULL, FOFS( classname ), AUTOBALL_CLASSNAME );
}

/* centre of the goal volume defended by team; qfalse if there is none */
static qboolean G_Autoball_GoalCentre( int team, vec3_t out ) {
	gentity_t *goal = NULL;

	while ( ( goal = G_Find( goal, FOFS( classname ), AUTOBALL_GOAL_CLASSNAME ) ) != NULL ) {
		if ( goal->count != team )
			continue;
		VectorAdd( goal->r.absmin, goal->r.absmax, out );
		VectorScale( out, 0.5f, out );
		return qtrue;
	}
	VectorClear( out );
	return qfalse;
}

/*
CS_AUTOBALLSTATUS: "state kickoffEnd ball goalTeam scorer kmh  rx ry rz  bx by bz  introEnd"
rx..bz are the red and blue goal centres (TV camera, goal lights), all zero
when the map has no goals. introEnd: server time the kick-off camera flight
ends, 0 when this kick-off has none.
*/
static void G_Autoball_Publish( void ) {
	vec3_t red, blue;

	if ( !G_Autoball_GoalCentre( TEAM_RED, red ) || !G_Autoball_GoalCentre( TEAM_BLUE, blue ) ) {
		VectorClear( red );
		VectorClear( blue );
	}
	trap_SetConfigstring( CS_AUTOBALLSTATUS, va( "%i %i %i %i %i %i %i %i %i %i %i %i %i",
		level.autoballState, level.autoballKickoffEnd, level.autoballBallNum,
		level.autoballGoalTeam, level.autoballScorer, level.autoballGoalSpeed,
		(int)red[0], (int)red[1], (int)red[2], (int)blue[0], (int)blue[1], (int)blue[2],
		level.autoballIntroEnd ) );
	level.autoballPublished = level.autoballBallNum;
}

static void G_Autoball_SetState( int state ) {
	level.autoballState = state;
	G_Autoball_Publish();
}

/*
QUAKED autoball_goal (1 .5 0) ?
Goal volume, one per team. "team" is the team that DEFENDS this goal
("red" or "blue"); when the ball's centre enters it, the other team scores.
Start the brush one ball radius behind the goal line so the whole ball must
be over the line.
*/
void SP_autoball_goal( gentity_t *ent ) {
	char *team;

	G_SpawnString( "team", "red", &team );
	ent->count = ( !Q_stricmp( team, "blue" ) ) ? TEAM_BLUE : TEAM_RED;
	trap_SetBrushModel( ent, ent->model );
	ent->r.contents = 0;			/* the ball is tested by hand each frame */
	ent->r.svFlags = SVF_NOCLIENT;
	trap_LinkEntity( ent );
}

/* ball_goal_add <red|blue> <x1> <y1> <z1> <x2> <y2> <z2>: goal box on any map */
void Svcmd_BallGoalAdd_f( void ) {
	char buffer[MAX_TOKEN_CHARS];
	vec3_t a, b;
	gentity_t *goal;
	int i;

	if ( trap_Argc() < 8 ) {
		G_Printf( "usage: ball_goal_add <red|blue> <x1> <y1> <z1> <x2> <y2> <z2>  (team = defender)\n" );
		return;
	}
	trap_Argv( 1, buffer, sizeof( buffer ) );
	goal = G_Spawn();
	goal->classname = AUTOBALL_GOAL_CLASSNAME;
	goal->count = ( !Q_stricmp( buffer, "blue" ) ) ? TEAM_BLUE : TEAM_RED;
	for ( i = 0; i < 3; i++ ) {
		trap_Argv( 2 + i, buffer, sizeof( buffer ) );
		a[i] = atof( buffer );
		trap_Argv( 5 + i, buffer, sizeof( buffer ) );
		b[i] = atof( buffer );
		goal->r.absmin[i] = a[i] < b[i] ? a[i] : b[i];
		goal->r.absmax[i] = a[i] < b[i] ? b[i] : a[i];
	}
	/* not linked: only its bounds matter, and linking would recompute them */
	if ( g_gametype.integer == GT_AUTOBALL )
		G_Autoball_Publish();	/* clients learn the new goal position (CS shared with KOTH) */
	G_Printf( "%s goal %d added (%.0f %.0f %.0f) - (%.0f %.0f %.0f)\n",
		goal->count == TEAM_BLUE ? "Blue" : "Red", goal->s.number,
		goal->r.absmin[0], goal->r.absmin[1], goal->r.absmin[2],
		goal->r.absmax[0], goal->r.absmax[1], goal->r.absmax[2] );
}

/*
Mutator g_autoballBalls: extra match balls beside the centre spot, on the
line across the field so both teams have the same distance to them.
*/
static void G_Autoball_SpawnExtraBalls( void ) {
	gentity_t *main, *goal = NULL, *extra;
	vec3_t redGoal, blueGoal, axis, across, spot, mins, maxs;
	int count, i, haveRed = 0, haveBlue = 0;
	float side, radius;
	trace_t trace;

	count = g_autoballBalls.integer;
	if ( count > AUTOBALL_MAX_MATCH_BALLS )
		count = AUTOBALL_MAX_MATCH_BALLS;
	main = G_Autoball_MainBall();
	if ( count <= 1 || !main )
		return;
	VectorClear( redGoal );
	VectorClear( blueGoal );

	while ( ( goal = G_Find( goal, FOFS( classname ), AUTOBALL_GOAL_CLASSNAME ) ) != NULL ) {
		vec3_t centre;
		VectorAdd( goal->r.absmin, goal->r.absmax, centre );
		VectorScale( centre, 0.5f, centre );
		if ( goal->count == TEAM_BLUE ) {
			VectorCopy( centre, blueGoal );
			haveBlue = 1;
		} else {
			VectorCopy( centre, redGoal );
			haveRed = 1;
		}
	}
	if ( haveRed && haveBlue ) {
		VectorSubtract( blueGoal, redGoal, axis );
		axis[2] = 0.0f;
		VectorNormalize( axis );
	} else {
		VectorSet( axis, 1.0f, 0.0f, 0.0f );
	}
	VectorSet( across, -axis[1], axis[0], 0.0f );

	radius = main->ballRadius > 0.0f ? main->ballRadius : AUTOBALL_DEFAULT_RADIUS;
	VectorSet( mins, -radius, -radius, -radius );
	VectorSet( maxs, radius, radius, radius );
	for ( i = 1; i < count; i++ ) {
		/* 2 balls: one beside the centre; 3 balls: one on each side */
		side = ( i & 1 ) ? 1.0f : -1.0f;
		VectorMA( main->ballHome, side * ( AUTOBALL_EXTRA_BALL_SPACING + radius ), across, spot );
		trap_Trace( &trace, main->ballHome, mins, maxs, spot, main->s.number, MASK_SOLID );
		if ( trace.fraction < 1.0f )
			VectorCopy( trace.endpos, spot );
		/* SpawnBall applies the size mutator again; undo the lift it adds */
		spot[2] -= radius - radius / G_Autoball_BallScale();
		extra = G_Autoball_SpawnBall( spot );
		if ( !extra )
			break;
	}
	G_Printf( "autoball: %d match balls\n", G_Autoball_CountBalls() );
}

void G_Autoball_InitGame( void ) {
	gentity_t *goal = NULL;
	int red = 0, blue = 0;

	level.autoballState = AUTOBALL_STATE_WAITING;
	level.autoballKickoffEnd = 0;
	level.autoballStateEnd = 0;
	level.autoballCountdown = -1;
	level.autoballBallNum = -1;
	level.autoballGoalTeam = TEAM_FREE;
	level.autoballScorer = -1;
	level.autoballGoalSpeed = 0;
	level.autoballPublished = -2;

	if ( g_gametype.integer != GT_AUTOBALL )
		return;

	while ( ( goal = G_Find( goal, FOFS( classname ), AUTOBALL_GOAL_CLASSNAME ) ) != NULL ) {
		if ( goal->count == TEAM_BLUE )
			blue++;
		else
			red++;
	}
	G_Autoball_SpawnExtraBalls();
	if ( G_Autoball_BallScale() != 1.0f || g_autoballBallGravity.value != 1.0f || g_autoballBalls.integer > 1 )
		G_Printf( "autoball mutators: %d ball(s), size %.2fx, ball gravity %.2fx\n",
			G_Autoball_CountBalls(), G_Autoball_BallScale(),
			Com_Clamp( 0.1f, 2.0f, g_autoballBallGravity.value ) );
	if ( !G_Autoball_MainBall() || !red || !blue ) {
		G_Printf( S_COLOR_YELLOW "AUTOBALL: this map has %s and %d red / %d blue autoball_goal. "
			"Use ball_spawn_at and ball_goal_add to test anyway.\n",
			G_Autoball_MainBall() ? "a ball" : "no autoball_ball", red, blue );
	}
	G_Autoball_Publish();
}

/*
================
G_Autoball_PredictGoal

Where is the ball going? Flies it ballistically for AUTOBALL_PREDICT_MSEC with
simple floor bounces and stops at map geometry. Returns the team that DEFENDS
the goal it would enter, or TEAM_FREE. Cars and other balls are ignored.
================
*/
static int G_Autoball_PredictGoal( gentity_t *ball, const vec3_t origin, const vec3_t velocity ) {
	vec3_t pos, vel, next;
	trace_t trace;
	gentity_t *goal;
	float t, restZ;

	VectorCopy( origin, pos );
	VectorCopy( velocity, vel );
	restZ = ball->ballHome[2];
	for ( t = 0.0f; t < AUTOBALL_PREDICT_MSEC * 0.001f; t += AUTOBALL_PREDICT_STEP ) {
		vel[2] -= g_gravity.value * Com_Clamp( 0.1f, 2.0f, g_autoballBallGravity.value ) * AUTOBALL_PREDICT_STEP;
		VectorMA( pos, AUTOBALL_PREDICT_STEP, vel, next );
		if ( next[2] < restZ ) {
			next[2] = restZ;
			if ( vel[2] < 0.0f )
				vel[2] = -vel[2] * ball->elasticity;
		}
		trap_Trace( &trace, pos, NULL, NULL, next, ball->s.number, MASK_SOLID );
		if ( trace.fraction < 1.0f || trace.startsolid )
			return TEAM_FREE;		/* a wall, post or ramp is in the way */
		goal = G_Autoball_GoalContaining( next );
		if ( goal )
			return goal->count;
		VectorCopy( next, pos );
	}
	return TEAM_FREE;
}

void G_Autoball_BallTouched( gentity_t *ball, gentity_t *other ) {
	int i, client;

	if ( !ball || !other || !other->client )
		return;
	if ( Q_stricmp( ball->classname, AUTOBALL_CLASSNAME ) )
		return;
	client = other->s.number;
	ball->ballLastToucher = client;
	ball->ballLastTouchTime = level.time;
	/* clients colour the ball's seams with the last toucher's team */
	ball->s.generic1 &= ~SCRIPTED_GENERIC1_TEAM_MASK;
	if ( other->client->sess.sessionTeam == TEAM_RED || other->client->sess.sessionTeam == TEAM_BLUE )
		ball->s.generic1 |= ( other->client->sess.sessionTeam << SCRIPTED_GENERIC1_TEAM_SHIFT ) &
			SCRIPTED_GENERIC1_TEAM_MASK;

	/* distinct touchers, newest first; a repeat touch only refreshes the time */
	if ( ball->ballTouchClient[0] != client ) {
		for ( i = 3; i > 0; i-- ) {
			ball->ballTouchClient[i] = ball->ballTouchClient[i - 1];
			ball->ballTouchTime[i] = ball->ballTouchTime[i - 1];
		}
		ball->ballTouchClient[0] = client;
	}
	ball->ballTouchTime[0] = level.time;

	/* Shot/save are judged next frame once Bullet has the new velocity; keep
	 * the threat from before the FIRST touch of a dribble. */
	if ( g_gametype.integer == GT_AUTOBALL && level.autoballState == AUTOBALL_STATE_LIVE &&
		ball->ballPendingClient != client ) {
		ball->ballPendingClient = client;
		ball->ballPendingThreat = G_Autoball_PredictGoal( ball, ball->r.currentOrigin, ball->s.pos.trDelta );
	}
	ball->ballPendingTime = level.time;
}

/* A client left or changed teams: drop it from every ball's touch history,
   so the slot's next owner or the other team gets no goal/assist credit. */
void G_Autoball_ForgetClient( int clientNum ) {
	gentity_t *ball = NULL;
	int i;

	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		if ( ball->ballLastToucher == clientNum )
			ball->ballLastToucher = -1;
		if ( ball->ballPendingClient == clientNum )
			ball->ballPendingClient = -1;
		for ( i = 0; i < 4; i++ ) {
			if ( ball->ballTouchClient[i] == clientNum )
				ball->ballTouchClient[i] = -1;
		}
	}
}

/* Contact sound, louder material for harder hits. */
void G_Autoball_BallHit( gentity_t *ball, gentity_t *other, const vec3_t impulse ) {
	float deltaV;

	if ( !ball || Q_stricmp( ball->classname, AUTOBALL_CLASSNAME ) || ball->mass <= 0 )
		return;
	if ( level.time < ball->ballHitSoundTime )
		return;
	deltaV = VectorLength( impulse ) / (float)ball->mass;
	if ( deltaV < 60.0f )
		return;
	G_AddEvent( ball, EV_GENERAL_SOUND, G_SoundIndex( deltaV > 700.0f ?
		AUTOBALL_SOUND_HIT_HARD : AUTOBALL_SOUND_HIT_SOFT ) );
	ball->ballHitSoundTime = level.time + 120;
}

/* Bullet bounces the ball off walls, floor and ceiling without telling the
   game, so a sudden velocity change without a car touch is a bounce. */
static void G_Autoball_BounceSounds( void ) {
	gentity_t *ball = NULL;
	vec3_t delta;
	float dv;

	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		VectorSubtract( ball->s.pos.trDelta, ball->ballPrevVelocity, delta );
		dv = VectorLength( delta );
		VectorCopy( ball->s.pos.trDelta, ball->ballPrevVelocity );
		if ( dv < AUTOBALL_BOUNCE_SOUND_DV || level.time < ball->ballHitSoundTime )
			continue;
		if ( ball->ballLastTouchTime && level.time - ball->ballLastTouchTime < 150 )
			continue;	/* a car hit, that has its own sound */
		G_AddEvent( ball, EV_GENERAL_SOUND, G_SoundIndex( AUTOBALL_SOUND_BOUNCE ) );
		ball->ballHitSoundTime = level.time + 150;
	}
}

static void G_Autoball_Award( gentity_t *ent, int points, int persistant ) {
	if ( !ent || !ent->client )
		return;
	AddScore( ent, ent->r.currentOrigin, points );
	if ( persistant >= 0 )
		ent->client->ps.persistant[persistant]++;	/* reward sound and medal on the client */
}

/* Judges the pending touch now that the ball flies with its new velocity. */
static void G_Autoball_JudgeTouch( gentity_t *ball ) {
	gentity_t *ent;
	gclient_t *client;
	int team, opponent, threat;

	if ( ball->ballPendingClient < 0 || level.time <= ball->ballPendingTime )
		return;
	ent = &g_entities[ball->ballPendingClient];
	ball->ballPendingClient = -1;
	if ( !ent->inuse || !ent->client || ent->client->pers.connected != CON_CONNECTED )
		return;
	client = ent->client;
	team = client->sess.sessionTeam;
	if ( team != TEAM_RED && team != TEAM_BLUE )
		return;
	opponent = G_Autoball_OpposingTeam( team );
	threat = G_Autoball_PredictGoal( ball, ball->r.currentOrigin, ball->s.pos.trDelta );
	if ( g_autoballDebug.integer ) {
		G_Printf( "autoball: touch by %s (%s): heading for %s goal before, %s goal after\n",
			client->pers.netname, TeamName( team ),
			ball->ballPendingThreat == TEAM_FREE ? "no" : TeamName( ball->ballPendingThreat ),
			threat == TEAM_FREE ? "no" : TeamName( threat ) );
	}

	G_LogPrintf( "AutoballTouch: %i %i %i %i\n", ent->s.number, team,
		(int)( VectorLength( client->ps.velocity ) / CP_M_2_QU * 3.6f ),
		(int)( VectorLength( ball->s.pos.trDelta ) / CP_M_2_QU * 3.6f ) );
	{
		int kmh = (int)( VectorLength( ball->s.pos.trDelta ) / CP_M_2_QU * 3.6f + 0.5f );
		if ( kmh > client->pers.autoballBestShot )
			client->pers.autoballBestShot = kmh;
	}

	if ( ball->ballPendingThreat == team && threat != team &&
		level.time - client->pers.autoballSaveTime >= AUTOBALL_SAVE_DELAY ) {
		client->pers.autoballSaveTime = level.time;
		client->pers.autoballSaves++;
		G_Autoball_Award( ent, AUTOBALL_SAVE_POINTS, PERS_DEFEND_COUNT );
		trap_SendServerCommand( -1, va( "print \"%s^7 saves!\n\"", client->pers.netname ) );
		G_LogPrintf( "AutoballSave: %i %i\n", ent->s.number, team );
		return;
	}
	if ( threat == opponent && ball->ballPendingThreat != opponent &&
		level.time - client->pers.autoballShotTime >= AUTOBALL_SHOT_DELAY ) {
		client->pers.autoballShotTime = level.time;
		client->pers.autoballShots++;
		G_Autoball_Award( ent, AUTOBALL_SHOT_POINTS, -1 );
		G_LogPrintf( "AutoballShot: %i %i\n", ent->s.number, team );
		return;
	}
	if ( level.time - client->pers.autoballTouchPointTime >= AUTOBALL_TOUCH_POINT_DELAY ) {
		client->pers.autoballTouchPointTime = level.time;
		G_Autoball_Award( ent, AUTOBALL_TOUCH_POINTS, -1 );
	}
}

static void G_Autoball_PushCar( gentity_t *ent, const vec3_t deltaV ) {
	car_t *car;
	int i;

	car = &ent->client->car;
	VectorAdd( car->sBody.v, deltaV, car->sBody.v );
	VectorAdd( car->tBody.v, deltaV, car->tBody.v );
	for ( i = 0; i < NUM_CAR_POINTS; i++ ) {
		VectorAdd( car->sPoints[i].v, deltaV, car->sPoints[i].v );
		VectorAdd( car->tPoints[i].v, deltaV, car->tPoints[i].v );
	}
	VectorAdd( ent->client->ps.velocity, deltaV, ent->client->ps.velocity );
}

/* The goal explosion shoves nearby cars away from the ball. */
static void G_Autoball_GoalPush( const vec3_t origin ) {
	int i;

	for ( i = 0; i < level.maxclients; i++ ) {
		gentity_t *ent = &g_entities[i];
		vec3_t dir;
		float dist, speed;

		if ( !ent->inuse || !ent->client || ent->client->ps.pm_type == PM_DEAD ||
			ent->client->sess.sessionTeam == TEAM_SPECTATOR )
			continue;
		VectorSubtract( ent->client->ps.origin, origin, dir );
		dist = VectorNormalize( dir );
		if ( dist > AUTOBALL_GOAL_PUSH_RADIUS ) {
			if ( g_autoballDebug.integer )
				G_Printf( "autoball: goal blast misses %s (%.0f units away)\n", ent->client->pers.netname, dist );
			continue;
		}
		/* square-root falloff: still a real shove halfway out */
		speed = g_autoballGoalPush.value * sqrt( 1.0f - dist / AUTOBALL_GOAL_PUSH_RADIUS );
		if ( speed <= 0.0f )
			continue;
		dir[2] += 0.5f;		/* lift: cars should fly, not slide */
		VectorNormalize( dir );
		VectorScale( dir, speed, dir );
		G_Autoball_PushCar( ent, dir );
		if ( g_autoballDebug.integer ) {
			G_Printf( "autoball: goal blast pushes %s (%.0f units away) with %.0f km/h, car now %.0f km/h\n",
				ent->client->pers.netname, dist, speed / CP_M_2_QU * 3.6f,
				VectorLength( ent->client->ps.velocity ) / CP_M_2_QU * 3.6f );
		}
	}
}

/* A rammer on turbo above g_autoballDemoSpeed destroys an opponent. */
void G_Autoball_VehicleContact( gentity_t *self, const vehicleCollisionContact_t *contact ) {
	gentity_t *other, *rammer, *victim;
	float minSpeed;
	int turbo;

	if ( g_gametype.integer != GT_AUTOBALL || level.autoballState != AUTOBALL_STATE_LIVE ||
		g_autoballDemoSpeed.value <= 0.0f || !self || !self->client || !contact ||
		!contact->valid || contact->otherEnt < 0 || contact->otherEnt >= MAX_CLIENTS ||
		contact->otherEnt == self->s.number )
		return;
	other = &g_entities[contact->otherEnt];
	if ( !other->inuse || !other->client ||
		other->client->sess.sessionTeam == self->client->sess.sessionTeam )
		return;

	minSpeed = g_autoballDemoSpeed.value * CP_M_2_QU / 3.6f;
	rammer = victim = NULL;
	if ( contact->selfWasRamming ) {
		rammer = self;
		victim = other;
	} else if ( contact->otherWasRamming ) {
		rammer = other;
		victim = self;
	}
	if ( !rammer || rammer->client->ps.pm_type == PM_DEAD || rammer->health <= 0 ||
		victim->client->ps.pm_type == PM_DEAD || victim->health <= 0 )
		return;
	if ( g_autoballDebug.integer && level.time >= level.autoballRamLogTime ) {
		level.autoballRamLogTime = level.time + 250;
		G_Printf( "autoball: %s rams %s at %.0f km/h, turbo %s (needs %.0f km/h + turbo)\n",
			rammer->client->pers.netname, victim->client->pers.netname,
			VectorLength( rammer->client->ps.velocity ) / CP_M_2_QU * 3.6f,
			rammer->client->ps.powerups[PW_TURBO] > level.time ? "on" : "off",
			g_autoballDemoSpeed.value );
	}
	if ( rammer->client->ps.powerups[PW_TURBO] <= level.time )
		return;		/* turbo must be burning */
	if ( VectorLength( rammer->client->ps.velocity ) < minSpeed )
		return;

	if ( level.intermissionQueued || level.intermissiontime )
		return;
	G_Damage( victim, rammer, rammer, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_AUTOBALL_DEMOLITION );
	if ( victim->health > 0 )
		return;		/* nothing happened (god mode, battle suit with weapons on) */
	rammer->client->pers.autoballDemos++;
	G_LogPrintf( "AutoballDemo: %i %i %i\n", rammer->s.number, victim->s.number,
		(int)( VectorLength( rammer->client->ps.velocity ) / CP_M_2_QU * 3.6f ) );

	/* a second of extra turbo for the demolition, capped like a pickup */
	turbo = rammer->client->ps.powerups[PW_TURBO] - level.time + AUTOBALL_DEMO_TURBO_BONUS;
	if ( turbo > RALLY_TURBO_MAX_MSEC )
		turbo = RALLY_TURBO_MAX_MSEC;
	rammer->client->ps.powerups[PW_TURBO] = level.time + turbo;
}

/* Collisions and stray weapon fire do not hurt cars in Autoball. */
qboolean G_Autoball_BlockDamage( gentity_t *targ, int mod ) {
	if ( g_gametype.integer != GT_AUTOBALL || !targ || !targ->client )
		return qfalse;
	switch ( mod ) {
	case MOD_AUTOBALL_DEMOLITION:
	case MOD_TRIGGER_HURT:
	case MOD_LAVA:
	case MOD_SLIME:
	case MOD_WATER:
	case MOD_CRUSH:
	case MOD_SUICIDE:
	case MOD_FALLING:
		return qfalse;
	case MOD_TELEFRAG:
		/* kick-off respawns put cars back while the others are still
		   linked; a car on its own spawn spot would telefrag itself */
		return qtrue;
	default:
		return g_autoballWeapons.integer ? qfalse : qtrue;
	}
}

qboolean G_Autoball_ForceRespawn( gentity_t *ent ) {
	if ( g_gametype.integer != GT_AUTOBALL || !ent || !ent->client )
		return qfalse;
	return ( level.time >= ent->client->respawnTime - 1700 + AUTOBALL_DEMO_RESPAWN ) ? qtrue : qfalse;
}

void G_Autoball_ClientSpawn( gentity_t *ent ) {
	gclient_t *client;
	int turbo;

	if ( g_gametype.integer != GT_AUTOBALL || !ent || !ent->client )
		return;
	client = ent->client;

	/* Cars go to every client, like the ball. The server picks what a client
	   sees from its real position; a free spectator watching through the TV
	   camera (cg_autoball.c) may float where the field is not visible and
	   would see the ball without cars. An arena has few cars, so it's cheap. */
	if ( client->sess.sessionTeam == TEAM_RED || client->sess.sessionTeam == TEAM_BLUE )
		ent->r.svFlags |= SVF_BROADCAST;
	else
		ent->r.svFlags &= ~SVF_BROADCAST;

	if ( !g_autoballWeapons.integer ) {
		client->ps.stats[STAT_WEAPONS] = 0;
		client->ps.weapon = WP_NONE;
	}
	/* a negative value is stored turbo that BUTTON_TURBO releases */
	turbo = g_autoballStartTurbo.integer;
	if ( turbo < 0 )
		turbo = 0;
	if ( turbo > RALLY_TURBO_MAX_MSEC )
		turbo = RALLY_TURBO_MAX_MSEC;
	client->ps.powerups[PW_TURBO] = turbo ? -turbo : 0;
}

qboolean G_Autoball_CarsFrozen( int serverTime ) {
	return ( level.autoballState == AUTOBALL_STATE_KICKOFF &&
		serverTime < level.autoballKickoffEnd ) ? qtrue : qfalse;
}

/* Without g_autoballWeapons only turbo pickups stay in the arena. */
qboolean G_Autoball_ItemDisabled( gitem_t *item ) {
	if ( g_gametype.integer != GT_AUTOBALL || g_autoballWeapons.integer || !item )
		return qfalse;
	if ( item->giType == IT_POWERUP && item->giTag == PW_TURBO )
		return qfalse;
	if ( item->giType == IT_HOLDABLE && item->giTag == HI_TURBO )
		return qfalse;
	return qtrue;
}

qboolean G_Autoball_HoldMatchEnd( void ) {
	gentity_t *ball;

	if ( level.autoballState == AUTOBALL_STATE_GOAL )
		return qtrue;
	if ( level.autoballState != AUTOBALL_STATE_LIVE )
		return qfalse;
	/* never longer than AUTOBALL_HOLD_MAX past the time limit, e.g. when a
	   ball lies on top of a goal where no car can reach it */
	if ( g_timelimit.integer &&
		level.time - level.startTime > g_timelimit.integer * 60000 + AUTOBALL_HOLD_MAX )
		return qfalse;
	ball = NULL;
	while ( ( ball = G_Find( ball, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		if ( ball->r.currentOrigin[2] > ball->ballHome[2] + AUTOBALL_AIRBORNE_HEIGHT &&
			VectorLength( ball->s.pos.trDelta ) > 40.0f )
			return qtrue;	/* in the air and moving */
	}
	return qfalse;
}

static void G_Autoball_StartKickoff( gentity_t *ball ) {
	int i, delay;
	gentity_t *other = NULL;

	if ( ball )
		G_Autoball_ResetBall( ball );
	/* extra match balls (g_autoballBalls) go back to their spots too */
	while ( ( other = G_Find( other, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
		if ( other != ball )
			G_Autoball_ResetBall( other );
	}

	/* everybody back to a kick-off spot (team_CTF_redplayer / blueplayer).
	   Unlink all cars first, so the spawn spot search doesn't see them at
	   their old positions. */
	for ( i = 0; i < level.maxclients; i++ ) {
		gentity_t *ent = &g_entities[i];
		if ( ent->inuse && ent->client && ent->client->pers.connected == CON_CONNECTED &&
			( ent->client->sess.sessionTeam == TEAM_RED || ent->client->sess.sessionTeam == TEAM_BLUE ) )
			trap_UnlinkEntity( ent );
	}
	for ( i = 0; i < level.maxclients; i++ ) {
		gentity_t *ent = &g_entities[i];
		gclient_t *client = ent->client;

		if ( !ent->inuse || !client || client->pers.connected != CON_CONNECTED )
			continue;
		if ( client->sess.sessionTeam != TEAM_RED && client->sess.sessionTeam != TEAM_BLUE )
			continue;
		client->pers.teamState.state = TEAM_BEGIN;
		ClientSpawn( ent );
	}

	delay = g_autoballKickoffDelay.integer;
	if ( delay < 0 )
		delay = 0;
	if ( delay > 10 )
		delay = 10;
	level.autoballKickoffEnd = level.time + delay * 1000;
	/* the first kick-off of a match waits for the clients' camera flight
	   over the arena (cg_autoball.c); they detect it from the longer wait */
	level.autoballIntroEnd = 0;
	if ( !level.autoballIntroDone ) {
		level.autoballIntroDone = 1;
		if ( g_autoballIntro.integer ) {
			level.autoballKickoffEnd += AUTOBALL_INTRO_MSEC;
			level.autoballIntroEnd = level.time + AUTOBALL_INTRO_MSEC;
		}
		/* the clients' match clock starts at the first whistle; level.startTime
		   itself (time limit) follows in G_Autoball_Countdown */
		trap_SetConfigstring( CS_LEVEL_START_TIME, va( "%i", level.autoballKickoffEnd ) );
	}
	level.autoballCountdown = -1;
	G_Autoball_SetState( AUTOBALL_STATE_KICKOFF );
	G_LogPrintf( "AutoballKickoff: %i %i\n", level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
}

static void G_Autoball_Countdown( gentity_t *ball ) {
	int secondsLeft;
	static char *countSounds[4] = { NULL, "sound/rally/race/one.ogg",
		"sound/rally/race/two.ogg", "sound/rally/race/three.ogg" };

	if ( level.time >= level.autoballKickoffEnd ) {
		trap_SendServerCommand( -1, "rc \"GO!\" 0" );
		if ( ball )
			Rally_Sound( ball, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex( AUTOBALL_SOUND_WHISTLE ) );
		/* the match (and its time limit) starts with the first whistle,
		   not with the map load, the intro flight or the countdown */
		if ( !level.autoballClockStarted ) {
			level.autoballClockStarted = 1;
			level.startTime = level.time;
			trap_SetConfigstring( CS_LEVEL_START_TIME, va( "%i", level.startTime ) );
		}
		G_Autoball_SetState( AUTOBALL_STATE_LIVE );
		G_LogPrintf( "AutoballLive:\n" );
		return;
	}
	secondsLeft = ( level.autoballKickoffEnd - level.time + 999 ) / 1000;
	if ( secondsLeft == level.autoballCountdown || secondsLeft > 3 )
		return;
	level.autoballCountdown = secondsLeft;
	trap_SendServerCommand( -1, va( "rc \"%i\" %i", secondsLeft, secondsLeft ) );
	if ( ball && secondsLeft >= 1 && secondsLeft <= 3 )
		Rally_Sound( ball, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex( countSounds[secondsLeft] ) );
}

static void G_Autoball_ScoreGoal( gentity_t *ball, gentity_t *goal ) {
	int scoringTeam, delay;
	gentity_t *scorer = NULL;
	gentity_t *te;
	qboolean ownGoal = qfalse;

	scoringTeam = G_Autoball_OpposingTeam( goal->count );
	if ( ball->ballLastToucher >= 0 && ball->ballLastToucher < level.maxclients &&
		level.time - ball->ballLastTouchTime <= AUTOBALL_SCORER_WINDOW ) {
		scorer = &g_entities[ball->ballLastToucher];
		if ( !scorer->inuse || !scorer->client ||
			scorer->client->pers.connected != CON_CONNECTED ) {
			scorer = NULL;
		} else if ( scorer->client->sess.sessionTeam != scoringTeam ) {
			ownGoal = qtrue;
		}
	}

	level.autoballGoalTeam = scoringTeam;
	/* scorer for the clients: client number, -2 own goal, -1 nobody credited */
	level.autoballScorer = ownGoal ? -2 : ( scorer ? scorer->s.number : -1 );
	level.autoballGoalSpeed = (int)( VectorLength( ball->s.pos.trDelta ) / CP_M_2_QU * 3.6f + 0.5f );

	AddTeamScore( ball->r.currentOrigin, scoringTeam, 1 );
	if ( scorer && !ownGoal ) {
		int i;

		scorer->client->pers.autoballGoals++;
		AddScore( scorer, ball->r.currentOrigin, AUTOBALL_GOAL_POINTS );
		/* newest teammate touch before the scorer's, within the window */
		for ( i = 0; i < 4; i++ ) {
			gentity_t *mate;
			int num = ball->ballTouchClient[i];

			if ( num < 0 || num == scorer->s.number )
				continue;
			if ( level.time - ball->ballTouchTime[i] > AUTOBALL_ASSIST_WINDOW )
				break;
			mate = &g_entities[num];
			if ( !mate->inuse || !mate->client || mate->client->pers.connected != CON_CONNECTED ||
				mate->client->sess.sessionTeam != scoringTeam )
				continue;
			mate->client->pers.autoballAssists++;
			G_Autoball_Award( mate, AUTOBALL_ASSIST_POINTS, PERS_ASSIST_COUNT );
			trap_SendServerCommand( -1, va( "print \"Assist: %s^7\n\"", mate->client->pers.netname ) );
			G_LogPrintf( "AutoballAssist: %i %i\n", num, scoringTeam );
			break;
		}
	}
	ball->ballPendingClient = -1;	/* the goal itself is the result of that touch */
	G_Autoball_GoalPush( ball->r.currentOrigin );
	Rally_Sound( ball, EV_GLOBAL_SOUND, CHAN_AUTO, G_SoundIndex( AUTOBALL_SOUND_CROWD ) );
	Rally_Sound( ball, EV_GLOBAL_SOUND, CHAN_ANNOUNCER, G_SoundIndex( AUTOBALL_SOUND_GOAL_HORN ) );
	CalculateRanks();

	te = G_TempEntity( ball->r.currentOrigin, EV_EXPLOSION );
	te->s.eventParm = EXPLOSION_PARM_AUTOBALL_GOAL | ( scoringTeam & 0x0F );
	te->r.svFlags |= SVF_BROADCAST;

	if ( scorer && !ownGoal ) {
		trap_SendServerCommand( -1, va( "print \"%s%s^7 scores: %s^7 (%i km/h)\n\"",
			TeamColorString( scoringTeam ), TeamName( scoringTeam ),
			scorer->client->pers.netname, level.autoballGoalSpeed ) );
	} else if ( scorer ) {
		trap_SendServerCommand( -1, va( "print \"%s%s^7 scores: own goal by %s^7 (%i km/h)\n\"",
			TeamColorString( scoringTeam ), TeamName( scoringTeam ),
			scorer->client->pers.netname, level.autoballGoalSpeed ) );
	} else {
		trap_SendServerCommand( -1, va( "print \"%s%s^7 scores (%i km/h)\n\"",
			TeamColorString( scoringTeam ), TeamName( scoringTeam ), level.autoballGoalSpeed ) );
	}
	G_LogPrintf( "AutoballGoal: %i %i %i %i: %s scores\n", scoringTeam,
		level.autoballScorer, ownGoal, level.autoballGoalSpeed, TeamName( scoringTeam ) );

	delay = g_autoballGoalDelay.integer;
	if ( delay < 1 )
		delay = 1;
	if ( delay > 15 )
		delay = 15;
	level.autoballStateEnd = level.time + delay * 1000;
	G_Autoball_SetState( AUTOBALL_STATE_GOAL );
}

static gentity_t *G_Autoball_GoalContaining( const vec3_t point ) {
	gentity_t *goal = NULL;

	while ( ( goal = G_Find( goal, FOFS( classname ), AUTOBALL_GOAL_CLASSNAME ) ) != NULL ) {
		if ( point[0] >= goal->r.absmin[0] && point[0] <= goal->r.absmax[0] &&
			point[1] >= goal->r.absmin[1] && point[1] <= goal->r.absmax[1] &&
			point[2] >= goal->r.absmin[2] && point[2] <= goal->r.absmax[2] )
			return goal;
	}
	return NULL;
}

/*
g_autoballStats 1: once a second a sample line for tools/autoball/analyze_log.py
  AutoballSample: <ball km/h> <ball height above its spot> <ball x> <ball y>
                  <red avg turbo ms> <blue avg turbo ms> <red avg km/h> <blue avg km/h>
*/
static void G_Autoball_StatsSample( gentity_t *ball ) {
	static int nextSample;
	int i, team, turbo[TEAM_NUM_TEAMS], cars[TEAM_NUM_TEAMS];
	float speed[TEAM_NUM_TEAMS];

	if ( !g_autoballStats.integer || !ball || level.autoballState != AUTOBALL_STATE_LIVE )
		return;
	if ( level.time < nextSample && nextSample - level.time <= 1000 )
		return;
	nextSample = level.time + 1000;
	memset( turbo, 0, sizeof( turbo ) );
	memset( cars, 0, sizeof( cars ) );
	memset( speed, 0, sizeof( speed ) );
	for ( i = 0; i < level.maxclients; i++ ) {
		gentity_t *ent = &g_entities[i];
		int t;
		if ( !ent->inuse || !ent->client || ent->client->pers.connected != CON_CONNECTED )
			continue;
		team = ent->client->sess.sessionTeam;
		if ( team != TEAM_RED && team != TEAM_BLUE )
			continue;
		t = ent->client->ps.powerups[PW_TURBO];
		turbo[team] += t > level.time ? t - level.time : ( t < 0 ? -t : 0 );
		speed[team] += VectorLength( ent->client->ps.velocity ) / CP_M_2_QU * 3.6f;
		cars[team]++;
	}
	G_LogPrintf( "AutoballSample: %i %i %i %i %i %i %i %i\n",
		(int)( VectorLength( ball->s.pos.trDelta ) / CP_M_2_QU * 3.6f ),
		(int)( ball->r.currentOrigin[2] - ball->ballHome[2] ),
		(int)ball->r.currentOrigin[0], (int)ball->r.currentOrigin[1],
		cars[TEAM_RED] ? turbo[TEAM_RED] / cars[TEAM_RED] : 0,
		cars[TEAM_BLUE] ? turbo[TEAM_BLUE] / cars[TEAM_BLUE] : 0,
		cars[TEAM_RED] ? (int)( speed[TEAM_RED] / cars[TEAM_RED] ) : 0,
		cars[TEAM_BLUE] ? (int)( speed[TEAM_BLUE] / cars[TEAM_BLUE] ) : 0 );
}

/* The first kick-off (with its camera flight) waits for players that are
   still loading the map, but not longer than 15 s. */
static qboolean G_Autoball_HumansLoading( void ) {
	int i;

	if ( level.autoballIntroDone || level.time - level.startTime > 15000 )
		return qfalse;
	for ( i = 0; i < level.maxclients; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTING &&
			!( g_entities[i].r.svFlags & SVF_BOT ) )
			return qtrue;
	}
	return qfalse;
}

/*
QUAKED autoball_reset (1 .5 0) ?
Ball rescue volume (brush entity, use common/trigger; never solid). A ball
whose centre enters it is put back (match ball: new kick-off). Use it for
places a ball can reach but no car can: roofs, gaps, outside the arena.
*/
void SP_autoball_reset( gentity_t *ent ) {
	trap_SetBrushModel( ent, ent->model );
	ent->r.contents = 0;			/* tested by hand, like the goals */
	ent->r.svFlags = SVF_NOCLIENT;
	trap_LinkEntity( ent );
}

static qboolean G_Autoball_InResetVolume( const vec3_t point ) {
	gentity_t *zone = NULL;

	while ( ( zone = G_Find( zone, FOFS( classname ), "autoball_reset" ) ) != NULL ) {
		if ( point[0] >= zone->r.absmin[0] && point[0] <= zone->r.absmax[0] &&
			point[1] >= zone->r.absmin[1] && point[1] <= zone->r.absmax[1] &&
			point[2] >= zone->r.absmin[2] && point[2] <= zone->r.absmax[2] )
			return qtrue;
	}
	return qfalse;
}

/*
Ball rescue: a ball that left the playable space comes back. Returns the
reason, or NULL when the ball is fine.
  - its centre is in an autoball_reset volume
  - it fell far below its kick-off spot (out of the map) or flew far away
  - its centre is stuck in solid geometry (for more than half a second)
  - no car touched it for g_autoballRescueTime seconds while it was away
    from its spot (on top of a goal, in a gap no car reaches; 0 = off)
*/
static const char *G_Autoball_RescueReason( gentity_t *ball ) {
	const float *org = ball->r.currentOrigin;
	vec3_t delta;
	int idle;

	if ( G_Autoball_InResetVolume( org ) )
		return "out of play";
	if ( org[2] < ball->ballHome[2] - AUTOBALL_RESCUE_DROP )
		return "fell out of the arena";
	VectorSubtract( org, ball->ballHome, delta );
	if ( VectorLength( delta ) > AUTOBALL_RESCUE_FAR )
		return "flew out of the arena";

	if ( trap_PointContents( org, ball->s.number ) & CONTENTS_SOLID ) {
		if ( !ball->ballSolidSince )
			ball->ballSolidSince = level.time;
		else if ( level.time - ball->ballSolidSince > 500 )
			return "stuck in the map";
	} else {
		ball->ballSolidSince = 0;
	}

	/* untouched: every car hit restarts the clock. Speed doesn't count, a
	   ball can roll slowly or bounce on a roof for a long time. A ball on
	   its own spot is not out of reach, a reset would not change anything. */
	if ( ( ball->ballLastTouchTime && level.time - ball->ballLastTouchTime < 1000 ) ||
		Distance( org, ball->ballHome ) < 2.0f * ( ball->ballRadius > 0.0f ? ball->ballRadius : AUTOBALL_DEFAULT_RADIUS ) ) {
		ball->ballStillSince = 0;
	} else if ( !ball->ballStillSince ) {
		ball->ballStillSince = level.time;
	}
	idle = g_autoballRescueTime.integer;
	if ( idle > 0 && ball->ballStillSince && level.time - ball->ballStillSince > idle * 1000 )
		return "out of reach";
	return NULL;
}

/* returns qtrue when the rescue restarted the match flow (match ball) */
static qboolean G_Autoball_RescueBall( gentity_t *ball, gentity_t *mainBall ) {
	const char *reason = G_Autoball_RescueReason( ball );

	if ( !reason )
		return qfalse;
	G_LogPrintf( "AutoballRescue: %i %s\n", ball->s.number, reason );
	if ( ball != mainBall && Q_stricmp( reason, "out of reach" ) &&
		ball->ballRescueTime && level.time - ball->ballRescueTime < 5000 ) {
		/* its own spot is bad (a test ball spawned in a wall): remove it */
		trap_SendServerCommand( -1, "print \"A ball kept leaving the arena and was removed.\n\"" );
		G_Autoball_RemoveBall( ball );
		return qfalse;
	}
	ball->ballRescueTime = level.time;
	if ( ball != mainBall ) {
		/* an extra ball (g_autoballBalls) just goes back to its spot */
		trap_SendServerCommand( -1, va( "print \"A ball was %s and is back on its spot.\n\"",
			!Q_stricmp( reason, "out of reach" ) ? "out of reach" : "out of play" ) );
		G_Autoball_ResetBall( ball );
		return qfalse;
	}
	/* the match ball: a new kick-off, so nobody gets a head start */
	trap_SendServerCommand( -1, va( "cp \"Ball %s\nNew kick-off\"", reason ) );
	trap_SendServerCommand( -1, va( "print \"Ball %s - new kick-off.\n\"", reason ) );
	G_Autoball_StartKickoff( ball );
	return qtrue;
}

static void G_Autoball_MatchFrame( void ) {
	gentity_t *ball, *goal;

	if ( g_gametype.integer != GT_AUTOBALL )
		return;
	if ( level.intermissiontime || level.intermissionQueued )
		return;

	ball = G_Autoball_MainBall();
	level.autoballBallNum = ball ? ball->s.number : -1;
	if ( level.autoballBallNum != level.autoballPublished )
		G_Autoball_Publish();
	G_Autoball_StatsSample( ball );

	switch ( level.autoballState ) {
	case AUTOBALL_STATE_WAITING:
		if ( ball && !level.warmupTime && level.numPlayingClients > 0 && !G_Autoball_HumansLoading() )
			G_Autoball_StartKickoff( ball );
		break;

	case AUTOBALL_STATE_KICKOFF:
		G_Autoball_Countdown( ball );
		break;

	case AUTOBALL_STATE_LIVE:
		if ( !ball ) {
			G_Autoball_SetState( AUTOBALL_STATE_WAITING );
			break;
		}
		{
			gentity_t *any = NULL;
			while ( ( any = G_Find( any, FOFS( classname ), AUTOBALL_CLASSNAME ) ) != NULL ) {
				goal = G_Autoball_GoalContaining( any->r.currentOrigin );
				if ( goal ) {
					G_Autoball_ScoreGoal( any, goal );
					break;
				}
				G_Autoball_JudgeTouch( any );
				if ( G_Autoball_RescueBall( any, ball ) )
					break;
			}
		}
		break;

	case AUTOBALL_STATE_GOAL:
		if ( level.time >= level.autoballStateEnd )
			G_Autoball_StartKickoff( ball );
		break;

	default:
		G_Autoball_SetState( AUTOBALL_STATE_WAITING );
		break;
	}
}
