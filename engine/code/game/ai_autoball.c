/*
===========================================================================
Copyright (C) 2026 Q3Rally Team

This file is part of Q3Rally source code.

Q3Rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Q3Rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Q3Rally source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================

ai_autoball.c -- bot driving for Autoball (car football)

The ball moves far too fast for AAS goals, so Autoball bots steer directly:
they point their view at a target point (mouse steering turns the wheels
towards the view) and hold the throttle, like the derby roam driver.

Every frame each team ranks its cars (humans included) by how cheaply they
can hit the ball towards the right direction:
  ATTACK   the best placed car: get behind the ball, then hit it
           towards the opponent goal (or away from the own goal)
  DEFEND   the car closest to its own goal (teams of 2+): hold a spot
           between ball and goal, clear the ball when it gets close
  SUPPORT  everybody else: wait behind the ball, collect turbo
The current attacker gets a bonus so roles don't flip every frame.
g_autoballDebug 2 prints role changes.
===========================================================================
*/

#include "g_local.h"
#include "../botlib/botlib.h"
#include "../botlib/be_aas.h"
#include "../botlib/be_ea.h"
#include "../botlib/be_ai_char.h"
#include "../botlib/be_ai_chat.h"
#include "../botlib/be_ai_gen.h"
#include "../botlib/be_ai_goal.h"
#include "../botlib/be_ai_move.h"
#include "../botlib/be_ai_weap.h"
#include "ai_main.h"
#include "ai_dmq3.h"

#define AB_BOT_ROLE_ATTACK      0
#define AB_BOT_ROLE_DEFEND      1
#define AB_BOT_ROLE_SUPPORT     2

#define AB_BOT_ATTACKER_BONUS   450.0f   /* hysteresis for the current attacker */
#define AB_BOT_DANGER_RADIUS    1700.0f  /* ball this close to own goal: clear it */
#define AB_BOT_DEFEND_DIST      750.0f   /* defend spot, from the goal volume centre */
#define AB_BOT_SUPPORT_DIST     1300.0f  /* support spot, behind the ball */
#define AB_BOT_STUCK_TIME       1.2f
#define AB_BOT_REVERSE_TIME     1.0f
#define AB_BOT_TURBO_SEARCH     1600.0f
#define AB_BOT_TURBO_LOW        2000     /* msec of stored turbo that counts as "low" */

static int   ab_teamAttacker[TEAM_NUM_TEAMS];
static int   ab_role[MAX_CLIENTS];
static int   ab_turbo[MAX_CLIENTS];
static float ab_slowSince[MAX_CLIENTS];
static float ab_reverseUntil[MAX_CLIENTS];
static float ab_reverseYaw[MAX_CLIENTS];
static int   ab_drive[MAX_CLIENTS];		/* asked for throttle this frame */
static float ab_lastThink[MAX_CLIENTS];
static vec3_t ab_aimError[MAX_CLIENTS];
static float ab_aimErrorTime[MAX_CLIENTS];

/* bot skill 1..5 -> 0..1 */
static float BotAutoball_Skill( bot_state_t *bs ) {
	float s = ( bs->settings.skill - 1.0f ) / 4.0f;

	if ( s < 0.0f )
		return 0.0f;
	if ( s > 1.0f )
		return 1.0f;
	return s;
}

/* read by BotUpdateInput (ai_main.c): hold BUTTON_TURBO this frame */
qboolean BotAutoball_WantsTurbo( int client ) {
	if ( client < 0 || client >= MAX_CLIENTS || gametype != GT_AUTOBALL )
		return qfalse;
	/* only while the bot is actually driving Autoball (not dead, not in
	   another AI node, not a new bot in an old slot) */
	if ( FloatTime() - ab_lastThink[client] > 0.25f )
		return qfalse;
	return ab_turbo[client] ? qtrue : qfalse;
}

/* The ball the team plays. With several balls (g_autoballBalls) it is the
   one closest to the own goal, so all bots of a team agree on it. */
static gentity_t *BotAutoball_Ball( const vec3_t ownGoal ) {
	gentity_t *ball = NULL, *best = NULL;
	vec3_t d;
	float dist, bestDist = 1.0e18f;

	while ( ( ball = G_Find( ball, FOFS( classname ), "autoball_ball" ) ) != NULL ) {
		VectorSubtract( ball->r.currentOrigin, ownGoal, d );
		d[2] = 0.0f;
		dist = VectorLengthSquared( d );
		if ( dist < bestDist ) {
			bestDist = dist;
			best = ball;
		}
	}
	return best;
}

/* centre of the goal volume defended by team, qfalse if the map has none */
static qboolean BotAutoball_GoalCentre( int team, vec3_t out ) {
	gentity_t *goal = NULL;

	while ( ( goal = G_Find( goal, FOFS( classname ), "autoball_goal" ) ) != NULL ) {
		if ( goal->count != team )
			continue;
		VectorAdd( goal->r.absmin, goal->r.absmax, out );
		VectorScale( out, 0.5f, out );
		return qtrue;
	}
	return qfalse;
}

static float BotAutoball_Dir2D( const vec3_t from, const vec3_t to, vec3_t dir ) {
	VectorSubtract( to, from, dir );
	dir[2] = 0.0f;
	return VectorNormalize( dir );
}

static int BotAutoball_TurboStored( const playerState_t *ps ) {
	int t = ps->powerups[PW_TURBO];

	if ( t > level.time )
		return t - level.time;
	if ( t < 0 )
		return -t;
	return 0;
}

/* how badly placed a car is to hit the ball in pushDir: distance plus a
   penalty for approaching from the wrong side */
static float BotAutoball_AttackCost( const vec3_t car, const vec3_t ball, const vec3_t pushDir ) {
	vec3_t toBall;
	float dist, align;

	dist = BotAutoball_Dir2D( car, ball, toBall );
	align = DotProduct( toBall, pushDir );
	if ( align < 0.0f )
		dist += -align * 1400.0f;
	return dist;
}

static int BotAutoball_Role( bot_state_t *bs, const vec3_t ballPos, const vec3_t pushDir, const vec3_t ownGoal ) {
	int i, team, attacker, defender, count;
	float cost, bestCost, goalDist, bestGoalDist;
	gentity_t *ent;
	vec3_t d;

	team = BotTeam( bs );
	if ( team != TEAM_RED && team != TEAM_BLUE )
		return AB_BOT_ROLE_ATTACK;

	attacker = -1;
	bestCost = 1.0e9f;
	count = 0;
	for ( i = 0; i < level.maxclients; i++ ) {
		ent = &g_entities[i];
		if ( !ent->inuse || !ent->client || ent->client->pers.connected != CON_CONNECTED )
			continue;
		if ( ent->client->sess.sessionTeam != team || ent->health <= 0 )
			continue;
		count++;
		cost = BotAutoball_AttackCost( ent->client->ps.origin, ballPos, pushDir );
		if ( i == ab_teamAttacker[team] )
			cost -= AB_BOT_ATTACKER_BONUS;
		if ( cost < bestCost ) {
			bestCost = cost;
			attacker = i;
		}
	}
	ab_teamAttacker[team] = attacker;
	if ( attacker == bs->client || count < 2 )
		return AB_BOT_ROLE_ATTACK;

	defender = -1;
	bestGoalDist = 1.0e9f;
	for ( i = 0; i < level.maxclients; i++ ) {
		ent = &g_entities[i];
		if ( i == attacker || !ent->inuse || !ent->client || ent->client->pers.connected != CON_CONNECTED )
			continue;
		if ( ent->client->sess.sessionTeam != team || ent->health <= 0 )
			continue;
		VectorSubtract( ent->client->ps.origin, ownGoal, d );
		d[2] = 0.0f;
		goalDist = VectorLength( d );
		if ( goalDist < bestGoalDist ) {
			bestGoalDist = goalDist;
			defender = i;
		}
	}
	return ( defender == bs->client ) ? AB_BOT_ROLE_DEFEND : AB_BOT_ROLE_SUPPORT;
}

/* nearest turbo pickup that is currently available */
static qboolean BotAutoball_NearestTurbo( const vec3_t from, float maxDist, vec3_t out ) {
	int i;
	gentity_t *ent;
	float d, best = maxDist;
	vec3_t delta;
	qboolean found = qfalse;

	for ( i = MAX_CLIENTS; i < level.num_entities; i++ ) {
		ent = &g_entities[i];
		if ( !ent->inuse || !ent->item || !( ent->r.contents & CONTENTS_TRIGGER ) )
			continue;
		if ( !( ( ent->item->giType == IT_POWERUP && ent->item->giTag == PW_TURBO ) ||
			( ent->item->giType == IT_HOLDABLE && ent->item->giTag == HI_TURBO ) ) )
			continue;
		VectorSubtract( ent->r.currentOrigin, from, delta );
		delta[2] = 0.0f;
		d = VectorLength( delta );
		if ( d < best ) {
			best = d;
			VectorCopy( ent->r.currentOrigin, out );
			found = qtrue;
		}
	}
	return found;
}

/* steer at target; throttle 1 = drive, 0 = coast, -1 = brake/reverse */
static float BotAutoball_Steer( bot_state_t *bs, const vec3_t target, int throttle ) {
	vec3_t dir, angles, forward, end, mins, maxs;
	float err, speed;
	trace_t tr;

	if ( BotAutoball_Dir2D( bs->cur_ps.origin, target, dir ) < 1.0f )
		AngleVectors( bs->cur_ps.damageAngles, dir, NULL, NULL );

	/* target behind a wall (ball in a corner, spot outside the field):
	   slide along the wall instead of pushing into it */
	VectorSet( mins, -8, -8, -8 );
	VectorSet( maxs, 8, 8, 8 );
	VectorMA( bs->cur_ps.origin, 260.0f, dir, end );
	trap_Trace( &tr, bs->cur_ps.origin, mins, maxs, end, bs->client, MASK_SOLID );
	if ( tr.fraction < 1.0f && tr.entityNum == ENTITYNUM_WORLD && fabs( tr.plane.normal[2] ) < 0.5f ) {
		float into = DotProduct( dir, tr.plane.normal );
		if ( into < 0.0f ) {
			VectorMA( dir, -into, tr.plane.normal, dir );
			dir[2] = 0.0f;
			if ( VectorNormalize( dir ) < 0.1f )
				AngleVectors( bs->cur_ps.damageAngles, dir, NULL, NULL );
		}
	}

	vectoangles( dir, angles );
	angles[PITCH] = 0.0f;
	VectorCopy( angles, bs->ideal_viewangles );

	AngleVectors( bs->cur_ps.damageAngles, forward, NULL, NULL );
	err = fabs( AngleDifference( angles[YAW], bs->cur_ps.damageAngles[YAW] ) );
	speed = VectorLength( bs->cur_ps.velocity );

	/* sharp turn at speed: lift off so the car can turn in */
	if ( throttle > 0 && err > 75.0f && speed > 1100.0f )
		throttle = 0;

	if ( throttle > 0 ) {
		trap_EA_MoveForward( bs->client );
		ab_drive[bs->client] = 1;
	} else if ( throttle < 0 ) {
		trap_EA_MoveBack( bs->client );
	}
	return err;
}

/* hold a spot: drive there, brake on arrival, then wait */
static void BotAutoball_HoldSpot( bot_state_t *bs, const vec3_t spot, const vec3_t lookAt ) {
	vec3_t d;
	float dist, speed;

	VectorSubtract( spot, bs->cur_ps.origin, d );
	d[2] = 0.0f;
	dist = VectorLength( d );
	speed = VectorLength( bs->cur_ps.velocity );

	if ( dist > 320.0f ) {
		BotAutoball_Steer( bs, spot, 1 );
	} else if ( speed > 280.0f ) {
		BotAutoball_Steer( bs, lookAt, -1 );
	} else {
		BotAutoball_Steer( bs, lookAt, 0 );
	}
}

static void BotAutoball_Attack( bot_state_t *bs, gentity_t *ball, const vec3_t pushDir, qboolean *turbo ) {
	vec3_t ballPos, aim, toBall, side, approach;
	float dist, align, err, speed, lead, sideSign, skill;

	skill = BotAutoball_Skill( bs );

	/* lead the ball by the time it takes to reach it (weaker bots less) */
	speed = VectorLength( bs->cur_ps.velocity );
	dist = BotAutoball_Dir2D( bs->cur_ps.origin, ball->r.currentOrigin, toBall );
	lead = dist / ( speed > 900.0f ? speed : 900.0f );
	if ( lead > 1.0f )
		lead = 1.0f;
	lead *= 0.3f + 0.7f * skill;
	VectorMA( ball->r.currentOrigin, lead, ball->s.pos.trDelta, ballPos );
	ballPos[2] = ball->r.currentOrigin[2];

	/* weaker bots aim sloppier; the error changes every second */
	if ( ab_aimErrorTime[bs->client] < FloatTime() ) {
		float amount = ( 1.0f - skill ) * 140.0f;
		ab_aimErrorTime[bs->client] = FloatTime() + 1.0f;
		VectorSet( ab_aimError[bs->client], crandom() * amount, crandom() * amount, 0.0f );
	}
	VectorAdd( ballPos, ab_aimError[bs->client], ballPos );

	dist = BotAutoball_Dir2D( bs->cur_ps.origin, ballPos, toBall );
	align = DotProduct( toBall, pushDir );

	if ( align > 0.35f || dist < 200.0f ) {
		/* behind the ball: hit it slightly off-centre so it goes along pushDir */
		VectorMA( ballPos, -( ball->ballRadius > 0 ? ball->ballRadius : 75.0f ) * 0.6f, pushDir, aim );
		err = BotAutoball_Steer( bs, aim, 1 );
		if ( skill >= 0.4f && align > 0.8f && err < 12.0f && dist > 320.0f && dist < 3000.0f )
			*turbo = qtrue;
		return;
	}

	/* wrong side: loop around the ball to a point behind it */
	VectorSet( side, -pushDir[1], pushDir[0], 0.0f );
	sideSign = ( DotProduct( bs->cur_ps.origin, side ) - DotProduct( ballPos, side ) ) >= 0.0f ? 1.0f : -1.0f;
	VectorMA( ballPos, -600.0f, pushDir, approach );
	VectorMA( approach, sideSign * ( 250.0f + 300.0f * ( 1.0f - align ) * 0.5f ), side, approach );
	err = BotAutoball_Steer( bs, approach, 1 );
	if ( skill >= 0.7f && err < 10.0f && dist > 1800.0f )
		*turbo = qtrue;
}

static void BotAutoball_DebugRole( bot_state_t *bs, int role ) {
	static const char *names[] = { "attack", "defend", "support" };

	if ( ab_role[bs->client] == role )
		return;
	ab_role[bs->client] = role;
	if ( g_autoballDebug.integer >= 2 )
		G_Printf( "Autoball bot %d (%s): %s\n", bs->client,
			BotTeam( bs ) == TEAM_BLUE ? "blue" : "red", names[role] );
}

/*
==================
BotAutoball_Think

Called from AINode_Seek_LTG instead of the item/enemy logic. Returns qtrue
when it produced input for this frame.
==================
*/
qboolean BotAutoball_Think( bot_state_t *bs ) {
	gentity_t *ball;
	vec3_t ownGoal, oppGoal, pushDir, toOwn, spot, turboSpot;
	float now, speed, ownDist;
	int team, role, client;
	qboolean turbo = qfalse;

	if ( gametype != GT_AUTOBALL )
		return qfalse;
	client = bs->client;
	ab_turbo[client] = 0;
	bs->enemy = -1;

	team = BotTeam( bs );
	if ( !BotAutoball_GoalCentre( team, ownGoal ) ||
		!BotAutoball_GoalCentre( team == TEAM_RED ? TEAM_BLUE : TEAM_RED, oppGoal ) )
		return qtrue;	/* no goals (broken map): stand still */
	ball = BotAutoball_Ball( ownGoal );
	if ( !ball )
		return qtrue;

	/* frozen at kick-off: just look at the ball */
	if ( G_Autoball_CarsFrozen( level.time ) || level.autoballState == AUTOBALL_STATE_GOAL ) {
		BotAutoball_Steer( bs, ball->r.currentOrigin, 0 );
		ab_slowSince[client] = 0.0f;
		return qtrue;
	}

	now = FloatTime();

	/* first think after a respawn or a pause: no stale stuck state */
	if ( now - ab_lastThink[client] > 0.5f ) {
		ab_slowSince[client] = 0.0f;
		ab_reverseUntil[client] = 0.0f;
	}
	ab_lastThink[client] = now;

	/* stuck against a wall or another car: back off with a turn */
	if ( ab_reverseUntil[client] > now ) {
		vec3_t angles;
		VectorSet( angles, 0.0f, ab_reverseYaw[client], 0.0f );
		VectorCopy( angles, bs->ideal_viewangles );
		trap_EA_MoveBack( client );
		return qtrue;
	}
	ab_drive[client] = 0;

	/* push direction: towards the opponent goal, or simply away from our
	   own goal when the ball is in our danger zone */
	ownDist = BotAutoball_Dir2D( ownGoal, ball->r.currentOrigin, toOwn );
	if ( ownDist < AB_BOT_DANGER_RADIUS ) {
		VectorCopy( toOwn, pushDir );
	} else {
		BotAutoball_Dir2D( ball->r.currentOrigin, oppGoal, pushDir );
	}

	role = BotAutoball_Role( bs, ball->r.currentOrigin, pushDir, ownGoal );
	/* a defender clears the ball itself once it is close */
	if ( role == AB_BOT_ROLE_DEFEND && ownDist < AB_BOT_DANGER_RADIUS * 0.75f )
		role = AB_BOT_ROLE_ATTACK;
	BotAutoball_DebugRole( bs, role );

	switch ( role ) {
	case AB_BOT_ROLE_ATTACK:
		BotAutoball_Attack( bs, ball, pushDir, &turbo );
		break;

	case AB_BOT_ROLE_DEFEND:
		/* between ball and goal; hurry back when far out of position */
		VectorMA( ownGoal, AB_BOT_DEFEND_DIST, toOwn, spot );
		spot[2] = bs->cur_ps.origin[2];
		BotAutoball_HoldSpot( bs, spot, ball->r.currentOrigin );
		if ( Distance( spot, bs->cur_ps.origin ) > 2200.0f &&
			fabs( AngleDifference( bs->ideal_viewangles[YAW], bs->cur_ps.damageAngles[YAW] ) ) < 10.0f )
			turbo = qtrue;
		break;

	default:
		/* support: refuel if low, otherwise wait behind the ball */
		if ( BotAutoball_TurboStored( &bs->cur_ps ) < AB_BOT_TURBO_LOW &&
			BotAutoball_NearestTurbo( bs->cur_ps.origin, AB_BOT_TURBO_SEARCH, turboSpot ) ) {
			BotAutoball_Steer( bs, turboSpot, 1 );
			break;
		}
		VectorSubtract( ownGoal, ball->r.currentOrigin, spot );
		spot[2] = 0.0f;
		if ( VectorNormalize( spot ) < 1.0f )
			VectorCopy( toOwn, spot );
		if ( ownDist * 0.5f < AB_BOT_SUPPORT_DIST )
			VectorMA( ball->r.currentOrigin, ownDist * 0.5f, spot, spot );
		else
			VectorMA( ball->r.currentOrigin, AB_BOT_SUPPORT_DIST, spot, spot );
		spot[2] = bs->cur_ps.origin[2];
		BotAutoball_HoldSpot( bs, spot, ball->r.currentOrigin );
		break;
	}

	if ( turbo && BotAutoball_TurboStored( &bs->cur_ps ) > 0 )
		ab_turbo[client] = 1;

	/* stuck: wants to drive but doesn't move (a car waiting on its spot
	   brakes on purpose and is not stuck) */
	speed = VectorLength( bs->cur_ps.velocity );
	if ( ab_drive[client] && speed < 70.0f ) {
		if ( !ab_slowSince[client] ) {
			ab_slowSince[client] = now;
		} else if ( now - ab_slowSince[client] > AB_BOT_STUCK_TIME ) {
			ab_reverseUntil[client] = now + AB_BOT_REVERSE_TIME;
			ab_reverseYaw[client] = AngleMod( bs->cur_ps.damageAngles[YAW] + ( random() < 0.5f ? 50.0f : -50.0f ) );
			ab_slowSince[client] = 0.0f;
		}
	} else {
		ab_slowSince[client] = 0.0f;
	}
	return qtrue;
}
