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
//

/*****************************************************************************
 * name:		ai_dmnet.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /MissionPack/code/game/ai_dmnet.c $
 *
 *****************************************************************************/

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
//
#include "ai_main.h"
#include "ai_dmq3.h"
#include "ai_chat.h"
#include "ai_cmd.h"
#include "ai_dmnet.h"
#include "ai_dmnet_pathselect.h"
#include "ai_team.h"
//data file headers
#include "chars.h"			//characteristics
#include "inv.h"			//indexes into the inventory
#include "syn.h"			//synonyms
#include "match.h"			//string matching types and vars

// for the voice chats
#include "../../ui/menudef.h"

//goal flag, see ../botlib/be_ai_goal.h for the other GFL_*
#define GFL_AIR			128

int numnodeswitches;
char nodeswitch[MAX_NODESWITCHES+1][144];

#define LOOKAHEAD_DISTANCE			300
#define GHOST_ROUTE_HINT_WINDOW		48
#define GHOST_ROUTE_LOOKAHEAD_MS	900
#define GHOST_RECOVERY_ROUTE_DIST_THRESHOLD	260.0f
#define GHOST_RECOVERY_MIN_PROGRESS		70.0f
#define GHOST_RECOVERY_SAMPLE_WINDOW		0.55f
#define GHOST_RECOVERY_MAX_COLLISION_COUNT	4
#define GHOST_RECOVERY_MAX_REVERSE_TIME	1.35f
#define GHOST_RECOVERY_REVERSE_CYCLE_WINDOW	4.5f
#define GHOST_RECOVERY_MAX_REVERSE_CYCLES	3
#define GHOST_RECOVERY_REVERSE_MIN_PROGRESS	38.0f
#define GHOST_RECOVERY_REVERSE_COLLISION_SPIKE	2
#define GHOST_RECOVERY_REJOIN_STEER_LIMIT	16.0f
#define GHOST_RECOVERY_REJOIN_THROTTLE_STEP	0.22f
#define GHOST_FORWARD_DOT_SOFT_REJECT		-0.05f
#define GHOST_FORWARD_DOT_STRICT_REJECT		0.25f

#define GHOST_FORWARD_INIT_PHASE_MS		1500
#define GHOST_RECOVERY_REARM_DELAY		0.75f

typedef enum {
	GHOST_DECISION_FOLLOW = 0,
	GHOST_DECISION_PREPARE_OVERTAKE,
	GHOST_DECISION_OVERTAKE_INSIDE,
	GHOST_DECISION_OVERTAKE_OUTSIDE,
	GHOST_DECISION_DEFEND_LINE,
	GHOST_DECISION_ABORT_OVERTAKE
} ghostDecisionState_t;

typedef struct {
	float nearestAheadDist;
	float nearestBehindDist;
	float nearestAheadRelSpeed;
	float nearestBehindRelSpeed;
	float nearestAheadLateral;
	float nearestBehindLateral;
	float sideSafetyInside;
	float sideSafetyOutside;
	qboolean hasPredictedConflict;
	qboolean laneSwapRecommended;
	qboolean abortOvertakeRecommended;
	float recommendedSpeedBias;
} botCollisionRisk_t;

static const char *Bot_DebugDecisionStateName( ghostDecisionState_t state ) {
	switch ( state ) {
		case GHOST_DECISION_PREPARE_OVERTAKE: return "prepare_overtake";
		case GHOST_DECISION_OVERTAKE_INSIDE: return "overtake_inside";
		case GHOST_DECISION_OVERTAKE_OUTSIDE: return "overtake_outside";
		case GHOST_DECISION_DEFEND_LINE: return "defend_line";
		case GHOST_DECISION_ABORT_OVERTAKE: return "abort_overtake";
		case GHOST_DECISION_FOLLOW:
		default:
			return "follow";
	}
}

static const char *Bot_DebugRecoveryStateName( bot_recovery_state_t state ) {
	switch ( state ) {
		case BOT_RECOVERY_STUCK_DETECT: return "stuck_detect";
		case BOT_RECOVERY_REVERSE_UNWIND: return "reverse_unwind";
		case BOT_RECOVERY_REJOIN_ROUTE: return "rejoin_route";
		case BOT_RECOVERY_EMERGENCY_RESET_REQUEST: return "emergency_reset";
		case BOT_RECOVERY_NONE:
		default:
			return "none";
	}
}

static const char *Bot_DebugObjectiveStateName( int state ) {
	switch ( state ) {
		case 1: return "dom_neutralize";
		case 2: return "dom_capture";
		case 3: return "dom_defend";
		case 4: return "koth_contest";
		case 5: return "koth_defend";
		case 6: return "health_retreat";
		case 7: return "lcs_avoid_contact";
		case 8: return "lcs_break_engagement";
		default: return "none";
	}
}

static void Bot_DebugGetObjectiveSnapshot( bot_state_t *bs, int *objectiveState, int *kothOwner, int *kothContested ) {
	int state = 0;
	int owner = TEAM_FREE;
	int contested = 0;

	if ( bs ) {
		if ( bs->inventory[INVENTORY_HEALTH] > 0 && bs->inventory[INVENTORY_HEALTH] < 40 ) {
			state = 6;
		}
		if ( gametype == GT_DOMINATION ) {
			bot_goal_t sigilGoal;
			int sigilStatus = SIGIL_NONE;
			int team = BotTeam( bs );

			if ( BotGetDominationSigilGoal( bs, &sigilGoal, &sigilStatus ) ) {
				if ( sigilStatus == SIGIL_ISWHITE ) {
					state = 2;
				} else if ( (team == TEAM_RED && sigilStatus == SIGIL_ISBLUE) ||
					(team == TEAM_BLUE && sigilStatus == SIGIL_ISRED) ) {
					state = 1;
				} else {
					state = 3;
				}
			}
		}
		else if ( gametype == GT_KOTH ) {
			int capturePct;
			float hillRadius;
			vec3_t hillOrigin;

			if ( BotGetKOTHStatus( &owner, &contested, &capturePct, hillOrigin, &hillRadius ) ) {
				if ( owner == BotTeam( bs ) && !contested && capturePct >= 100 ) {
					state = 5;
				} else {
					state = 4;
				}
			}
		}
		else if ( gametype == GT_LCS ) {
			if ( bs->inventory[INVENTORY_HEALTH] < 50 ) {
				state = 8;
			} else if ( bs->inventory[INVENTORY_HEALTH] < 70 ) {
				state = 7;
			}
		}
	}

	if ( objectiveState ) {
		*objectiveState = state;
	}
	if ( kothOwner ) {
		*kothOwner = owner;
	}
	if ( kothContested ) {
		*kothContested = contested;
	}
}

static qboolean Bot_LcsShouldAvoidBattleEntry( bot_state_t *bs, const char **reasonOut ) {
	float relativePosition = 0.5f;
	float threatProximity = 0.0f;
	int remainingOpponents = 0;

	if ( reasonOut ) {
		*reasonOut = "none";
	}
	if ( !bs || gametype != GT_LCS ) {
		return qfalse;
	}

	if ( BotGetLcsRiskMetrics( bs, &relativePosition, &threatProximity, &remainingOpponents ) ) {
		if ( bs->inventory[INVENTORY_HEALTH] < 62 ) {
			if ( reasonOut ) {
				*reasonOut = "lcs_avoid_low_health";
			}
			return qtrue;
		}
		if ( threatProximity > 0.45f && remainingOpponents > 1 ) {
			if ( reasonOut ) {
				*reasonOut = "lcs_avoid_cluster";
			}
			return qtrue;
		}
		if ( remainingOpponents > 2 && relativePosition < 0.35f ) {
			if ( reasonOut ) {
				*reasonOut = "lcs_avoid_last_place_risk";
			}
			return qtrue;
		}
	}

	return qfalse;
}

static qboolean Bot_LcsShouldBreakEngagement( bot_state_t *bs, const botCollisionRisk_t *risk, const char **triggerOut ) {
	float relativePosition = 0.5f;
	float threatProximity = 0.0f;
	int remainingOpponents = 0;

	if ( triggerOut ) {
		*triggerOut = "none";
	}
	if ( !bs || gametype != GT_LCS ) {
		return qfalse;
	}

	if ( bs->inventory[INVENTORY_HEALTH] <= 45 ) {
		if ( triggerOut ) {
			*triggerOut = "low_health";
		}
		return qtrue;
	}

	if ( risk && risk->hasPredictedConflict &&
		risk->nearestAheadDist < 95.0f && risk->nearestBehindDist < 85.0f ) {
		if ( triggerOut ) {
			*triggerOut = "cluster_danger";
		}
		return qtrue;
	}

	if ( BotGetLcsRiskMetrics( bs, &relativePosition, &threatProximity, &remainingOpponents ) ) {
		if ( remainingOpponents > 2 && relativePosition < 0.32f && threatProximity > 0.35f ) {
			if ( triggerOut ) {
				*triggerOut = "last_place_risk";
			}
			return qtrue;
		}
	}

	return qfalse;
}

static void Bot_DebugFormatRecoveryTransition( char *buffer, int bufferSize, bot_recovery_state_t fromState, bot_recovery_state_t toState ) {
	if ( !buffer || bufferSize <= 0 ) {
		return;
	}
	if ( fromState == toState ) {
		Com_sprintf( buffer, bufferSize, "%s", Bot_DebugRecoveryStateName( toState ) );
		return;
	}
	Com_sprintf( buffer, bufferSize, "%s->%s",
		Bot_DebugRecoveryStateName( fromState ),
		Bot_DebugRecoveryStateName( toState ) );
}

static void Bot_DebugExportDmnetTick( bot_state_t *bs, int routeIndex, float targetSpeed, float actualSpeed,
	ghostDecisionState_t decisionState, qboolean collisionRisk, bot_recovery_state_t recoveryState,
	bot_recovery_state_t previousRecoveryState, const char *recoveryEvent,
	const char *recoveryTrigger, float routeDeviation, int pathId, int nodeIndex, int lookAheadIndex,
	int widthClampEvent, int autoSpeedActive, int targetSpeedOverrideActive, int launchGateActive,
	int objectiveState, int kothOwner, int kothContested ) {
	fileHandle_t f;
	char line[1024];
	char recoveryTransition[96];
	char autoPath[MAX_QPATH];
	char mapname[MAX_QPATH];
	vec3_t origin;
	int mode;
	const char *path;
	int isJson;
	int len;

	mode = g_aiDmnetDebugExport.integer;
	VectorCopy( bs->cur_ps.origin, origin );

	if ( mode <= 0 ) {
		return;
	}

	trap_Cvar_VariableStringBuffer( "mapname", mapname, sizeof( mapname ) );
	if ( mapname[0] ) {
		Com_sprintf( autoPath, sizeof( autoPath ),
			( mode == 2 ) ? "logs/%s_bot.jsonl" : "logs/%s_bot.csv", mapname );
	} else {
		Com_sprintf( autoPath, sizeof( autoPath ),
			( mode == 2 ) ? "logs/bot.jsonl" : "logs/bot.csv" );
	}
	path = g_aiDmnetDebugExportPath.string;
	if ( !path || !path[0] ) {
		path = autoPath;
	}
	isJson = ( mode == 2 );

	len = trap_FS_FOpenFile( path, &f, FS_APPEND );
	if ( f <= 0 ) {
		return;
	}

	if ( len == 0 && !isJson ) {
		char header[] = "time,client,routeIndex,targetSpeed,actualSpeed,decisionState,collisionRisk,recoveryState,recoveryTransition,recoveryEvent,recoveryTrigger,routeDeviation,pathId,nodeIndex,lookaheadIndex,widthClampEvent,autoSpeedActive,targetSpeedOverrideActive,launchGate,objectiveState,kothOwner,kothContested\n";
		trap_FS_Write( header, strlen( header ), f );
	}
	Bot_DebugFormatRecoveryTransition( recoveryTransition, sizeof( recoveryTransition ), previousRecoveryState, recoveryState );

	if ( isJson ) {
		Com_sprintf( line, sizeof( line ),
			"{\"time\":%.3f,\"client\":%d,\"routeIndex\":%d,\"targetSpeed\":%.2f,\"actualSpeed\":%.2f,"
			"\"decisionState\":\"%s\",\"collisionRisk\":%d,\"recoveryState\":\"%s\","
			"\"recoveryTransition\":\"%s\",\"recoveryEvent\":\"%s\",\"recoveryTrigger\":\"%s\",\"routeDeviation\":%.2f,"
			"\"pathId\":%d,\"nodeIndex\":%d,\"lookaheadIndex\":%d,\"widthClampEvent\":%d,"
			"\"autoSpeedActive\":%d,\"targetSpeedOverrideActive\":%d,\"launchGate\":%d,"
			"\"objectiveState\":\"%s\",\"kothOwner\":%d,\"kothContested\":%d,"
			"\"ox\":%.2f,\"oy\":%.2f,\"oz\":%.2f}\n",
			level.time * 0.001f, bs->client, routeIndex, targetSpeed, actualSpeed,
			Bot_DebugDecisionStateName( decisionState ), collisionRisk ? 1 : 0,
			Bot_DebugRecoveryStateName( recoveryState ), recoveryTransition,
			recoveryEvent ? recoveryEvent : "", recoveryTrigger ? recoveryTrigger : "",
			routeDeviation, pathId, nodeIndex, lookAheadIndex, widthClampEvent,
			autoSpeedActive, targetSpeedOverrideActive, launchGateActive,
			Bot_DebugObjectiveStateName( objectiveState ), kothOwner, kothContested,
			origin[0], origin[1], origin[2] );
	} else {
		Com_sprintf( line, sizeof( line ),
			"%.3f,%d,%d,%.2f,%.2f,%s,%d,%s,%s,%s,%s,%.2f,%d,%d,%d,%d,%d,%d,%d,%s,%d,%d\n",
			level.time * 0.001f, bs->client, routeIndex, targetSpeed, actualSpeed,
			Bot_DebugDecisionStateName( decisionState ), collisionRisk ? 1 : 0,
			Bot_DebugRecoveryStateName( recoveryState ), recoveryTransition,
			recoveryEvent ? recoveryEvent : "", recoveryTrigger ? recoveryTrigger : "",
			routeDeviation, pathId, nodeIndex, lookAheadIndex, widthClampEvent,
			autoSpeedActive, targetSpeedOverrideActive, launchGateActive,
			Bot_DebugObjectiveStateName( objectiveState ), kothOwner, kothContested );
	}
	trap_FS_Write( line, strlen( line ), f );
	trap_FS_FCloseFile( f );
}

static ghostRouteLineFamily_t Bot_SelectGhostLineFamily( const botCollisionRisk_t *risk, float cornerPhase, qboolean chaosActive ) {
	if ( !risk ) {
		return GHOST_LINE_BASE;
	}
	if ( gametype == GT_LCS ) {
		if ( chaosActive || risk->hasPredictedConflict || risk->nearestAheadDist < 130.0f ) {
			return GHOST_LINE_SAFE;
		}
		return GHOST_LINE_DEFENSIVE;
	}

	if ( chaosActive ) {
		return GHOST_LINE_SAFE;
	}

	if ( risk->nearestBehindDist < 130.0f && risk->nearestBehindRelSpeed > 55.0f ) {
		return GHOST_LINE_DEFENSIVE;
	}

	if ( risk->nearestAheadDist < 220.0f && risk->nearestAheadRelSpeed > 20.0f && cornerPhase < 0.70f ) {
		return GHOST_LINE_RACE;
	}

	return GHOST_LINE_BASE;
}

typedef enum {
	BOT_PATH_LINE_BASE = 0,
	BOT_PATH_LINE_AGGRESSIVE = 1,
	BOT_PATH_LINE_SAFE = 2,
	BOT_PATH_LINE_FAMILY_COUNT = 3
} botPathLineFamily_t;

static int Bot_SelectBotPathRouteIdWithFallback( const botPathRoute_t *routes[BOT_PATH_LINE_FAMILY_COUNT], int preferredId ) {
	qboolean available[BOT_PATH_LINE_FAMILY_COUNT];
	int i;

	for ( i = 0; i < BOT_PATH_LINE_FAMILY_COUNT; ++i ) {
		available[i] = ( routes[i] && routes[i]->valid && routes[i]->numNodes > 1 ) ? qtrue : qfalse;
	}

	return Bot_SelectRouteIdWithFallbackByAvailability( available, BOT_PATH_LINE_FAMILY_COUNT, preferredId, BOT_PATH_LINE_BASE );
}

static qboolean Bot_BuildBotPathGuidance( const botPathRoute_t *route, bot_state_t *bs, float actualSpeed, vec3_t targetPoint,
	float *targetSpeed, float *avgCurvatureOut, int *nodeIndexOut, int *lookAheadIndexOut ) {
	int closestIndex;
	int lookAheadIndex;
	int segmentStart;
	int segmentEnd;
	int segmentSamples = 0;
	int i;
	float segmentSpeedSum = 0.0f;
	float segmentCurvatureSum = 0.0f;

	if ( !route || !bs || !targetPoint || !targetSpeed ) {
		return qfalse;
	}
	if ( !route->valid || route->numNodes <= 1 ) {
		return qfalse;
	}

	closestIndex = G_BotPath_SelectClosestNode( route, bs->cur_ps.origin, bs->botPathRouteIndexHint, GHOST_ROUTE_HINT_WINDOW );
	if ( closestIndex < 0 ) {
		return qfalse;
	}

	// Distance-based lookahead: target a fixed distance ahead along the route
	// instead of a fixed node count. This prevents the bot from steering to
	// nodes far across the map on routes with uneven segment lengths.
	{
		float targetLookAheadDist = 400.0f + actualSpeed * 0.35f;
		float cumDist = 0.0f;
		lookAheadIndex = closestIndex;
		while ( lookAheadIndex < route->numNodes - 1 ) {
			float segLen = route->segments[lookAheadIndex].length;
			if ( cumDist + segLen >= targetLookAheadDist ) {
				break;
			}
			cumDist += segLen;
			lookAheadIndex++;
		}
		if ( lookAheadIndex <= closestIndex ) {
			lookAheadIndex = closestIndex + 1;
			if ( lookAheadIndex >= route->numNodes ) {
				lookAheadIndex = route->numNodes - 1;
			}
		}
	}

	segmentStart = closestIndex;
	segmentEnd = lookAheadIndex - 1;
	if ( segmentStart < 0 ) {
		segmentStart = 0;
	}
	if ( segmentEnd >= route->numSegments ) {
		segmentEnd = route->numSegments - 1;
	}

	for ( i = segmentStart; i <= segmentEnd; ++i ) {
		segmentSpeedSum += route->segments[i].recommendedSpeed;
		segmentCurvatureSum += route->segments[i].curvature;
		segmentSamples++;
	}

	if ( segmentSamples > 0 ) {
		float avgCurvature = segmentCurvatureSum / segmentSamples;
		*targetSpeed = segmentSpeedSum / segmentSamples;
		*targetSpeed *= ( 1.02f - avgCurvature * 0.24f );
		if ( avgCurvatureOut ) {
			*avgCurvatureOut = avgCurvature;
		}
	} else {
		*targetSpeed = route->nodes[closestIndex].targetSpeed;
		if ( *targetSpeed < 0.0f ) {
			*targetSpeed = 700.0f;
		}
		if ( avgCurvatureOut ) {
			*avgCurvatureOut = 0.0f;
		}
	}

	VectorCopy( route->nodes[lookAheadIndex].origin, targetPoint );
	if ( nodeIndexOut ) {
		*nodeIndexOut = closestIndex;
	}
	if ( lookAheadIndexOut ) {
		*lookAheadIndexOut = lookAheadIndex;
	}
	bs->botPathRouteIndexHint = closestIndex;
	return qtrue;
}

static void Bot_SetRecoveryState( bot_state_t *bs, bot_recovery_state_t newState ) {
	float now = FloatTime();

	if ( newState == BOT_RECOVERY_STUCK_DETECT && bs->ghostRecoveryState != BOT_RECOVERY_NONE ) {
		return;
	}

	if ( bs->ghostRecoveryState != newState ) {
		bs->ghostRecoveryState = newState;
		bs->ghostRecoveryStateTime = now;
		if ( newState == BOT_RECOVERY_STUCK_DETECT ) {
			VectorCopy( bs->cur_ps.origin, bs->ghostRecoveryLastOrigin );
			bs->ghostRecoveryLastSampleTime = now;
			bs->ghostRecoveryCollisionCount = 0;
		}
		if ( newState == BOT_RECOVERY_REVERSE_UNWIND ) {
			if ( now - bs->ghostRecoveryReverseWindowStart > GHOST_RECOVERY_REVERSE_CYCLE_WINDOW ) {
				bs->ghostRecoveryReverseWindowStart = now;
				bs->ghostRecoveryReverseCycles = 0;
			}
			bs->ghostRecoveryReverseCycles++;
			VectorCopy( bs->cur_ps.origin, bs->ghostRecoveryReverseStartOrigin );
			bs->ghostRecoveryReverseStartCollisionCount = bs->ghostRecoveryCollisionCount;
		}
		if ( newState == BOT_RECOVERY_REJOIN_ROUTE ) {
			bs->ghostRecoveryThrottleRamp = 0.0f;
		}
		if ( newState == BOT_RECOVERY_NONE ) {
			bs->ghostRecoveryReverseCycles = 0;
			bs->ghostRecoveryReverseWindowStart = 0.0f;
			bs->ghostRecoveryCollisionCount = 0;
			VectorCopy( bs->cur_ps.origin, bs->ghostRecoveryLastOrigin );
			bs->ghostRecoveryLastSampleTime = now;
		}
	}
}

static float Bot_ClampSteeringToRecoveryLimit( float currentYaw, float desiredYaw, float yawLimit ) {
	float yawDelta = AngleSubtract( desiredYaw, currentYaw );
	if ( yawDelta > yawLimit ) {
		yawDelta = yawLimit;
	} else if ( yawDelta < -yawLimit ) {
		yawDelta = -yawLimit;
	}
	return AngleNormalize360( currentYaw + yawDelta );
}

static int Bot_SelectForwardWaypointIndex( const ghostBotRoute_t *route, const vec3_t origin, const vec3_t forward,
	int hintIndex, int hintWindow, qboolean strictForwardOnly ) {
	int i;
	int searchStart = 0;
	int searchEnd;
	int bestIndex = -1;
	float bestScore = -999999.0f;
	float rejectDot = strictForwardOnly ? GHOST_FORWARD_DOT_STRICT_REJECT : GHOST_FORWARD_DOT_SOFT_REJECT;

	if ( !route || !route->valid || route->numWaypoints <= 0 ) {
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
		vec3_t toWaypoint;
		float distSq;
		float dotForward = 1.0f;
		float score;
		VectorSubtract( route->waypoints[i].origin, origin, toWaypoint );
		toWaypoint[2] = 0.0f;
		distSq = VectorLengthSquared( toWaypoint );
		if ( distSq > 1.0f ) {
			VectorNormalize( toWaypoint );
			dotForward = DotProduct( forward, toWaypoint );
		}

		if ( dotForward < rejectDot ) {
			continue;
		}

		/* Skip waypoints that are too close - bot standing on WP0 (distSq~0)
		   would always win, causing lookAhead to point backwards */
		if ( distSq < 80.0f * 80.0f ) {
			continue;
		}

		/* Score: forward-facing wins, closer breaks ties */
		score = dotForward * 10000.0f - distSq * 0.005f;

		if ( bestIndex < 0 || score > bestScore ) {
			bestIndex = i;
			bestScore = score;
		}
	}

	/* Fallback: if all waypoints are within minDist (e.g. very start),
	   pick nearest with positive dot */
	if ( bestIndex < 0 ) {
		for ( i = searchStart; i <= searchEnd; ++i ) {
			vec3_t toWaypoint2;
			float distSq2, dot2 = 1.0f;
			VectorSubtract( route->waypoints[i].origin, origin, toWaypoint2 );
			toWaypoint2[2] = 0.0f;
			distSq2 = VectorLengthSquared( toWaypoint2 );
			if ( distSq2 > 1.0f ) { VectorNormalize( toWaypoint2 ); dot2 = DotProduct( forward, toWaypoint2 ); }
			if ( dot2 < rejectDot ) continue;
			if ( bestIndex < 0 || distSq2 < bestScore ) { bestIndex = i; bestScore = distSq2; }
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

/*
==================
Bot_PredictCollisionRisk
==================
*/
static void Bot_PredictCollisionRisk( bot_state_t *bs, const vec3_t routeForward, const vec3_t routeRight,
	float minPredictionSec, float maxPredictionSec, botCollisionRisk_t *risk ) {
	int i;
	float myForwardSpeed;

	risk->nearestAheadDist = 4096.0f;
	risk->nearestBehindDist = 4096.0f;
	risk->nearestAheadRelSpeed = 0.0f;
	risk->nearestBehindRelSpeed = 0.0f;
	risk->nearestAheadLateral = 0.0f;
	risk->nearestBehindLateral = 0.0f;
	risk->sideSafetyInside = 9999.0f;
	risk->sideSafetyOutside = 9999.0f;
	risk->hasPredictedConflict = qfalse;
	risk->laneSwapRecommended = qfalse;
	risk->abortOvertakeRecommended = qfalse;
	risk->recommendedSpeedBias = 0.0f;

	if ( minPredictionSec < 0.5f ) {
		minPredictionSec = 0.5f;
	}
	if ( maxPredictionSec > 1.5f ) {
		maxPredictionSec = 1.5f;
	}
	if ( maxPredictionSec < minPredictionSec ) {
		maxPredictionSec = minPredictionSec;
	}

	myForwardSpeed = DotProduct( bs->cur_ps.velocity, routeForward );

	for ( i = 0; i < level.maxclients; ++i ) {
		gentity_t *otherEnt;
		vec3_t toOther;
		float ahead;
		float lateral;
		float relSpeed;
		float absLateral;
		float predictionTime;
		float predictionScale;
		float dampedPrediction;
		vec3_t predictedSelf;
		vec3_t predictedOther;
		vec3_t predictedDelta;
		float predictedAhead;
		float predictedLateral;
		float predictedAbsLateral;
		float otherForwardSpeed;

		if ( i == bs->client ) {
			continue;
		}
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}

		otherEnt = &g_entities[i];
		if ( !otherEnt->inuse || !otherEnt->client || otherEnt->health <= 0 ) {
			continue;
		}

		VectorSubtract( otherEnt->client->ps.origin, bs->cur_ps.origin, toOther );
		ahead = DotProduct( toOther, routeForward );
		lateral = DotProduct( toOther, routeRight );
		otherForwardSpeed = DotProduct( otherEnt->client->ps.velocity, routeForward );
		relSpeed = myForwardSpeed - otherForwardSpeed;
		absLateral = fabs( lateral );

		if ( ahead > -40.0f && ahead < risk->nearestAheadDist && absLateral < 120.0f ) {
			risk->nearestAheadDist = ahead;
			risk->nearestAheadRelSpeed = relSpeed;
			risk->nearestAheadLateral = lateral;
		}
		if ( ahead < 40.0f && -ahead < risk->nearestBehindDist && absLateral < 140.0f ) {
			risk->nearestBehindDist = -ahead;
			risk->nearestBehindRelSpeed = otherForwardSpeed - myForwardSpeed;
			risk->nearestBehindLateral = lateral;
		}

		predictionScale = fabs( ahead ) / 240.0f;
		if ( predictionScale < 0.0f ) {
			predictionScale = 0.0f;
		} else if ( predictionScale > 1.0f ) {
			predictionScale = 1.0f;
		}
		predictionTime = minPredictionSec + ( maxPredictionSec - minPredictionSec ) * predictionScale;
		dampedPrediction = predictionTime * 0.82f;

		VectorMA( bs->cur_ps.origin, dampedPrediction, bs->cur_ps.velocity, predictedSelf );
		VectorMA( otherEnt->client->ps.origin, dampedPrediction, otherEnt->client->ps.velocity, predictedOther );
		VectorSubtract( predictedOther, predictedSelf, predictedDelta );
		predictedAhead = DotProduct( predictedDelta, routeForward );
		predictedLateral = DotProduct( predictedDelta, routeRight );
		predictedAbsLateral = fabs( predictedLateral );

		if ( predictedAhead > -30.0f && predictedAhead < 150.0f ) {
			if ( predictedLateral < 0.0f && -predictedLateral < risk->sideSafetyInside ) {
				risk->sideSafetyInside = -predictedLateral;
			}
			if ( predictedLateral > 0.0f && predictedLateral < risk->sideSafetyOutside ) {
				risk->sideSafetyOutside = predictedLateral;
			}
		}

		if ( predictedAhead > -35.0f && predictedAhead < 130.0f && predictedAbsLateral < 92.0f ) {
			risk->hasPredictedConflict = qtrue;

			if ( relSpeed > 45.0f ) {
				risk->recommendedSpeedBias -= 55.0f;
			} else {
				risk->recommendedSpeedBias -= 35.0f;
			}

			if ( predictedAbsLateral < 70.0f || predictedAhead < 30.0f ) {
				risk->abortOvertakeRecommended = qtrue;
			}
		}
	}

	if ( risk->sideSafetyInside + 8.0f < risk->sideSafetyOutside ) {
		risk->laneSwapRecommended = qtrue;
	}
}

/*
==================
BotResetNodeSwitches
==================
*/
void BotResetNodeSwitches(void) {
	numnodeswitches = 0;
}

/*
==================
BotDumpNodeSwitches
==================
*/
void BotDumpNodeSwitches(bot_state_t *bs) {
	int i;
	char netname[MAX_NETNAME];

	ClientName(bs->client, netname, sizeof(netname));
	BotAI_Print(PRT_MESSAGE, "%s at %1.1f switched more than %d AI nodes\n", netname, FloatTime(), MAX_NODESWITCHES);
	for (i = 0; i < numnodeswitches; i++) {
		BotAI_Print(PRT_MESSAGE, "%s", nodeswitch[i]);
	}
	BotAI_Print(PRT_FATAL, "");
}

/*
==================
BotRecordNodeSwitch
==================
*/
void BotRecordNodeSwitch(bot_state_t *bs, char *node, char *str, char *s) {
	char netname[MAX_NETNAME];

	ClientName(bs->client, netname, sizeof(netname));
	Com_sprintf(nodeswitch[numnodeswitches], 144, "%s at %2.1f entered %s: %s from %s\n", netname, FloatTime(), node, str, s);
#ifdef DEBUG
	if (0) {
		BotAI_Print(PRT_MESSAGE, "%s", nodeswitch[numnodeswitches]);
	}
#endif //DEBUG
	numnodeswitches++;
}

/*
==================
BotGetAirGoal
==================
*/
int BotGetAirGoal(bot_state_t *bs, bot_goal_t *goal) {
	bsp_trace_t bsptrace;
	vec3_t end, mins = {-15, -15, -2}, maxs = {15, 15, 2};
	int areanum;

	//trace up until we hit solid
	VectorCopy(bs->origin, end);
	end[2] += 1000;
	BotAI_Trace(&bsptrace, bs->origin, mins, maxs, end, bs->entitynum, CONTENTS_SOLID|CONTENTS_PLAYERCLIP);
	//trace down until we hit water
	VectorCopy(bsptrace.endpos, end);
	BotAI_Trace(&bsptrace, end, mins, maxs, bs->origin, bs->entitynum, CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA);
	//if we found the water surface
	if (bsptrace.fraction > 0) {
		areanum = BotPointAreaNum(bsptrace.endpos);
		if (areanum) {
			VectorCopy(bsptrace.endpos, goal->origin);
			goal->origin[2] -= 2;
			goal->areanum = areanum;
			goal->mins[0] = -15;
			goal->mins[1] = -15;
			goal->mins[2] = -1;
			goal->maxs[0] = 15;
			goal->maxs[1] = 15;
			goal->maxs[2] = 1;
			goal->flags = GFL_AIR;
			goal->number = 0;
			goal->iteminfo = 0;
			goal->entitynum = 0;
			return qtrue;
		}
	}
	return qfalse;
}

/*
==================
BotGoForAir
==================
*/
int BotGoForAir(bot_state_t *bs, int tfl, bot_goal_t *ltg, float range) {
	bot_goal_t goal;

	//if the bot needs air
	if (bs->lastair_time < FloatTime() - 6) {
		//
#ifdef DEBUG
		//BotAI_Print(PRT_MESSAGE, "going for air\n");
#endif //DEBUG
		//if we can find an air goal
		if (BotGetAirGoal(bs, &goal)) {
			trap_BotPushGoal(bs->gs, &goal);
			return qtrue;
		}
		else {
			//get a nearby goal outside the water
			while(trap_BotChooseNBGItem(bs->gs, bs->origin, bs->inventory, tfl, ltg, range)) {
				trap_BotGetTopGoal(bs->gs, &goal);
				//if the goal is not in water
				if (!(trap_AAS_PointContents(goal.origin) & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))) {
					return qtrue;
				}
				trap_BotPopGoal(bs->gs);
			}
			trap_BotResetAvoidGoals(bs->gs);
		}
	}
	return qfalse;
}

/*
==================
BotNearbyGoal
==================
*/
int BotNearbyGoal(bot_state_t *bs, int tfl, bot_goal_t *ltg, float range) {
	int ret;
	float lcsThreatProximity = 0.0f;
	int lcsOpponents = 0;

	//check if the bot should go for air
	if (BotGoForAir(bs, tfl, ltg, range)) return qtrue;
	// if the bot is carrying a flag or cubes
	if (BotCTFCarryingFlag(bs)
#ifdef MISSIONPACK
		|| Bot1FCTFCarryingFlag(bs) || BotHarvesterCarryingCubes(bs)
#endif
		) {
		//if the bot is just a few secs away from the base 
		if (trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin,
				bs->teamgoal.areanum, TFL_DEFAULT) < 300) {
			//make the range really small
			range = 50;
		}
	}
	if ( BotGetLcsRiskMetrics(bs, NULL, &lcsThreatProximity, &lcsOpponents) ) {
		if ( bs->inventory[INVENTORY_HEALTH] < 80 || bs->inventory[INVENTORY_ARMOR] < 60 || lcsThreatProximity > 0.50f ) {
			if ( range < 260 ) {
				range = 260;
			}
		} else {
			range *= 0.55f;
		}
		if ( lcsOpponents > 2 && range > 220 ) {
			range = 220;
		}
	}
	//
	ret = trap_BotChooseNBGItem(bs->gs, bs->origin, bs->inventory, tfl, ltg, range);
	/*
	if (ret)
	{
		char buf[128];
		//get the goal at the top of the stack
		trap_BotGetTopGoal(bs->gs, &goal);
		trap_BotGoalName(goal.number, buf, sizeof(buf));
		BotAI_Print(PRT_MESSAGE, "%1.1f: new nearby goal %s\n", FloatTime(), buf);
	}
    */
	return ret;
}

/*
==================
BotReachedGoal
==================
*/
int BotReachedGoal(bot_state_t *bs, bot_goal_t *goal) {
	if (goal->flags & GFL_ITEM) {
		//if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal)) {
			if (!(goal->flags & GFL_DROPPED)) {
				trap_BotSetAvoidGoalTime(bs->gs, goal->number, -1);
			}
			return qtrue;
		}
		//if the goal isn't there
// Q3Rally Code Start
//		if (trap_BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->viewangles, goal)) {
		if (trap_BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, goal)) {
// END
			/*
			float avoidtime;
			int t;

			avoidtime = trap_BotAvoidGoalTime(bs->gs, goal->number);
			if (avoidtime > 0) {
				t = trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, goal->areanum, bs->tfl);
				if ((float) t * 0.009 < avoidtime)
					return qtrue;
			}
			*/
			return qtrue;
		}
		//if in the goal area and below or above the goal and not swimming
		if (bs->areanum == goal->areanum) {
			if (bs->origin[0] > goal->origin[0] + goal->mins[0] && bs->origin[0] < goal->origin[0] + goal->maxs[0]) {
				if (bs->origin[1] > goal->origin[1] + goal->mins[1] && bs->origin[1] < goal->origin[1] + goal->maxs[1]) {
					if (!trap_AAS_Swimming(bs->origin)) {
						return qtrue;
					}
				}
			}
		}
	}
	else if (goal->flags & GFL_AIR) {
		//if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal)) return qtrue;
		//if the bot got air
		if (bs->lastair_time > FloatTime() - 1) return qtrue;
	}
	else {
		//if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal)) return qtrue;
	}
	return qfalse;
}

/*
==================
BotGetItemLongTermGoal
==================
*/
int BotGetItemLongTermGoal(bot_state_t *bs, int tfl, bot_goal_t *goal) {
	//if the bot has no goal
	if (!trap_BotGetTopGoal(bs->gs, goal)) {
		//BotAI_Print(PRT_MESSAGE, "no ltg on stack\n");
		bs->ltg_time = 0;
	}
	//if the bot touches the current goal
	else if (BotReachedGoal(bs, goal)) {
		BotChooseWeapon(bs);
		bs->ltg_time = 0;
	}
	//if it is time to find a new long term goal
	if (bs->ltg_time < FloatTime()) {
		//pop the current goal from the stack
		trap_BotPopGoal(bs->gs);
		//BotAI_Print(PRT_MESSAGE, "%s: choosing new ltg\n", ClientName(bs->client, netname, sizeof(netname)));
		//choose a new goal
		//BotAI_Print(PRT_MESSAGE, "%6.1f client %d: BotChooseLTGItem\n", FloatTime(), bs->client);
		if (trap_BotChooseLTGItem(bs->gs, bs->origin, bs->inventory, tfl)) {
			/*
			char buf[128];
			//get the goal at the top of the stack
			trap_BotGetTopGoal(bs->gs, goal);
			trap_BotGoalName(goal->number, buf, sizeof(buf));
			BotAI_Print(PRT_MESSAGE, "%1.1f: new long term goal %s\n", FloatTime(), buf);
            */
			bs->ltg_time = FloatTime() + 20;
		}
		else {//the bot gets sorta stuck with all the avoid timings, shouldn't happen though
			//
#ifdef DEBUG
			char netname[128];

			BotAI_Print(PRT_MESSAGE, "%s: no valid ltg (probably stuck)\n", ClientName(bs->client, netname, sizeof(netname)));
#endif
			//trap_BotDumpAvoidGoals(bs->gs);
			//reset the avoid goals and the avoid reach
			trap_BotResetAvoidGoals(bs->gs);
			trap_BotResetAvoidReach(bs->ms);
		}
		//get the goal at the top of the stack
		return trap_BotGetTopGoal(bs->gs, goal);
	}
	return qtrue;
}

/*
==================
BotGetLongTermGoal

we could also create a separate AI node for every long term goal type
however this saves us a lot of code
==================
*/
int BotGetLongTermGoal(bot_state_t *bs, int tfl, int retreat, bot_goal_t *goal) {
	vec3_t target, dir, dir2;
	char netname[MAX_NETNAME];
	char buf[MAX_MESSAGE_SIZE];
	int areanum;
	float croucher;
	aas_entityinfo_t entinfo, botinfo;
	bot_waypoint_t *wp;

	if (bs->ltgtype == LTG_TEAMHELP && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "help_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
			trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			bs->teammessage_time = 0;
		}
		//if trying to help the team mate for more than a minute
		if (bs->teamgoal_time < FloatTime())
			bs->ltgtype = 0;
		//if the team mate IS visible for quite some time
		if (bs->teammatevisible_time < FloatTime() - 10) bs->ltgtype = 0;
		//get entity information of the companion
		BotEntityInfo(bs->teammate, &entinfo);
		//if the team mate is visible
// Q3Rally Code Start
//		if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->teammate)) {
		if (BotEntityVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, 360, bs->teammate)) {
// END
			//if close just stand still there
			VectorSubtract(entinfo.origin, bs->origin, dir);
			if (VectorLengthSquared(dir) < Square(100)) {
				trap_BotResetAvoidReach(bs->ms);
				return qfalse;
			}
		}
		else {
			//last time the bot was NOT visible
			bs->teammatevisible_time = FloatTime();
		}
		//if the entity information is valid (entity in PVS)
		if (entinfo.valid) {
			areanum = BotPointAreaNum(entinfo.origin);
			if (areanum && trap_AAS_AreaReachability(areanum)) {
				//update team goal
				bs->teamgoal.entitynum = bs->teammate;
				bs->teamgoal.areanum = areanum;
				VectorCopy(entinfo.origin, bs->teamgoal.origin);
				VectorSet(bs->teamgoal.mins, -8, -8, -8);
				VectorSet(bs->teamgoal.maxs, 8, 8, 8);
			}
		}
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		return qtrue;
	}
	//if the bot accompanies someone
	if (bs->ltgtype == LTG_TEAMACCOMPANY && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "accompany_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
			trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			bs->teammessage_time = 0;
		}
		//if accompanying the companion for 3 minutes
		if (bs->teamgoal_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "accompany_stop", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
			bs->ltgtype = 0;
		}
		//get entity information of the companion
		BotEntityInfo(bs->teammate, &entinfo);
		//if the companion is visible
// Q3Rally Code Start
//		if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->teammate)) {
		if (BotEntityVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, 360, bs->teammate)) {
// END
			//update visible time
			bs->teammatevisible_time = FloatTime();
			VectorSubtract(entinfo.origin, bs->origin, dir);
			if (VectorLengthSquared(dir) < Square(bs->formation_dist)) {
				//
				// if the client being followed bumps into this bot then
				// the bot should back up
				BotEntityInfo(bs->entitynum, &botinfo);
				// if the followed client is not standing ontop of the bot
				if (botinfo.origin[2] + botinfo.maxs[2] > entinfo.origin[2] + entinfo.mins[2]) {
					// if the bounding boxes touch each other
					if (botinfo.origin[0] + botinfo.maxs[0] > entinfo.origin[0] + entinfo.mins[0] - 4&&
						botinfo.origin[0] + botinfo.mins[0] < entinfo.origin[0] + entinfo.maxs[0] + 4) {
						if (botinfo.origin[1] + botinfo.maxs[1] > entinfo.origin[1] + entinfo.mins[1] - 4 &&
							botinfo.origin[1] + botinfo.mins[1] < entinfo.origin[1] + entinfo.maxs[1] + 4) {
							if (botinfo.origin[2] + botinfo.maxs[2] > entinfo.origin[2] + entinfo.mins[2] - 4 &&
								botinfo.origin[2] + botinfo.mins[2] < entinfo.origin[2] + entinfo.maxs[2] + 4) {
								// if the followed client looks in the direction of this bot
								AngleVectors(entinfo.angles, dir, NULL, NULL);
								dir[2] = 0;
								VectorNormalize(dir);
								//VectorSubtract(entinfo.origin, entinfo.lastvisorigin, dir);
								VectorSubtract(bs->origin, entinfo.origin, dir2);
								VectorNormalize(dir2);
								if (DotProduct(dir, dir2) > 0.7) {
									// back up
									BotSetupForMovement(bs);
// Q3Rally Code Start
//									Com_Printf("Backing up\n");
									trap_EA_MoveBack( bs->entitynum );
// END
									trap_BotMoveInDirection(bs->ms, dir2, 400, MOVE_WALK);
								}
							}
						}
					}
				}
				//check if the bot wants to crouch
				//don't crouch if crouched less than 5 seconds ago
				if (bs->attackcrouch_time < FloatTime() - 5) {
					croucher = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_CROUCHER, 0, 1);
					if (random() < bs->thinktime * croucher) {
						bs->attackcrouch_time = FloatTime() + 5 + croucher * 15;
					}
				}
				//don't crouch when swimming
				if (trap_AAS_Swimming(bs->origin)) bs->attackcrouch_time = FloatTime() - 1;
				//if not arrived yet or arived some time ago
				if (bs->arrive_time < FloatTime() - 2) {
					//if not arrived yet
					if (!bs->arrive_time) {
						trap_EA_Gesture(bs->client);
						BotAI_BotInitialChat(bs, "accompany_arrive", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
						trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
						bs->arrive_time = FloatTime();
					}
					//if the bot wants to crouch
					else if (bs->attackcrouch_time > FloatTime()) {
						trap_EA_Crouch(bs->client);
					}
					//else do some model taunts
					else if (random() < bs->thinktime * 0.05) {
						//do a gesture :)
						trap_EA_Gesture(bs->client);
					}
				}
				//if just arrived look at the companion
				if (bs->arrive_time > FloatTime() - 2) {
					VectorSubtract(entinfo.origin, bs->origin, dir);
					vectoangles(dir, bs->ideal_viewangles);
					bs->ideal_viewangles[2] *= 0.5;
				}
				//else look strategically around for enemies
				else if (random() < bs->thinktime * 0.8) {
					BotRoamGoal(bs, target);
					VectorSubtract(target, bs->origin, dir);
					vectoangles(dir, bs->ideal_viewangles);
					bs->ideal_viewangles[2] *= 0.5;
				}
				//check if the bot wants to go for air
				if (BotGoForAir(bs, bs->tfl, &bs->teamgoal, 400)) {
					trap_BotResetLastAvoidReach(bs->ms);
					//get the goal at the top of the stack
					//trap_BotGetTopGoal(bs->gs, &tmpgoal);
					//trap_BotGoalName(tmpgoal.number, buf, 144);
					//BotAI_Print(PRT_MESSAGE, "new nearby goal %s\n", buf);
					//time the bot gets to pick up the nearby goal item
					bs->nbg_time = FloatTime() + 8;
					AIEnter_Seek_NBG(bs, "BotLongTermGoal: go for air");
					return qfalse;
				}
				//
				trap_BotResetAvoidReach(bs->ms);
				return qfalse;
			}
		}
		//if the entity information is valid (entity in PVS)
		if (entinfo.valid) {
			areanum = BotPointAreaNum(entinfo.origin);
			if (areanum && trap_AAS_AreaReachability(areanum)) {
				//update team goal
				bs->teamgoal.entitynum = bs->teammate;
				bs->teamgoal.areanum = areanum;
				VectorCopy(entinfo.origin, bs->teamgoal.origin);
				VectorSet(bs->teamgoal.mins, -8, -8, -8);
				VectorSet(bs->teamgoal.maxs, 8, 8, 8);
			}
		}
		//the goal the bot should go for
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		//if the companion is NOT visible for too long
		if (bs->teammatevisible_time < FloatTime() - 60) {
			BotAI_BotInitialChat(bs, "accompany_cannotfind", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
			bs->ltgtype = 0;
			// just to make sure the bot won't spam this message
			bs->teammatevisible_time = FloatTime();
		}
		return qtrue;
	}
	//
	if (bs->ltgtype == LTG_DEFENDKEYAREA) {
		if (trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin,
				bs->teamgoal.areanum, TFL_DEFAULT) > bs->defendaway_range) {
			bs->defendaway_time = 0;
		}
	}
	//if defending a key area
	if (bs->ltgtype == LTG_DEFENDKEYAREA && !retreat &&
				bs->defendaway_time < FloatTime()) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "defend_start", buf, NULL);
			trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
			BotVoiceChatOnly(bs, -1, VOICECHAT_ONDEFENSE);
			bs->teammessage_time = 0;
		}
		//set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		//stop after 2 minutes
		if (bs->teamgoal_time < FloatTime()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "defend_stop", buf, NULL);
			trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
			bs->ltgtype = 0;
		}
		//if very close... go away for some time
		VectorSubtract(goal->origin, bs->origin, dir);
		if (VectorLengthSquared(dir) < Square(70)) {
			trap_BotResetAvoidReach(bs->ms);
			bs->defendaway_time = FloatTime() + 3 + 3 * random();
			if (BotHasPersistantPowerupAndWeapon(bs)) {
				bs->defendaway_range = 100;
			}
			else {
				bs->defendaway_range = 350;
			}
		}
		return qtrue;
	}
	//going to kill someone
	if (bs->ltgtype == LTG_KILL && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			EasyClientName(bs->teamgoal.entitynum, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "kill_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->teammessage_time = 0;
		}
		//
		if (bs->killedenemy_time > bs->teamgoal_time - TEAM_KILL_SOMEONE && bs->lastkilledplayer == bs->teamgoal.entitynum) {
			EasyClientName(bs->teamgoal.entitynum, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "kill_done", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->ltgtype = 0;
		}
		//
		if (bs->teamgoal_time < FloatTime()) {
			bs->ltgtype = 0;
		}
		//just roam around
		return BotGetItemLongTermGoal(bs, tfl, goal);
	}
	//get an item
	if (bs->ltgtype == LTG_GETITEM && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "getitem_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
			trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			bs->teammessage_time = 0;
		}
		//set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		//stop after some time
		if (bs->teamgoal_time < FloatTime()) {
			bs->ltgtype = 0;
		}
		//
// Q3Rally Code Start
//		if (trap_BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->viewangles, goal)) {
		if (trap_BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, goal)) {
// END
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "getitem_notthere", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->ltgtype = 0;
		}
		else if (BotReachedGoal(bs, goal)) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "getitem_gotit", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->ltgtype = 0;
		}
		return qtrue;
	}
	//if camping somewhere
	if ((bs->ltgtype == LTG_CAMP || bs->ltgtype == LTG_CAMPORDER) && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			if (bs->ltgtype == LTG_CAMPORDER) {
				BotAI_BotInitialChat(bs, "camp_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
				trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
				BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
				trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			}
			bs->teammessage_time = 0;
		}
		//set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		//
		if (bs->teamgoal_time < FloatTime()) {
			if (bs->ltgtype == LTG_CAMPORDER) {
				BotAI_BotInitialChat(bs, "camp_stop", NULL);
				trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			}
			bs->ltgtype = 0;
		}
		//if really near the camp spot
		VectorSubtract(goal->origin, bs->origin, dir);
		if (VectorLengthSquared(dir) < Square(60))
		{
			//if not arrived yet
			if (!bs->arrive_time) {
				if (bs->ltgtype == LTG_CAMPORDER) {
					BotAI_BotInitialChat(bs, "camp_arrive", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
					trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
					BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_INPOSITION);
				}
				bs->arrive_time = FloatTime();
			}
			//look strategically around for enemies
			if (random() < bs->thinktime * 0.8) {
				BotRoamGoal(bs, target);
				VectorSubtract(target, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
				bs->ideal_viewangles[2] *= 0.5;
			}
			//check if the bot wants to crouch
			//don't crouch if crouched less than 5 seconds ago
			if (bs->attackcrouch_time < FloatTime() - 5) {
				croucher = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_CROUCHER, 0, 1);
				if (random() < bs->thinktime * croucher) {
					bs->attackcrouch_time = FloatTime() + 5 + croucher * 15;
				}
			}
			//if the bot wants to crouch
			if (bs->attackcrouch_time > FloatTime()) {
				trap_EA_Crouch(bs->client);
			}
			//don't crouch when swimming
			if (trap_AAS_Swimming(bs->origin)) bs->attackcrouch_time = FloatTime() - 1;
			//make sure the bot is not gonna drown
			if (trap_PointContents(bs->eye,bs->entitynum) & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA)) {
				if (bs->ltgtype == LTG_CAMPORDER) {
					BotAI_BotInitialChat(bs, "camp_stop", NULL);
					trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
					//
					if (bs->lastgoal_ltgtype == LTG_CAMPORDER) {
						bs->lastgoal_ltgtype = 0;
					}
				}
				bs->ltgtype = 0;
			}
			//
			//FIXME: move around a bit
			//
			trap_BotResetAvoidReach(bs->ms);
			return qfalse;
		}
		return qtrue;
	}
	//patrolling along several waypoints
	if (bs->ltgtype == LTG_PATROL && !retreat) {
		//check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			strcpy(buf, "");
			for (wp = bs->patrolpoints; wp; wp = wp->next) {
				strcat(buf, wp->name);
				if (wp->next) strcat(buf, " to ");
			}
			BotAI_BotInitialChat(bs, "patrol_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
			trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			bs->teammessage_time = 0;
		}
		//
		if (!bs->curpatrolpoint) {
			bs->ltgtype = 0;
			return qfalse;
		}
		//if the bot touches the current goal
		if (trap_BotTouchingGoal(bs->origin, &bs->curpatrolpoint->goal)) {
			if (bs->patrolflags & PATROL_BACK) {
				if (bs->curpatrolpoint->prev) {
					bs->curpatrolpoint = bs->curpatrolpoint->prev;
				}
				else {
					bs->curpatrolpoint = bs->curpatrolpoint->next;
					bs->patrolflags &= ~PATROL_BACK;
				}
			}
			else {
				if (bs->curpatrolpoint->next) {
					bs->curpatrolpoint = bs->curpatrolpoint->next;
				}
				else {
					bs->curpatrolpoint = bs->curpatrolpoint->prev;
					bs->patrolflags |= PATROL_BACK;
				}
			}
		}
		//stop after 5 minutes
		if (bs->teamgoal_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "patrol_stop", NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->ltgtype = 0;
		}
		if (!bs->curpatrolpoint) {
			bs->ltgtype = 0;
			return qfalse;
		}
		memcpy(goal, &bs->curpatrolpoint->goal, sizeof(bot_goal_t));
		return qtrue;
	}
// Q3Rally Code Start
	if( isRallyRace() )
	{
		if (bs->ltgtype == LTG_WINRACE) {
			AIEnter_MoveToNextCheckpoint( bs, "BotGetLongTermGoal" );
/*
			gentity_t	*checkpoint = NULL;
			int num;


			num = trap_BotGetLevelItemGoal(-1, "SP_info_player_deathmatch", goal);
			while( num >= 0 )
			{
				Com_Printf( "Found checkpoint %i\n", g_entities[num].number );
				if( g_entities[num].number == g_entities[bs->client].number )
				{
					Com_Printf( "Found next checkpoint\n" );
					break;
				}

				num = trap_BotGetLevelItemGoal( num, "SP_info_player_deathmatch", goal );
			}


//			Com_Printf( "Win the race!, next checkpoint is %i\n", g_entities[bs->client].number );

			while ((checkpoint = G_Find (checkpoint, FOFS(classname), "rally_checkpoint")) != NULL)
			{
				if ( checkpoint->number == g_entities[bs->client].number )
				{
					break;
				}
			}

			if ( !checkpoint )
				return qfalse;

//			Com_Printf( "Found checkpoint\n" );

			goal->areanum = trap_AAS_PointAreaNum( checkpoint->s.origin );
			goal->entitynum = checkpoint->s.number;
			VectorCopy( checkpoint->r.mins, goal->mins );
			VectorCopy( checkpoint->r.maxs, goal->maxs );
			VectorCopy( checkpoint->s.origin, goal->origin );
			goal->number = 10;
			goal->iteminfo = 0;
			goal->flags = 0;
*/
			return qtrue;
		}

		return qfalse;
	}
// END
#ifdef CTF
	if (gametype == GT_CTF) {
		//if going for enemy flag
		if (bs->ltgtype == LTG_GETFLAG) {
			//check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "captureflag_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONGETFLAG);
				bs->teammessage_time = 0;
			}
			//
			switch(BotTeam(bs)) {
				case TEAM_RED: memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t)); break;
				case TEAM_BLUE: memcpy(goal, &ctf_redflag, sizeof(bot_goal_t)); break;
				default: bs->ltgtype = 0; return qfalse;
			}
			//if touching the flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				// make sure the bot knows the flag isn't there anymore
				switch(BotTeam(bs)) {
					case TEAM_RED: bs->blueflagstatus = 1; break;
					case TEAM_BLUE: bs->redflagstatus = 1; break;
				}
				bs->ltgtype = 0;
			}
			//stop after 3 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			BotAlternateRoute(bs, goal);
			return qtrue;
		}
		//if rushing to the base
		if (bs->ltgtype == LTG_RUSHBASE && bs->rushbaseaway_time < FloatTime()) {
			switch(BotTeam(bs)) {
				case TEAM_RED: memcpy(goal, &ctf_redflag, sizeof(bot_goal_t)); break;
				case TEAM_BLUE: memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t)); break;
				default: bs->ltgtype = 0; return qfalse;
			}
			//if not carrying the flag anymore
			if (!BotCTFCarryingFlag(bs)) bs->ltgtype = 0;
			//quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) bs->ltgtype = 0;
			//if touching the base flag the bot should loose the enemy flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				//if the bot is still carrying the enemy flag then the
				//base flag is gone, now just walk near the base a bit
				if (BotCTFCarryingFlag(bs)) {
					trap_BotResetAvoidReach(bs->ms);
					bs->rushbaseaway_time = FloatTime() + 5 + 10 * random();
					//FIXME: add chat to tell the others to get back the flag
				}
				else {
					bs->ltgtype = 0;
				}
			}
			BotAlternateRoute(bs, goal);
			return qtrue;
		}
		//returning flag
		if (bs->ltgtype == LTG_RETURNFLAG) {
			//check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "returnflag_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONRETURNFLAG);
				bs->teammessage_time = 0;
			}
			//
			switch(BotTeam(bs)) {
				case TEAM_RED: memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t)); break;
				case TEAM_BLUE: memcpy(goal, &ctf_redflag, sizeof(bot_goal_t)); break;
				default: bs->ltgtype = 0; return qfalse;
			}
			//if touching the flag
			if (trap_BotTouchingGoal(bs->origin, goal)) bs->ltgtype = 0;
			//stop after 3 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			BotAlternateRoute(bs, goal);
			return qtrue;
		}
	}
#endif //CTF
#ifdef MISSIONPACK
	else if (gametype == GT_1FCTF) {
		if (bs->ltgtype == LTG_GETFLAG) {
			//check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "captureflag_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONGETFLAG);
				bs->teammessage_time = 0;
			}
			memcpy(goal, &ctf_neutralflag, sizeof(bot_goal_t));
			//if touching the flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->ltgtype = 0;
			}
			//stop after 3 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			return qtrue;
		}
		//if rushing to the base
		if (bs->ltgtype == LTG_RUSHBASE) {
			switch(BotTeam(bs)) {
				case TEAM_RED: memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t)); break;
				case TEAM_BLUE: memcpy(goal, &ctf_redflag, sizeof(bot_goal_t)); break;
				default: bs->ltgtype = 0; return qfalse;
			}
			//if not carrying the flag anymore
			if (!Bot1FCTFCarryingFlag(bs)) {
				bs->ltgtype = 0;
			}
			//quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			//if touching the base flag the bot should loose the enemy flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->ltgtype = 0;
			}
			BotAlternateRoute(bs, goal);
			return qtrue;
		}
		//attack the enemy base
		if (bs->ltgtype == LTG_ATTACKENEMYBASE &&
				bs->attackaway_time < FloatTime()) {
			//check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "attackenemybase_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONOFFENSE);
				bs->teammessage_time = 0;
			}
			switch(BotTeam(bs)) {
				case TEAM_RED: memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t)); break;
				case TEAM_BLUE: memcpy(goal, &ctf_redflag, sizeof(bot_goal_t)); break;
				default: bs->ltgtype = 0; return qfalse;
			}
			//quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			//if touching the base flag the bot should loose the enemy flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->attackaway_time = FloatTime() + 2 + 5 * random();
			}
			return qtrue;
		}
		//returning flag
		if (bs->ltgtype == LTG_RETURNFLAG) {
			//check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "returnflag_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONRETURNFLAG);
				bs->teammessage_time = 0;
			}
			//
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			//just roam around
			return BotGetItemLongTermGoal(bs, tfl, goal);
		}
	}
	else if (gametype == GT_OBELISK) {
		if (bs->ltgtype == LTG_ATTACKENEMYBASE &&
				bs->attackaway_time < FloatTime()) {

			//check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "attackenemybase_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONOFFENSE);
				bs->teammessage_time = 0;
			}
			switch(BotTeam(bs)) {
				case TEAM_RED: memcpy(goal, &blueobelisk, sizeof(bot_goal_t)); break;
				case TEAM_BLUE: memcpy(goal, &redobelisk, sizeof(bot_goal_t)); break;
				default: bs->ltgtype = 0; return qfalse;
			}
			//if the bot no longer wants to attack the obelisk
			if (BotFeelingBad(bs) > 50) {
				return BotGetItemLongTermGoal(bs, tfl, goal);
			}
			//if touching the obelisk
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->attackaway_time = FloatTime() + 3 + 5 * random();
			}
			// or very close to the obelisk
			VectorSubtract(bs->origin, goal->origin, dir);
			if (VectorLengthSquared(dir) < Square(60)) {
				bs->attackaway_time = FloatTime() + 3 + 5 * random();
			}
			//quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			BotAlternateRoute(bs, goal);
			//just move towards the obelisk
			return qtrue;
		}
	}
	else if (gametype == GT_HARVESTER) {
		//if rushing to the base
		if (bs->ltgtype == LTG_RUSHBASE) {
			switch(BotTeam(bs)) {
				case TEAM_RED: memcpy(goal, &blueobelisk, sizeof(bot_goal_t)); break;
				case TEAM_BLUE: memcpy(goal, &redobelisk, sizeof(bot_goal_t)); break;
				default: BotGoHarvest(bs); return qfalse;
			}
			//if not carrying any cubes
			if (!BotHarvesterCarryingCubes(bs)) {
				BotGoHarvest(bs);
				return qfalse;
			}
			//quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				BotGoHarvest(bs);
				return qfalse;
			}
			//if touching the base flag the bot should loose the enemy flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				BotGoHarvest(bs);
				return qfalse;
			}
			BotAlternateRoute(bs, goal);
			return qtrue;
		}
		//attack the enemy base
		if (bs->ltgtype == LTG_ATTACKENEMYBASE &&
				bs->attackaway_time < FloatTime()) {
			//check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "attackenemybase_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONOFFENSE);
				bs->teammessage_time = 0;
			}
			switch(BotTeam(bs)) {
				case TEAM_RED: memcpy(goal, &blueobelisk, sizeof(bot_goal_t)); break;
				case TEAM_BLUE: memcpy(goal, &redobelisk, sizeof(bot_goal_t)); break;
				default: bs->ltgtype = 0; return qfalse;
			}
			//quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			//if touching the base flag the bot should loose the enemy flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->attackaway_time = FloatTime() + 2 + 5 * random();
			}
			return qtrue;
		}
		//harvest cubes
		if (bs->ltgtype == LTG_HARVEST &&
			bs->harvestaway_time < FloatTime()) {
			//check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "harvest_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONOFFENSE);
				bs->teammessage_time = 0;
			}
			memcpy(goal, &neutralobelisk, sizeof(bot_goal_t));
			//
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			//
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->harvestaway_time = FloatTime() + 4 + 3 * random();
			}
			return qtrue;
		}
	}
#endif
	if (gametype == GT_DOMINATION) {
		int sigilStatus = SIGIL_NONE;
		int team = BotTeam(bs);
		qboolean lowHealth = (bs->inventory[INVENTORY_HEALTH] > 0 && bs->inventory[INVENTORY_HEALTH] < 40) ? qtrue : qfalse;

		if ((bs->ltgtype == LTG_GETFLAG || bs->ltgtype == LTG_ATTACKENEMYBASE || bs->ltgtype == LTG_DEFENDKEYAREA) &&
			BotGetDominationSigilGoal(bs, goal, &sigilStatus)) {
			if (lowHealth && BotFindEnemy(bs, -1)) {
				return BotGetItemLongTermGoal(bs, tfl, goal);
			}

			if (sigilStatus == SIGIL_ISWHITE) {
				bs->ltgtype = LTG_GETFLAG;
			}
			else if ((team == TEAM_RED && sigilStatus == SIGIL_ISBLUE) ||
				(team == TEAM_BLUE && sigilStatus == SIGIL_ISRED)) {
				bs->ltgtype = LTG_ATTACKENEMYBASE;
			}
			else {
				bs->ltgtype = LTG_DEFENDKEYAREA;
			}

			BotAlternateRoute(bs, goal);
			return qtrue;
		}
	}
	else if (gametype == GT_KOTH) {
		int owner, contested, capturePct;
		float hillRadius;
		float radiusExtent;
		vec3_t hillOrigin;
		qboolean lowHealth = (bs->inventory[INVENTORY_HEALTH] > 0 && bs->inventory[INVENTORY_HEALTH] < 40) ? qtrue : qfalse;

		if ((bs->ltgtype == LTG_ATTACKENEMYBASE || bs->ltgtype == LTG_DEFENDKEYAREA || bs->ltgtype == LTG_GETFLAG)
			&& BotGetKOTHStatus(&owner, &contested, &capturePct, hillOrigin, &hillRadius)) {
			if (lowHealth && owner != BotTeam(bs)) {
				return BotGetItemLongTermGoal(bs, tfl, goal);
			}

			radiusExtent = (hillRadius > 32.0f) ? hillRadius : 128.0f;
			goal->entitynum = ENTITYNUM_NONE;
			goal->areanum = BotPointAreaNum(hillOrigin);
			VectorSet(goal->mins, -radiusExtent, -radiusExtent, -24);
			VectorSet(goal->maxs, radiusExtent, radiusExtent, 48);
			VectorCopy(hillOrigin, goal->origin);
			goal->flags = 0;
			goal->number = 0;
			goal->iteminfo = 0;

			if (owner == BotTeam(bs) && !contested && capturePct >= 100) {
				bs->ltgtype = LTG_DEFENDKEYAREA;
			}
			else {
				bs->ltgtype = LTG_ATTACKENEMYBASE;
			}
			return qtrue;
		}
	}
	//normal goal stuff
	return BotGetItemLongTermGoal(bs, tfl, goal);
}

/*
==================
BotLongTermGoal
==================
*/
int BotLongTermGoal(bot_state_t *bs, int tfl, int retreat, bot_goal_t *goal) {
	aas_entityinfo_t entinfo;
	char teammate[MAX_MESSAGE_SIZE];
	float squaredist;
	int areanum;
	vec3_t dir;

// Q3Rally Code Start
/*
   if ( gametype == GT_RACING || gametype == GT_RACING_DM || gametype == GT_SPRINT )
	{
		gentity_t	*ent = NULL;
		while ((ent = G_Find (ent, FOFS(classname), "rally_checkpoint")) != NULL) {
			if( ent->number == g_entities[bs->entitynum].number )
				break;
		}

		if( ent->number == g_entities[bs->entitynum].number )
		{
//			Com_Printf( "Setting long term goal to next checkpoint, %i, ent %i\n", ent->number, (ent - g_entities) );
			goal->entitynum = (ent - g_entities);
			VectorCopy( ent->s.origin, goal->origin );
			VectorCopy( ent->r.mins, goal->mins );
			VectorCopy( ent->r.maxs, goal->maxs );
			goal->number = (ent - g_entities);
			goal->areanum = trap_AAS_PointAreaNum( goal->origin );

			trap_BotEmptyGoalStack(bs->gs);
			trap_BotPushGoal(bs->gs, &goal);

			return qtrue;
		}
	}
*/
// END

	//FIXME: also have air long term goals?
	//
	//if the bot is leading someone and not retreating
	if (bs->lead_time > 0 && !retreat) {
		if (bs->lead_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "lead_stop", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
			trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
			bs->lead_time = 0;
			return BotGetLongTermGoal(bs, tfl, retreat, goal);
		}
		//
		if (bs->leadmessage_time < 0 && -bs->leadmessage_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
			trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
			bs->leadmessage_time = FloatTime();
		}
		//get entity information of the companion
		BotEntityInfo(bs->lead_teammate, &entinfo);
		//
		if (entinfo.valid) {
			areanum = BotPointAreaNum(entinfo.origin);
			if (areanum && trap_AAS_AreaReachability(areanum)) {
				//update team goal
				bs->lead_teamgoal.entitynum = bs->lead_teammate;
				bs->lead_teamgoal.areanum = areanum;
				VectorCopy(entinfo.origin, bs->lead_teamgoal.origin);
				VectorSet(bs->lead_teamgoal.mins, -8, -8, -8);
				VectorSet(bs->lead_teamgoal.maxs, 8, 8, 8);
			}
		}
		//if the team mate is visible
// Q3Rally Code Start
//		if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->lead_teammate)) {
		if (BotEntityVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, 360, bs->lead_teammate)) {
// END
			bs->leadvisible_time = FloatTime();
		}
		//if the team mate is not visible for 1 seconds
		if (bs->leadvisible_time < FloatTime() - 1) {
			bs->leadbackup_time = FloatTime() + 2;
		}
		//distance towards the team mate
		VectorSubtract(bs->origin, bs->lead_teamgoal.origin, dir);
		squaredist = VectorLengthSquared(dir);
		//if backing up towards the team mate
		if (bs->leadbackup_time > FloatTime()) {
			if (bs->leadmessage_time < FloatTime() - 20) {
				BotAI_BotInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
				trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
				bs->leadmessage_time = FloatTime();
			}
			//if very close to the team mate
			if (squaredist < Square(100)) {
				bs->leadbackup_time = 0;
			}
			//the bot should go back to the team mate
			memcpy(goal, &bs->lead_teamgoal, sizeof(bot_goal_t));
			return qtrue;
		}
		else {
			//if quite distant from the team mate
			if (squaredist > Square(500)) {
				if (bs->leadmessage_time < FloatTime() - 20) {
					BotAI_BotInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
					trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
					bs->leadmessage_time = FloatTime();
				}
				//look at the team mate
				VectorSubtract(entinfo.origin, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
				bs->ideal_viewangles[2] *= 0.5;
				//just wait for the team mate
				return qfalse;
			}
		}
	}
	return BotGetLongTermGoal(bs, tfl, retreat, goal);
}

/*
==================
AIEnter_Intermission
==================
*/
void AIEnter_Intermission(bot_state_t *bs, char *s) {
	BotRecordNodeSwitch(bs, "intermission", "", s);
	//reset the bot state
	BotResetState(bs);
	//check for end level chat
	if (BotChat_EndLevel(bs)) {
		trap_BotEnterChat(bs->cs, 0, bs->chatto);
	}
	bs->ainode = AINode_Intermission;
}

/*
==================
AINode_Intermission
==================
*/
int AINode_Intermission(bot_state_t *bs) {
// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Intermission\n", bs->client );
// END

	//if the intermission ended
	if (!BotIntermission(bs)) {
		if (BotChat_StartLevel(bs)) {
			bs->stand_time = FloatTime() + BotChatTime(bs);
		}
		else {
			bs->stand_time = FloatTime() + 2;
		}
		AIEnter_Stand(bs, "intermission: chat");
	}
	return qtrue;
}

/*
==================
AIEnter_Observer
==================
*/
void AIEnter_Observer(bot_state_t *bs, char *s) {
	BotRecordNodeSwitch(bs, "observer", "", s);
	//reset the bot state
	BotResetState(bs);
	bs->ainode = AINode_Observer;
}

/*
==================
AINode_Observer
==================
*/
int AINode_Observer(bot_state_t *bs) {
// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Observer\n", bs->client );
// END

	//if the bot left observer mode
	if (!BotIsObserver(bs)) {
		AIEnter_Stand(bs, "observer: left observer");
	}
	return qtrue;
}

/*
==================
AIEnter_Stand
==================
*/
void AIEnter_Stand(bot_state_t *bs, char *s) {
	BotRecordNodeSwitch(bs, "stand", "", s);
	bs->standfindenemy_time = FloatTime() + 1;
	bs->ainode = AINode_Stand;
}

/*
==================
AINode_Stand
==================
*/
int AINode_Stand(bot_state_t *bs) {

// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Stand\n", bs->client );
// END

	//if the bot's health decreased
	if (bs->lastframe_health > bs->inventory[INVENTORY_HEALTH]) {
		if (BotChat_HitTalking(bs)) {
			bs->standfindenemy_time = FloatTime() + BotChatTime(bs) + 0.1;
			bs->stand_time = FloatTime() + BotChatTime(bs) + 0.1;
		}
	}
	if (bs->standfindenemy_time < FloatTime()) {
		if (BotFindEnemy(bs, -1)) {
			AIEnter_Battle_Fight(bs, "stand: found enemy");
			return qfalse;
		}
		bs->standfindenemy_time = FloatTime() + 1;
	}
	// put up chat icon
	trap_EA_Talk(bs->client);
	// when done standing
	if (bs->stand_time < FloatTime()) {
		trap_BotEnterChat(bs->cs, 0, bs->chatto);
		AIEnter_Seek_LTG(bs, "stand: time out");
		return qfalse;
	}
	//
	return qtrue;
}

/*
==================
AIEnter_Respawn
==================
*/
void AIEnter_Respawn(bot_state_t *bs, char *s) {
	BotRecordNodeSwitch(bs, "respawn", "", s);
	//reset some states
	trap_BotResetMoveState(bs->ms);
	trap_BotResetGoalState(bs->gs);
	trap_BotResetAvoidGoals(bs->gs);
	trap_BotResetAvoidReach(bs->ms);
	//if the bot wants to chat
	if (BotChat_Death(bs)) {
		bs->respawn_time = FloatTime() + BotChatTime(bs);
		bs->respawnchat_time = FloatTime();
	}
	else {
		bs->respawn_time = FloatTime() + 1 + random();
		bs->respawnchat_time = 0;
	}
	//set respawn state
	bs->respawn_wait = qfalse;
	bs->ainode = AINode_Respawn;
}

/*
==================
AINode_Respawn
==================
*/
int AINode_Respawn(bot_state_t *bs) {

// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Respawn\n", bs->client );
// END

	// if waiting for the actual respawn
	if (bs->respawn_wait) {
		if (!BotIsDead(bs)) {
			AIEnter_Seek_LTG(bs, "respawn: respawned");
		}
		else {
			trap_EA_Respawn(bs->client);
		}
	}
	else if (bs->respawn_time < FloatTime()) {
		// wait until respawned
		bs->respawn_wait = qtrue;
		// elementary action respawn
		trap_EA_Respawn(bs->client);
		//
		if (bs->respawnchat_time) {
			trap_BotEnterChat(bs->cs, 0, bs->chatto);
			bs->enemy = -1;
		}
	}
	if (bs->respawnchat_time && bs->respawnchat_time < FloatTime() - 0.5) {
		trap_EA_Talk(bs->client);
	}
	//
	return qtrue;
}

/*
==================
BotSelectActivateWeapon
==================
*/
int BotSelectActivateWeapon(bot_state_t *bs) {
	//
	if (bs->inventory[INVENTORY_MACHINEGUN] > 0 && bs->inventory[INVENTORY_BULLETS] > 0)
		return WEAPONINDEX_MACHINEGUN;
	else if (bs->inventory[INVENTORY_SHOTGUN] > 0 && bs->inventory[INVENTORY_SHELLS] > 0)
		return WEAPONINDEX_SHOTGUN;
	else if (bs->inventory[INVENTORY_PLASMAGUN] > 0 && bs->inventory[INVENTORY_CELLS] > 0)
		return WEAPONINDEX_PLASMAGUN;
	else if (bs->inventory[INVENTORY_LIGHTNING] > 0 && bs->inventory[INVENTORY_LIGHTNINGAMMO] > 0)
		return WEAPONINDEX_LIGHTNING;
#ifdef MISSIONPACK
	else if (bs->inventory[INVENTORY_CHAINGUN] > 0 && bs->inventory[INVENTORY_BELT] > 0)
		return WEAPONINDEX_CHAINGUN;
	else if (bs->inventory[INVENTORY_NAILGUN] > 0 && bs->inventory[INVENTORY_NAILS] > 0)
		return WEAPONINDEX_NAILGUN;
	else if (bs->inventory[INVENTORY_PROXLAUNCHER] > 0 && bs->inventory[INVENTORY_MINES] > 0)
		return WEAPONINDEX_PROXLAUNCHER;
#endif
	else if (bs->inventory[INVENTORY_GRENADELAUNCHER] > 0 && bs->inventory[INVENTORY_GRENADES] > 0)
		return WEAPONINDEX_GRENADE_LAUNCHER;
	else if (bs->inventory[INVENTORY_RAILGUN] > 0 && bs->inventory[INVENTORY_SLUGS] > 0)
		return WEAPONINDEX_RAILGUN;
	else if (bs->inventory[INVENTORY_ROCKETLAUNCHER] > 0 && bs->inventory[INVENTORY_ROCKETS] > 0)
		return WEAPONINDEX_ROCKET_LAUNCHER;
	else if (bs->inventory[INVENTORY_BFG10K] > 0 && bs->inventory[INVENTORY_BFGAMMO] > 0)
		return WEAPONINDEX_BFG;
	else {
		return -1;
	}
}

/*
==================
BotClearPath

 try to deactivate obstacles like proximity mines on the bot's path
==================
*/
void BotClearPath(bot_state_t *bs, bot_moveresult_t *moveresult) {
	int i, bestmine;
	float dist, bestdist;
	vec3_t target, dir;
	bsp_trace_t bsptrace;
	entityState_t state;

	// if there is a dead body wearing kamikze nearby
	if (bs->kamikazebody) {
		// if the bot's view angles and weapon are not used for movement
		if ( !(moveresult->flags & (MOVERESULT_MOVEMENTVIEW | MOVERESULT_MOVEMENTWEAPON)) ) {
			//
			BotAI_GetEntityState(bs->kamikazebody, &state);
			VectorCopy(state.pos.trBase, target);
			target[2] += 8;
			VectorSubtract(target, bs->eye, dir);
			vectoangles(dir, moveresult->ideal_viewangles);
			//
			moveresult->weapon = BotSelectActivateWeapon(bs);
			if (moveresult->weapon == -1) {
				// FIXME: run away!
				moveresult->weapon = 0;
			}
			if (moveresult->weapon) {
				//
				moveresult->flags |= MOVERESULT_MOVEMENTWEAPON | MOVERESULT_MOVEMENTVIEW;
				// if holding the right weapon
				if (bs->cur_ps.weapon == moveresult->weapon) {
					// if the bot is pretty close with its aim
// Q3Rally Code Start
//					if (InFieldOfVision(bs->viewangles, 20, moveresult->ideal_viewangles)) {
					if (InFieldOfVision(bs->cur_ps.viewangles, 20, moveresult->ideal_viewangles)) {
// END
						//
						BotAI_Trace(&bsptrace, bs->eye, NULL, NULL, target, bs->entitynum, MASK_SHOT);
						// if the mine is visible from the current position
						if (bsptrace.fraction >= 1.0 || bsptrace.ent == state.number) {
							// shoot at the mine
							trap_EA_Attack(bs->client);
						}
					}
				}
			}
		}
	}
	if (moveresult->flags & MOVERESULT_BLOCKEDBYAVOIDSPOT) {
		bs->blockedbyavoidspot_time = FloatTime() + 5;
	}
	// if blocked by an avoid spot and the view angles and weapon are used for movement
	if (bs->blockedbyavoidspot_time > FloatTime() &&
		!(moveresult->flags & (MOVERESULT_MOVEMENTVIEW | MOVERESULT_MOVEMENTWEAPON)) ) {
		bestdist = 300;
		bestmine = -1;
		for (i = 0; i < bs->numproxmines; i++) {
			BotAI_GetEntityState(bs->proxmines[i], &state);
			VectorSubtract(state.pos.trBase, bs->origin, dir);
			dist = VectorLength(dir);
			if (dist < bestdist) {
				bestdist = dist;
				bestmine = i;
			}
		}
		if (bestmine != -1) {
			//
			// state->generic1 == TEAM_RED || state->generic1 == TEAM_BLUE
			//
			// deactivate prox mines in the bot's path by shooting
			// rockets or plasma cells etc. at them
			BotAI_GetEntityState(bs->proxmines[bestmine], &state);
			VectorCopy(state.pos.trBase, target);
			target[2] += 2;
			VectorSubtract(target, bs->eye, dir);
			vectoangles(dir, moveresult->ideal_viewangles);
			// if the bot has a weapon that does splash damage
			if (bs->inventory[INVENTORY_PLASMAGUN] > 0 && bs->inventory[INVENTORY_CELLS] > 0)
				moveresult->weapon = WEAPONINDEX_PLASMAGUN;
			else if (bs->inventory[INVENTORY_ROCKETLAUNCHER] > 0 && bs->inventory[INVENTORY_ROCKETS] > 0)
				moveresult->weapon = WEAPONINDEX_ROCKET_LAUNCHER;
			else if (bs->inventory[INVENTORY_BFG10K] > 0 && bs->inventory[INVENTORY_BFGAMMO] > 0)
				moveresult->weapon = WEAPONINDEX_BFG;
			else {
				moveresult->weapon = 0;
			}
			if (moveresult->weapon) {
				//
				moveresult->flags |= MOVERESULT_MOVEMENTWEAPON | MOVERESULT_MOVEMENTVIEW;
				// if holding the right weapon
				if (bs->cur_ps.weapon == moveresult->weapon) {
					// if the bot is pretty close with its aim
// Q3Rally Code Start
//					if (InFieldOfVision(bs->viewangles, 20, moveresult->ideal_viewangles)) {
					if (InFieldOfVision(bs->cur_ps.viewangles, 20, moveresult->ideal_viewangles)) {
// END
						//
						BotAI_Trace(&bsptrace, bs->eye, NULL, NULL, target, bs->entitynum, MASK_SHOT);
						// if the mine is visible from the current position
						if (bsptrace.fraction >= 1.0 || bsptrace.ent == state.number) {
							// shoot at the mine
							trap_EA_Attack(bs->client);
						}
					}
				}
			}
		}
	}
}

/*
==================
AIEnter_Seek_ActivateEntity
==================
*/
void AIEnter_Seek_ActivateEntity(bot_state_t *bs, char *s) {
	BotRecordNodeSwitch(bs, "activate entity", "", s);
	bs->ainode = AINode_Seek_ActivateEntity;
}

/*
==================
AINode_Seek_Activate_Entity
==================
*/
int AINode_Seek_ActivateEntity(bot_state_t *bs) {
	bot_goal_t *goal;
	vec3_t target, dir, ideal_viewangles;
	bot_moveresult_t moveresult;
	int targetvisible;
	bsp_trace_t bsptrace;
	aas_entityinfo_t entinfo;

// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Seek_ActivateEntity\n", bs->client );
// END

	if (BotIsObserver(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Observer(bs, "active entity: observer");
		return qfalse;
	}
	//if in the intermission
	if (BotIntermission(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Intermission(bs, "activate entity: intermission");
		return qfalse;
	}
	//respawn if dead
	if (BotIsDead(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Respawn(bs, "activate entity: bot dead");
		return qfalse;
	}
	//
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer) bs->tfl |= TFL_GRAPPLEHOOK;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) bs->tfl |= TFL_LAVA|TFL_SLIME;
	// map specific code
	BotMapScripts(bs);
	// no enemy
	bs->enemy = -1;
	// if the bot has no activate goal
	if (!bs->activatestack) {
		BotClearActivateGoalStack(bs);
		AIEnter_Seek_NBG(bs, "activate entity: no goal");
		return qfalse;
	}
	//
	goal = &bs->activatestack->goal;
	// initialize target being visible to false
	targetvisible = qfalse;
	// if the bot has to shoot at a target to activate something
	if (bs->activatestack->shoot) {
		//
		BotAI_Trace(&bsptrace, bs->eye, NULL, NULL, bs->activatestack->target, bs->entitynum, MASK_SHOT);
		// if the shootable entity is visible from the current position
		if (bsptrace.fraction >= 1.0 || bsptrace.ent == goal->entitynum) {
			targetvisible = qtrue;
			// if holding the right weapon
			if (bs->cur_ps.weapon == bs->activatestack->weapon) {
				VectorSubtract(bs->activatestack->target, bs->eye, dir);
				vectoangles(dir, ideal_viewangles);
				// if the bot is pretty close with its aim
// Q3Rally Code Start
//				if (InFieldOfVision(bs->viewangles, 20, ideal_viewangles)) {
				if (InFieldOfVision(bs->cur_ps.viewangles, 20, ideal_viewangles)) {
// END
					trap_EA_Attack(bs->client);
				}
			}
		}
	}
	// if the shoot target is visible
	if (targetvisible) {
		// get the entity info of the entity the bot is shooting at
		BotEntityInfo(goal->entitynum, &entinfo);
		// if the entity the bot shoots at moved
		if (!VectorCompare(bs->activatestack->origin, entinfo.origin)) {
#ifdef DEBUG
			BotAI_Print(PRT_MESSAGE, "hit shootable button or trigger\n");
#endif //DEBUG
			bs->activatestack->time = 0;
		}
		// if the activate goal has been activated or the bot takes too long
		if (bs->activatestack->time < FloatTime()) {
			BotPopFromActivateGoalStack(bs);
			// if there are more activate goals on the stack
			if (bs->activatestack) {
				bs->activatestack->time = FloatTime() + 10;
				return qfalse;
			}
			AIEnter_Seek_NBG(bs, "activate entity: time out");
			return qfalse;
		}
		memset(&moveresult, 0, sizeof(bot_moveresult_t));
	}
	else {
		// if the bot has no goal
		if (!goal) {
			bs->activatestack->time = 0;
		}
		// if the bot does not have a shoot goal
		else if (!bs->activatestack->shoot) {
			//if the bot touches the current goal
			if (trap_BotTouchingGoal(bs->origin, goal)) {
#ifdef DEBUG
				BotAI_Print(PRT_MESSAGE, "touched button or trigger\n");
#endif //DEBUG
				bs->activatestack->time = 0;
			}
		}
		// if the activate goal has been activated or the bot takes too long
		if (bs->activatestack->time < FloatTime()) {
			BotPopFromActivateGoalStack(bs);
			// if there are more activate goals on the stack
			if (bs->activatestack) {
				bs->activatestack->time = FloatTime() + 10;
				return qfalse;
			}
			AIEnter_Seek_NBG(bs, "activate entity: activated");
			return qfalse;
		}
		//predict obstacles
		if (BotAIPredictObstacles(bs, goal))
			return qfalse;
		//initialize the movement state
		BotSetupForMovement(bs);
		//move towards the goal
// Q3Rally Code Start
		trap_EA_MoveForward( bs->entitynum );
// END
		trap_BotMoveToGoal(&moveresult, bs->ms, goal, bs->tfl);
		//if the movement failed
		if (moveresult.failure) {
			//reset the avoid reach, otherwise bot is stuck in current area
			trap_BotResetAvoidReach(bs->ms);
			//
			bs->activatestack->time = 0;
		}
		//check if the bot is blocked
		BotAIBlocked(bs, &moveresult, qtrue);
	}
	//
	BotClearPath(bs, &moveresult);
	// if the bot has to shoot to activate
	if (bs->activatestack->shoot) {
		// if the view angles aren't yet used for the movement
		if (!(moveresult.flags & MOVERESULT_MOVEMENTVIEW)) {
			VectorSubtract(bs->activatestack->target, bs->eye, dir);
			vectoangles(dir, moveresult.ideal_viewangles);
			moveresult.flags |= MOVERESULT_MOVEMENTVIEW;
		}
		// if there's no weapon yet used for the movement
		if (!(moveresult.flags & MOVERESULT_MOVEMENTWEAPON)) {
			moveresult.flags |= MOVERESULT_MOVEMENTWEAPON;
			//
			bs->activatestack->weapon = BotSelectActivateWeapon(bs);
			if (bs->activatestack->weapon == -1) {
				//FIXME: find a decent weapon first
				bs->activatestack->weapon = 0;
			}
			moveresult.weapon = bs->activatestack->weapon;
		}
	}
	// if the ideal view angles are set for movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	}
	// if waiting for something
	else if (moveresult.flags & MOVERESULT_WAITING) {
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	}
	else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (trap_BotMovementViewTarget(bs->ms, goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
		}
		else {
			vectoangles(moveresult.movedir, bs->ideal_viewangles);
		}
		bs->ideal_viewangles[2] *= 0.5;
	}
	// if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON)
		bs->weaponnum = moveresult.weapon;
	// if there is an enemy
	if (BotFindEnemy(bs, -1)) {
		if (BotWantsToRetreat(bs)) {
			//keep the current long term goal and retreat
			AIEnter_Battle_NBG(bs, "activate entity: found enemy");
		}
		else {
			trap_BotResetLastAvoidReach(bs->ms);
			//empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			//go fight
			AIEnter_Battle_Fight(bs, "activate entity: found enemy");
		}
		BotClearActivateGoalStack(bs);
	}
	return qtrue;
}

/*
==================
AIEnter_Seek_NBG
==================
*/
void AIEnter_Seek_NBG(bot_state_t *bs, char *s) {
	bot_goal_t goal;
	char buf[144];

	if (trap_BotGetTopGoal(bs->gs, &goal)) {
		trap_BotGoalName(goal.number, buf, 144);
		BotRecordNodeSwitch(bs, "seek NBG", buf, s);
	}
	else {
		BotRecordNodeSwitch(bs, "seek NBG", "no goal", s);
	}
	bs->ainode = AINode_Seek_NBG;
}

/*
==================
AINode_Seek_NBG
==================
*/
int AINode_Seek_NBG(bot_state_t *bs) {
	bot_goal_t goal;
	vec3_t target, dir;
	bot_moveresult_t moveresult;

// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Seek_NBG\n", bs->client );
// END

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "seek nbg: observer");
		return qfalse;
	}
	//if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "seek nbg: intermision");
		return qfalse;
	}
	//respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "seek nbg: bot dead");
		return qfalse;
	}
	//
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer) bs->tfl |= TFL_GRAPPLEHOOK;
	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) bs->tfl |= TFL_LAVA|TFL_SLIME;
	//
	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	//map specific code
	BotMapScripts(bs);
	//no enemy
	bs->enemy = -1;
	//if the bot has no goal
	if (!trap_BotGetTopGoal(bs->gs, &goal)) bs->nbg_time = 0;
	//if the bot touches the current goal
	else if (BotReachedGoal(bs, &goal)) {
		BotChooseWeapon(bs);
		bs->nbg_time = 0;
	}
	//
	if (bs->nbg_time < FloatTime()) {
		//pop the current goal from the stack
		trap_BotPopGoal(bs->gs);
		//check for new nearby items right away
		//NOTE: we canNOT reset the check_time to zero because it would create an endless loop of node switches
		bs->check_time = FloatTime() + 0.05;
		//go back to seek ltg
		AIEnter_Seek_LTG(bs, "seek nbg: time out");
		return qfalse;
	}
	//predict obstacles
	if (BotAIPredictObstacles(bs, &goal))
		return qfalse;
	//initialize the movement state
	BotSetupForMovement(bs);
	//move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
// Q3Rally Code Start
	trap_EA_MoveForward( bs->entitynum );
// END

	//if the movement failed
	if (moveresult.failure) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		bs->nbg_time = 0;
	}
	//check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qtrue);
	//
	BotClearPath(bs, &moveresult);
	//if the viewangles are used for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	}
	//if waiting for something
	else if (moveresult.flags & MOVERESULT_WAITING) {
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	}
	else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (!trap_BotGetSecondGoal(bs->gs, &goal)) trap_BotGetTopGoal(bs->gs, &goal);
		if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
		}
		//FIXME: look at cluster portals?
		else vectoangles(moveresult.movedir, bs->ideal_viewangles);
		bs->ideal_viewangles[2] *= 0.5;
	}
	//if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) bs->weaponnum = moveresult.weapon;
	//if there is an enemy
	if (BotFindEnemy(bs, -1)) {
		if (BotWantsToRetreat(bs)) {
			//keep the current long term goal and retreat
			AIEnter_Battle_NBG(bs, "seek nbg: found enemy");
		}
		else {
			trap_BotResetLastAvoidReach(bs->ms);
			//empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			//go fight
			AIEnter_Battle_Fight(bs, "seek nbg: found enemy");
		}
	}
	return qtrue;
}

/*
==================
AIEnter_Seek_LTG
==================
*/
void AIEnter_Seek_LTG(bot_state_t *bs, char *s) {
	bot_goal_t goal;
	char buf[144];

	if (trap_BotGetTopGoal(bs->gs, &goal)) {
		trap_BotGoalName(goal.number, buf, 144);
		BotRecordNodeSwitch(bs, "seek LTG", buf, s);
	}
	else {
		BotRecordNodeSwitch(bs, "seek LTG", "no goal", s);
	}
	bs->ainode = AINode_Seek_LTG;
}

/*
==================
AINode_Seek_LTG
==================
*/
int AINode_Seek_LTG(bot_state_t *bs)
{
	bot_goal_t goal;
	vec3_t target, dir;
	bot_moveresult_t moveresult;
	int range;
	//char buf[128];
	//bot_goal_t tmpgoal;

// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Seek_LTG\n", bs->client );
// END

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "seek ltg: observer");
		return qfalse;
	}
	//if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "seek ltg: intermission");
		return qfalse;
	}
	//respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "seek ltg: bot dead");
		return qfalse;
	}
	//
	if (BotChat_Random(bs)) {
		bs->stand_time = FloatTime() + BotChatTime(bs);
		AIEnter_Stand(bs, "seek ltg: random chat");
		return qfalse;
	}
	//
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer) bs->tfl |= TFL_GRAPPLEHOOK;
	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) bs->tfl |= TFL_LAVA|TFL_SLIME;
	//
	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	//map specific code
	BotMapScripts(bs);
	//no enemy
	bs->enemy = -1;
	//
	if (bs->killedenemy_time > FloatTime() - 2) {
		if (random() < bs->thinktime * 1) {
			trap_EA_Gesture(bs->client);
		}
	}
	//if there is an enemy
// Q3Rally Code Start
//	if (BotFindEnemy(bs, -1)) {
   if ( BotFindEnemy(bs, -1) && gametype != GT_RACING && gametype != GT_SPRINT && gametype != GT_TEAM_RACING ) {
// END
		if (BotWantsToRetreat(bs)) {
			//keep the current long term goal and retreat
			AIEnter_Battle_Retreat(bs, "seek ltg: found enemy");
			return qfalse;
		}
		else {
			trap_BotResetLastAvoidReach(bs->ms);
			//empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			//go fight
			AIEnter_Battle_Fight(bs, "seek ltg: found enemy");
			return qfalse;
		}
	}
	//
	BotTeamGoals(bs, qfalse);
	//get the current long term goal
	if (!BotLongTermGoal(bs, bs->tfl, qfalse, &goal)) {
		return qtrue;
	}
	//check for nearby goals periodicly
	if (bs->check_time < FloatTime()) {
		bs->check_time = FloatTime() + 0.5;
		//check if the bot wants to camp
		BotWantsToCamp(bs);
		//
		if (bs->ltgtype == LTG_DEFENDKEYAREA) range = 400;
		else range = 150;
		//
#ifdef CTF
		if (gametype == GT_CTF) {
			//if carrying a flag the bot shouldn't be distracted too much
			if (BotCTFCarryingFlag(bs))
				range = 50;
		}
#endif //CTF
#ifdef MISSIONPACK
		else if (gametype == GT_1FCTF) {
			if (Bot1FCTFCarryingFlag(bs))
				range = 50;
		}
		else if (gametype == GT_HARVESTER) {
			if (BotHarvesterCarryingCubes(bs))
				range = 80;
		}
#endif
		//
		if (BotNearbyGoal(bs, bs->tfl, &goal, range)) {
			trap_BotResetLastAvoidReach(bs->ms);
			//get the goal at the top of the stack
			//trap_BotGetTopGoal(bs->gs, &tmpgoal);
			//trap_BotGoalName(tmpgoal.number, buf, 144);
			//BotAI_Print(PRT_MESSAGE, "new nearby goal %s\n", buf);
			//time the bot gets to pick up the nearby goal item
			bs->nbg_time = FloatTime() + 4 + range * 0.01;
			AIEnter_Seek_NBG(bs, "ltg seek: nbg");
			return qfalse;
		}
	}
	//predict obstacles
	if (BotAIPredictObstacles(bs, &goal))
		return qfalse;
	//initialize the movement state
	BotSetupForMovement(bs);
	//move towards the goal
// Q3Rally Code Start
/*
	if( isRallyRace() )
	{
		vec3_t		vec;
		VectorSubtract( goal.origin, bs->origin, vec );
		bs->viewangles[YAW] = vectoyaw( vec );

		Com_Printf( "viewyaw %f\n", bs->viewangles[YAW] );
		return qtrue;
	}
*/
// END

	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);

// Q3Rally Code Start
	trap_EA_MoveForward( bs->entitynum );

//	Com_Printf( "bot %i moveresult: blocked %i, fail %i, view angle %f, movedir (%f %f %f)\n", moveresult.blocked, moveresult.failure, moveresult.ideal_viewangles[YAW], moveresult.movedir );
// END

	//if the movement failed
	if (moveresult.failure) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	//
	BotAIBlocked(bs, &moveresult, qtrue);
	//
	BotClearPath(bs, &moveresult);
	//if the viewangles are used for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	}
	//if waiting for something
	else if (moveresult.flags & MOVERESULT_WAITING) {
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	}
	else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
		}
		//FIXME: look at cluster portals?
		else if (VectorLengthSquared(moveresult.movedir)) {
			vectoangles(moveresult.movedir, bs->ideal_viewangles);
		}
		else if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			vectoangles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
		bs->ideal_viewangles[2] *= 0.5;
	}
	//if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) bs->weaponnum = moveresult.weapon;
	//
	return qtrue;
}

/*
==================
AIEnter_Battle_Fight
==================
*/
void AIEnter_Battle_Fight(bot_state_t *bs, char *s) {
	const char *lcsReason = NULL;
// Q3Rally Code Start
   if ( gametype == GT_RACING || gametype == GT_SPRINT || gametype == GT_TEAM_RACING )
		return;
	if ( gametype == GT_LCS &&
		( !BotWantsToChase(bs) || Bot_LcsShouldAvoidBattleEntry( bs, &lcsReason ) ) ) {
		AIEnter_Seek_LTG( bs, lcsReason ? (char *)lcsReason : "lcs_avoid_contact" );
		return;
	}
// END

	BotRecordNodeSwitch(bs, "battle fight", "", s);
	trap_BotResetLastAvoidReach(bs->ms);
	bs->ainode = AINode_Battle_Fight;
	bs->flags &= ~BFL_FIGHTSUICIDAL;
}

/*
==================
AIEnter_Battle_SuicidalFight
==================
*/
void AIEnter_Battle_SuicidalFight(bot_state_t *bs, char *s) {
// Q3Rally Code Start
//	if ( gametype == GT_RACING )
//		return;
// END

	BotRecordNodeSwitch(bs, "battle fight", "", s);
	trap_BotResetLastAvoidReach(bs->ms);
	bs->ainode = AINode_Battle_Fight;
	bs->flags |= BFL_FIGHTSUICIDAL;
}

/*
==================
AINode_Battle_Fight
==================
*/
int AINode_Battle_Fight(bot_state_t *bs) {
	int areanum;
	vec3_t target;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;

// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Battle_Fight\n", bs->client );
// END

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "battle fight: observer");
		return qfalse;
	}

	//if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "battle fight: intermission");
		return qfalse;
	}
	//respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "battle fight: bot dead");
		return qfalse;
	}
	//if there is another better enemy
	if (BotFindEnemy(bs, bs->enemy)) {
#ifdef DEBUG
		BotAI_Print(PRT_MESSAGE, "found new better enemy\n");
#endif
	}
	//if no enemy
	if (bs->enemy < 0) {
		AIEnter_Seek_LTG(bs, "battle fight: no enemy");
		return qfalse;
	}
	//
	BotEntityInfo(bs->enemy, &entinfo);
	//if the enemy is dead
	if (bs->enemydeath_time) {
		if (bs->enemydeath_time < FloatTime() - 1.0) {
			bs->enemydeath_time = 0;
			if (bs->enemysuicide) {
				BotChat_EnemySuicide(bs);
			}
			if (bs->lastkilledplayer == bs->enemy && BotChat_Kill(bs)) {
				bs->stand_time = FloatTime() + BotChatTime(bs);
				AIEnter_Stand(bs, "battle fight: enemy dead");
			}
			else {
				bs->ltg_time = 0;
				AIEnter_Seek_LTG(bs, "battle fight: enemy dead");
			}
			return qfalse;
		}
	}
	else {
		if (EntityIsDead(&entinfo)) {
			bs->enemydeath_time = FloatTime();
		}
	}
	//if the enemy is invisible and not shooting the bot looses track easily
	if (EntityIsInvisible(&entinfo) && !EntityIsShooting(&entinfo)) {
		if (random() < 0.2) {
			AIEnter_Seek_LTG(bs, "battle fight: invisible");
			return qfalse;
		}
	}
	//
	VectorCopy(entinfo.origin, target);
	// if not a player enemy
	if (bs->enemy >= MAX_CLIENTS) {
#ifdef MISSIONPACK
		// if attacking an obelisk
		if ( bs->enemy == redobelisk.entitynum ||
			bs->enemy == blueobelisk.entitynum ) {
			target[2] += 16;
		}
#endif
	}
	//update the reachability area and origin if possible
	areanum = BotPointAreaNum(target);
	if (areanum && trap_AAS_AreaReachability(areanum)) {
		VectorCopy(target, bs->lastenemyorigin);
		bs->lastenemyareanum = areanum;
	}
	//update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);
	//if the bot's health decreased
	if (bs->lastframe_health > bs->inventory[INVENTORY_HEALTH]) {
		if (BotChat_HitNoDeath(bs)) {
			bs->stand_time = FloatTime() + BotChatTime(bs);
			AIEnter_Stand(bs, "battle fight: chat health decreased");
			return qfalse;
		}
	}
	//if the bot hit someone
	if (bs->cur_ps.persistant[PERS_HITS] > bs->lasthitcount) {
		if (BotChat_HitNoKill(bs)) {
			bs->stand_time = FloatTime() + BotChatTime(bs);
			AIEnter_Stand(bs, "battle fight: chat hit someone");
			return qfalse;
		}
	}
	//if the enemy is not visible
// Q3Rally Code Start
//	if (!BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
	if (!BotEntityVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, 360, bs->enemy)) {
// END
#ifdef MISSIONPACK
		if (bs->enemy == redobelisk.entitynum || bs->enemy == blueobelisk.entitynum) {
			AIEnter_Battle_Chase(bs, "battle fight: obelisk out of sight");
			return qfalse;
		}
#endif
		if (BotWantsToChase(bs)) {
			AIEnter_Battle_Chase(bs, "battle fight: enemy out of sight");
			return qfalse;
		}
		else {
			AIEnter_Seek_LTG(bs, "battle fight: enemy out of sight");
			return qfalse;
		}
	}
	//use holdable items
	BotBattleUseItems(bs);
	//
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer) bs->tfl |= TFL_GRAPPLEHOOK;
	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) bs->tfl |= TFL_LAVA|TFL_SLIME;
	//
	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	//choose the best weapon to fight with
	BotChooseWeapon(bs);
	//do attack movements
	moveresult = BotAttackMove(bs, bs->tfl);
	//if the movement failed
	if (moveresult.failure) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	//
	BotAIBlocked(bs, &moveresult, qfalse);
	//aim at the enemy
	BotAimAtEnemy(bs);
	//attack the enemy if possible
	BotCheckAttack(bs);
	//if the bot wants to retreat
	if (!(bs->flags & BFL_FIGHTSUICIDAL)) {
		if (BotWantsToRetreat(bs)) {
			AIEnter_Battle_Retreat(bs, "battle fight: wants to retreat");
			return qtrue;
		}
	}
	return qtrue;
}

/*
==================
AIEnter_Battle_Chase
==================
*/
void AIEnter_Battle_Chase(bot_state_t *bs, char *s) {
	const char *lcsReason = NULL;
// Q3Rally Code Start
   if ( gametype == GT_RACING || gametype == GT_SPRINT || gametype == GT_TEAM_RACING )
		return;
	if ( gametype == GT_LCS && Bot_LcsShouldAvoidBattleEntry( bs, &lcsReason ) ) {
		AIEnter_Seek_LTG( bs, lcsReason ? (char *)lcsReason : "lcs_avoid_contact" );
		return;
	}
// END

	BotRecordNodeSwitch(bs, "battle chase", "", s);
	bs->chase_time = FloatTime();
	bs->ainode = AINode_Battle_Chase;
}

/*
==================
AINode_Battle_Chase
==================
*/
int AINode_Battle_Chase(bot_state_t *bs)
{
	bot_goal_t goal;
	vec3_t target, dir;
	bot_moveresult_t moveresult;
	float range;

// Q3Rally Code Start
//	Com_Printf( "bot %i in AINode_Battle_Chase\n", bs->client );
// END

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "battle chase: observer");
		return qfalse;
	}
	//if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "battle chase: intermission");
		return qfalse;
	}
	//respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "battle chase: bot dead");
		return qfalse;
	}
	//if no enemy
	if (bs->enemy < 0) {
		AIEnter_Seek_LTG(bs, "battle chase: no enemy");
		return qfalse;
	}
	//if the enemy is visible
// STONELANCE
//	if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
	if (BotEntityVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, 360, bs->enemy)) {
// END
		AIEnter_Battle_Fight(bs, "battle chase");
		return qfalse;
	}
	//if there is another enemy
	if (BotFindEnemy(bs, -1)) {
		AIEnter_Battle_Fight(bs, "battle chase: better enemy");
		return qfalse;
	}
	//there is no last enemy area
	if (!bs->lastenemyareanum) {
		AIEnter_Seek_LTG(bs, "battle chase: no enemy area");
		return qfalse;
	}
	//
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer) bs->tfl |= TFL_GRAPPLEHOOK;
	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) bs->tfl |= TFL_LAVA|TFL_SLIME;
	//
	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	//map specific code
	BotMapScripts(bs);
	//create the chase goal
	goal.entitynum = bs->enemy;
	goal.areanum = bs->lastenemyareanum;
	VectorCopy(bs->lastenemyorigin, goal.origin);
	VectorSet(goal.mins, -8, -8, -8);
	VectorSet(goal.maxs, 8, 8, 8);
	//if the last seen enemy spot is reached the enemy could not be found
	if (trap_BotTouchingGoal(bs->origin, &goal)) bs->chase_time = 0;
	//if there's no chase time left
	if (!bs->chase_time || bs->chase_time < FloatTime() - 10) {
		AIEnter_Seek_LTG(bs, "battle chase: time out");
		return qfalse;
	}
	//check for nearby goals periodicly
	if (bs->check_time < FloatTime()) {
		bs->check_time = FloatTime() + 1;
		range = 150;
		//
		if (BotNearbyGoal(bs, bs->tfl, &goal, range)) {
			//the bot gets 5 seconds to pick up the nearby goal item
			bs->nbg_time = FloatTime() + 0.1 * range + 1;
			trap_BotResetLastAvoidReach(bs->ms);
			AIEnter_Battle_NBG(bs, "battle chase: nbg");
			return qfalse;
		}
	}
	//
	BotUpdateBattleInventory(bs, bs->enemy);
	//initialize the movement state
	BotSetupForMovement(bs);
	//move towards the goal
// STONELANCE
	trap_EA_MoveForward( bs->entitynum );
// END
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
	//if the movement failed
	if (moveresult.failure) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	//
	BotAIBlocked(bs, &moveresult, qfalse);
	//
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	}
	else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (bs->chase_time > FloatTime() - 2) {
			BotAimAtEnemy(bs);
		}
		else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
				VectorSubtract(target, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
			}
			else {
				vectoangles(moveresult.movedir, bs->ideal_viewangles);
			}
		}
		bs->ideal_viewangles[2] *= 0.5;
	}
	//if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) bs->weaponnum = moveresult.weapon;
	//if the bot is in the area the enemy was last seen in
	if (bs->areanum == bs->lastenemyareanum) bs->chase_time = 0;
	//if the bot wants to retreat (the bot could have been damage during the chase)
	if (BotWantsToRetreat(bs)) {
		AIEnter_Battle_Retreat(bs, "battle chase: wants to retreat");
		return qtrue;
	}
	return qtrue;
}

/*
==================
AIEnter_Battle_Retreat
==================
*/
void AIEnter_Battle_Retreat(bot_state_t *bs, char *s) {
	const char *lcsReason = NULL;
// STONELANCE
   if ( gametype == GT_RACING || gametype == GT_SPRINT || gametype == GT_TEAM_RACING )
		return;
   if ( gametype == GT_LCS && Bot_LcsShouldAvoidBattleEntry( bs, &lcsReason ) ) {
		AIEnter_Seek_LTG( bs, lcsReason ? (char *)lcsReason : "lcs_avoid_contact" );
		return;
	}
// END

	BotRecordNodeSwitch(bs, "battle retreat", "", s);
	bs->ainode = AINode_Battle_Retreat;
}

/*
==================
AINode_Battle_Retreat
==================
*/
int AINode_Battle_Retreat(bot_state_t *bs) {
	bot_goal_t goal;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;
	vec3_t target, dir;
	float attack_skill, range;
	int areanum;

// STONELANCE
//	Com_Printf( "bot %i in AINode_Battle_Retreat\n", bs->client );
// END

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "battle retreat: observer");
		return qfalse;
	}
	//if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "battle retreat: intermission");
		return qfalse;
	}
	//respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "battle retreat: bot dead");
		return qfalse;
	}
	//if no enemy
	if (bs->enemy < 0) {
		AIEnter_Seek_LTG(bs, "battle retreat: no enemy");
		return qfalse;
	}
	//
	BotEntityInfo(bs->enemy, &entinfo);
	if (EntityIsDead(&entinfo)) {
		AIEnter_Seek_LTG(bs, "battle retreat: enemy dead");
		return qfalse;
	}
	//if there is another better enemy
	if (BotFindEnemy(bs, bs->enemy)) {
#ifdef DEBUG
		BotAI_Print(PRT_MESSAGE, "found new better enemy\n");
#endif
	}
	//
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer) bs->tfl |= TFL_GRAPPLEHOOK;
	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) bs->tfl |= TFL_LAVA|TFL_SLIME;
	//map specific code
	BotMapScripts(bs);
	//update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);
	//if the bot doesn't want to retreat anymore... probably picked up some nice items
	if (BotWantsToChase(bs)) {
		//empty the goal stack, when chasing, only the enemy is the goal
		trap_BotEmptyGoalStack(bs->gs);
		//go chase the enemy
		AIEnter_Battle_Chase(bs, "battle retreat: wants to chase");
		return qfalse;
	}
	//update the last time the enemy was visible
// STONELANCE
//	if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
	if (BotEntityVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, 360, bs->enemy)) {
// END
		bs->enemyvisible_time = FloatTime();
		VectorCopy(entinfo.origin, target);
		// if not a player enemy
		if (bs->enemy >= MAX_CLIENTS) {
#ifdef MISSIONPACK
			// if attacking an obelisk
			if ( bs->enemy == redobelisk.entitynum ||
				bs->enemy == blueobelisk.entitynum ) {
				target[2] += 16;
			}
#endif
		}
		//update the reachability area and origin if possible
		areanum = BotPointAreaNum(target);
		if (areanum && trap_AAS_AreaReachability(areanum)) {
			VectorCopy(target, bs->lastenemyorigin);
			bs->lastenemyareanum = areanum;
		}
	}
	//if the enemy is NOT visible for 4 seconds
	if (bs->enemyvisible_time < FloatTime() - 4) {
		AIEnter_Seek_LTG(bs, "battle retreat: lost enemy");
		return qfalse;
	}
	//else if the enemy is NOT visible
	else if (bs->enemyvisible_time < FloatTime()) {
		//if there is another enemy
		if (BotFindEnemy(bs, -1)) {
			AIEnter_Battle_Fight(bs, "battle retreat: another enemy");
			return qfalse;
		}
	}
	//
	BotTeamGoals(bs, qtrue);
	//use holdable items
	BotBattleUseItems(bs);
	//get the current long term goal while retreating
	if (!BotLongTermGoal(bs, bs->tfl, qtrue, &goal)) {
		AIEnter_Battle_SuicidalFight(bs, "battle retreat: no way out");
		return qfalse;
	}
	//check for nearby goals periodicly
	if (bs->check_time < FloatTime()) {
		bs->check_time = FloatTime() + 1;
		range = 150;
#ifdef CTF
		if (gametype == GT_CTF) {
			//if carrying a flag the bot shouldn't be distracted too much
			if (BotCTFCarryingFlag(bs))
				range = 50;
		}
#endif //CTF
#ifdef MISSIONPACK
		else if (gametype == GT_1FCTF) {
			if (Bot1FCTFCarryingFlag(bs))
				range = 50;
		}
		else if (gametype == GT_HARVESTER) {
			if (BotHarvesterCarryingCubes(bs))
				range = 80;
		}
#endif
		//
		if (BotNearbyGoal(bs, bs->tfl, &goal, range)) {
			trap_BotResetLastAvoidReach(bs->ms);
			//time the bot gets to pick up the nearby goal item
			bs->nbg_time = FloatTime() + range / 100 + 1;
			AIEnter_Battle_NBG(bs, "battle retreat: nbg");
			return qfalse;
		}
	}
	//initialize the movement state
	BotSetupForMovement(bs);
	//move towards the goal
// STONELANCE
	trap_EA_MoveForward( bs->entitynum );
// END
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
	//if the movement failed
	if (moveresult.failure) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	//
	BotAIBlocked(bs, &moveresult, qfalse);
	//choose the best weapon to fight with
	BotChooseWeapon(bs);
	//if the view is fixed for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	}
	else if (!(moveresult.flags & MOVERESULT_MOVEMENTVIEWSET)
				&& !(bs->flags & BFL_IDEALVIEWSET) ) {
		attack_skill = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_ATTACK_SKILL, 0, 1);
		//if the bot is skilled enough
		if (attack_skill > 0.3) {
			BotAimAtEnemy(bs);
		}
		else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
				VectorSubtract(target, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
			}
			else {
				vectoangles(moveresult.movedir, bs->ideal_viewangles);
			}
			bs->ideal_viewangles[2] *= 0.5;
		}
	}
	//if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) bs->weaponnum = moveresult.weapon;
	//attack the enemy if possible
	BotCheckAttack(bs);
	//
	return qtrue;
}

/*
==================
AIEnter_Battle_NBG
==================
*/
void AIEnter_Battle_NBG(bot_state_t *bs, char *s) {
// STONELANCE
   if ( gametype == GT_RACING || gametype == GT_SPRINT || gametype == GT_TEAM_RACING )
		return;
// END

	BotRecordNodeSwitch(bs, "battle NBG", "", s);
	bs->ainode = AINode_Battle_NBG;
}

/*
==================
AINode_Battle_NBG
==================
*/
int AINode_Battle_NBG(bot_state_t *bs) {
	int areanum;
	bot_goal_t goal;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;
	float attack_skill;
	vec3_t target, dir;

// STONELANCE
//	Com_Printf( "bot %i in AINode_Battle_NBG\n", bs->client );
// END

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "battle nbg: observer");
		return qfalse;
	}
	//if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "battle nbg: intermission");
		return qfalse;
	}
	//respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "battle nbg: bot dead");
		return qfalse;
	}
	//if no enemy
	if (bs->enemy < 0) {
		AIEnter_Seek_NBG(bs, "battle nbg: no enemy");
		return qfalse;
	}
	//
	BotEntityInfo(bs->enemy, &entinfo);
	if (EntityIsDead(&entinfo)) {
		AIEnter_Seek_NBG(bs, "battle nbg: enemy dead");
		return qfalse;
	}
	//
	bs->tfl = TFL_DEFAULT;
	if (bot_grapple.integer) bs->tfl |= TFL_GRAPPLEHOOK;
	//if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) bs->tfl |= TFL_LAVA|TFL_SLIME;
	//
	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	//map specific code
	BotMapScripts(bs);
	//update the last time the enemy was visible
// STONELANCE
//	if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)) {
	if (BotEntityVisible(bs->entitynum, bs->eye, bs->cur_ps.viewangles, 360, bs->enemy)) {
// END
		bs->enemyvisible_time = FloatTime();
		VectorCopy(entinfo.origin, target);
		// if not a player enemy
		if (bs->enemy >= MAX_CLIENTS) {
#ifdef MISSIONPACK
			// if attacking an obelisk
			if ( bs->enemy == redobelisk.entitynum ||
				bs->enemy == blueobelisk.entitynum ) {
				target[2] += 16;
			}
#endif
		}
		//update the reachability area and origin if possible
		areanum = BotPointAreaNum(target);
		if (areanum && trap_AAS_AreaReachability(areanum)) {
			VectorCopy(target, bs->lastenemyorigin);
			bs->lastenemyareanum = areanum;
		}
	}
	//if the bot has no goal or touches the current goal
	if (!trap_BotGetTopGoal(bs->gs, &goal)) {
		bs->nbg_time = 0;
	}
	else if (BotReachedGoal(bs, &goal)) {
		bs->nbg_time = 0;
	}
	//
	if (bs->nbg_time < FloatTime()) {
		//pop the current goal from the stack
		trap_BotPopGoal(bs->gs);
		//if the bot still has a goal
		if (trap_BotGetTopGoal(bs->gs, &goal))
			AIEnter_Battle_Retreat(bs, "battle nbg: time out");
		else
			AIEnter_Battle_Fight(bs, "battle nbg: time out");
		//
		return qfalse;
	}
	//initialize the movement state
	BotSetupForMovement(bs);
	//move towards the goal
// STONELANCE
	trap_EA_MoveForward( bs->entitynum );
// END
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
	//if the movement failed
	if (moveresult.failure) {
		//reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->nbg_time = 0;
	}
	//
	BotAIBlocked(bs, &moveresult, qfalse);
	//update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);
	//choose the best weapon to fight with
	BotChooseWeapon(bs);
	//if the view is fixed for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	}
	else if (!(moveresult.flags & MOVERESULT_MOVEMENTVIEWSET)
				&& !(bs->flags & BFL_IDEALVIEWSET)) {
		attack_skill = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_ATTACK_SKILL, 0, 1);
		//if the bot is skilled enough and the enemy is visible
		if (attack_skill > 0.3) {
			//&& BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, bs->enemy)
			BotAimAtEnemy(bs);
		}
		else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
				VectorSubtract(target, bs->origin, dir);
				vectoangles(dir, bs->ideal_viewangles);
			}
			else {
				vectoangles(moveresult.movedir, bs->ideal_viewangles);
			}
			bs->ideal_viewangles[2] *= 0.5;
		}
	}
	//if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) bs->weaponnum = moveresult.weapon;
	//attack the enemy if possible
	BotCheckAttack(bs);
	//
	return qtrue;
}


// STONELANCE - April 23, 2002
/*
==================
AIEnter_MoveToNextCheckpoint
==================
*/
void AIEnter_MoveToNextCheckpoint( bot_state_t *bs, char *s )
{
	if ( !isRallyRace() )
		return;

	BotRecordNodeSwitch(bs, "move to next checkpoint", "", s);
	bs->ainode = AINode_MoveToNextCheckpoint;
}


int Bot_CheckForObstacles( bot_state_t *bs, vec3_t angles, int throttleChange )
{
	trace_t		tr;
	vec3_t		start, end;
	vec3_t		forward, right;
	vec3_t		mins = { -4, -4, -4 };
	vec3_t		maxs = { 4, 4, 4 };
	float		rightFrac, centerFrac, leftFrac;
	float		dot, dot2;
	int			hitEnt;

	AngleVectors( bs->cur_ps.viewangles, forward, right, NULL );

	dot = DotProduct( forward, bs->cur_ps.velocity );
/*
	if ( dot >= 0.0f )
		dot = 1;
	if ( dot < 0.0f )
		dot = -1;
*/

	// center forward tracer, looking ahead 150 units plus 10% of velocity
	VectorCopy( bs->cur_ps.origin, start );
	VectorMA( start, 150.0f, forward, end );
	if ( dot >= 0.0f )
		VectorMA( end, 0.075f, bs->cur_ps.velocity,end );
	trap_Trace( &tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID );

	if ( tr.fraction < 1.0f && g_entities[tr.entityNum].flags & FL_EXTRA_BBOX )
		hitEnt = g_entities[tr.entityNum].r.ownerNum;
	else
		hitEnt = tr.entityNum;

	if( tr.fraction < 1.0f && (tr.contents & CONTENTS_BODY) &&
		g_entities[hitEnt].client )
	{
		dot2 = DotProduct( forward, g_entities[hitEnt].client->ps.velocity );
		if( dot2 > dot )
		{
			centerFrac = 1.0f;
//			Com_Printf( "Not avoiding player\n" );
		}
		else
		{
			centerFrac = tr.fraction;
//			Com_Printf( "Avoiding player\n" );
		}
	}
	else if( tr.plane.normal[2] < 0.5f )
		centerFrac = tr.fraction;
	else
		centerFrac = 1.0f;

	// right tracer, looking ahead 1 seconds
	VectorMA( bs->cur_ps.origin, CAR_WIDTH/2, right, start );
//	VectorMA( start, CAR_LENGTH, forward, end );
	VectorMA( start, 0.8f, bs->cur_ps.velocity, end );
	trap_Trace( &tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID );
	
	if ( tr.fraction < 1.0f && g_entities[tr.entityNum].flags & FL_EXTRA_BBOX )
		hitEnt = g_entities[tr.entityNum].r.ownerNum;
	else
		hitEnt = tr.entityNum;
	
	if( tr.fraction < 1.0f && (tr.contents & CONTENTS_BODY) &&
		g_entities[hitEnt].client )
	{
		dot2 = DotProduct( forward, g_entities[hitEnt].client->ps.velocity );
		if( dot2 > dot )
		{
			rightFrac = 1.0f;
//			Com_Printf( "Not avoiding player\n" );
		}
		else
		{
			rightFrac = tr.fraction;
//			Com_Printf( "Avoiding player\n" );
		}
	}
	else if( tr.plane.normal[2] < 0.5f )
		rightFrac = tr.fraction;
	else
		rightFrac = 1.0f;

	// left tracer, looking ahead 1 seconds
	VectorMA( bs->cur_ps.origin, -CAR_WIDTH/2, right, start );
//	VectorMA( start, CAR_LENGTH, forward, end );
	VectorMA( start, 0.8f, bs->cur_ps.velocity, end );
	trap_Trace( &tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID );

	if ( tr.fraction < 1.0f && g_entities[tr.entityNum].flags & FL_EXTRA_BBOX )
		hitEnt = g_entities[tr.entityNum].r.ownerNum;
	else
		hitEnt = tr.entityNum;

	if( tr.fraction < 1.0f && (tr.contents & CONTENTS_BODY) &&
		g_entities[hitEnt].client )
	{
		dot2 = DotProduct( forward, g_entities[hitEnt].client->ps.velocity );
		if( dot2 > dot )
		{
			leftFrac = 1.0f;
//			Com_Printf( "Not avoiding player\n" );
		}
		else
		{
			leftFrac = tr.fraction;
//			Com_Printf( "Avoiding player\n" );
		}
	}
	else if( tr.plane.normal[2] < 0.5f )
		leftFrac = tr.fraction;
	else
		leftFrac = 1.0f;

//	Com_Printf( "rightFrac %f, centerFrac %f, leftFrac %f\n", rightFrac, centerFrac, leftFrac );

	if( rightFrac < leftFrac )
	{
		VectorCopy( bs->cur_ps.viewangles, angles );
		angles[YAW] += 15;
//		Com_Printf( "Turning left\n" );
	}
	else if( rightFrac > leftFrac )
	{
		VectorCopy( bs->cur_ps.viewangles, angles );
		angles[YAW] += -15;
//		Com_Printf( "Turning right\n" );
	}

	if( centerFrac < 1.0f )
	{
//		Com_Printf( "Slowing down and possibly backing up\n" );
		return -1;
	}

	return throttleChange;
}


/*
==================
AINode_MoveToNextCheckpoint
==================
*/
int AINode_MoveToNextCheckpoint( bot_state_t *bs )
{
	vec3_t		origin, dir, angles, alpha, delta, cross;
	gentity_t	*next = NULL, *prev = NULL, *ent = NULL;
	int			nextCheckpoint;
	int			lastCheckpoint;
	float		f, dist, speed, actualSpeed, dot, curvature;
	//float		accel, a_normal;
	int			throttleChange;
	const ghostBotRoute_t *ghostRoute;
	const char *routeVariant = NULL;
	ghostDecisionState_t decisionState;
	float desiredOffset;
	float speedBias;
	bot_recovery_state_t recoveryState;
	bot_recovery_state_t previousRecoveryState;
	float routeDistanceFromCenter = 0.0f;
	qboolean collisionRiskActive = qfalse;
	qboolean lcsPredictedConflict = qfalse;
	const char *recoveryEvent = "";
	const char *recoveryTrigger = "";
	qboolean forwardLaunchPhase = qfalse;
	qboolean raceStartGateActive = qfalse;
	qboolean spawnInitPhase = qfalse;

	if (BotIsObserver(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Observer(bs, "moveToNextCheckpoint: observer");
		return qfalse;
	}
	//if in the intermission
	if (BotIntermission(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Intermission(bs, "moveToNextCheckpoint: intermission");
		return qfalse;
	}
	//respawn if dead
	if (BotIsDead(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Respawn(bs, "moveToNextCheckpoint: dead");
		return qfalse;
	}

	nextCheckpoint = bs->cur_ps.stats[STAT_NEXT_CHECKPOINT];
	lastCheckpoint = (nextCheckpoint - 1);
	if( lastCheckpoint < 1 )
		lastCheckpoint = level.numCheckpoints;

	if ( bs->entitynum >= 0 && bs->entitynum < level.maxclients ) {
		gentity_t *botEnt = &g_entities[bs->entitynum];
		if ( botEnt->client && botEnt->client->pers.vehicleClass[0] ) {
			routeVariant = botEnt->client->pers.vehicleClass;
		}
		if ( botEnt->client && level.time - botEnt->client->ghostSpawnTime <= GHOST_FORWARD_INIT_PHASE_MS ) {
			spawnInitPhase = qtrue;
		}
	}
	raceStartGateActive = ( level.startRaceTime <= 0 || level.time < level.startRaceTime ) ? qtrue : qfalse;
	forwardLaunchPhase = ( raceStartGateActive || spawnInitPhase ) ? qtrue : qfalse;

	{
		const botPathRoute_t *pathRoutes[BOT_PATH_LINE_FAMILY_COUNT];
		vec3_t baseTargetPoint;
		vec3_t lineTargetPoint;
		vec3_t blendedTargetPoint;
		vec3_t routeForward;
		vec3_t routeRight;
		float baseRouteSpeed = 0.0f;
		float lineRouteSpeed = 0.0f;
		float speedFromRoute;
		float baseCurvature = 0.0f;
		float cornerPhase = 0.0f;
		float routeBlendAlpha = 0.2f;
		float speedBlendAlpha = 0.2f;
		float stateSpeedBias = 0.0f;
		float selectedWidthLimit = 0.0f;
		float selectedLateralOffset = 0.0f;
		int preferredPathId = BOT_PATH_LINE_BASE;
		int selectedPathId = -1;
		int baseNodeIndex = -1;
		int baseLookAheadIndex = -1;
		int selectedNodeIndex = -1;
		int selectedLookAheadIndex = -1;
		int widthClampEvent = 0;
		int autoSpeedActive = 1;
		int targetSpeedOverrideActive = 0;
		botCollisionRisk_t pathCollisionRisk;
		bot_recovery_state_t pathRecoveryState;
		qboolean haveBaseGuidance = qfalse;
		qboolean haveSelectedGuidance = qfalse;

		pathRoutes[BOT_PATH_LINE_BASE] = G_BotPath_GetRouteByIndex( BOT_PATH_LINE_BASE );
		pathRoutes[BOT_PATH_LINE_AGGRESSIVE] = G_BotPath_GetRouteByIndex( BOT_PATH_LINE_AGGRESSIVE );
		pathRoutes[BOT_PATH_LINE_SAFE] = G_BotPath_GetRouteByIndex( BOT_PATH_LINE_SAFE );

		actualSpeed = VectorLength( bs->cur_ps.velocity );
		haveBaseGuidance = Bot_BuildBotPathGuidance( pathRoutes[BOT_PATH_LINE_BASE], bs, actualSpeed, baseTargetPoint,
			&baseRouteSpeed, &baseCurvature, &baseNodeIndex, &baseLookAheadIndex );

		if ( !haveBaseGuidance ) {
			int baseFallbackId = Bot_SelectBotPathRouteIdWithFallback( pathRoutes, BOT_PATH_LINE_BASE );
			if ( baseFallbackId >= 0 ) {
				haveBaseGuidance = Bot_BuildBotPathGuidance( pathRoutes[baseFallbackId], bs, actualSpeed, baseTargetPoint,
					&baseRouteSpeed, &baseCurvature, &baseNodeIndex, &baseLookAheadIndex );
			}
		}

		if ( haveBaseGuidance ) {
        // Segment-Direction des closest-Segments nutzen statt Bot?Lookahead-Vektor.
        // Das ergibt ein stabiles, routenparalleles Koordinatensystem unabhängig
        // von der aktuellen Bot-Position relativ zum Pfad.
        if ( baseNodeIndex >= 0 && baseNodeIndex < pathRoutes[BOT_PATH_LINE_BASE]->numSegments ) {
            VectorCopy( pathRoutes[BOT_PATH_LINE_BASE]->segments[baseNodeIndex].direction, routeForward );
            routeForward[2] = 0.0f;
        if ( VectorNormalize( routeForward ) <= 0.001f ) {
            VectorSubtract( baseTargetPoint, bs->cur_ps.origin, routeForward );
            routeForward[2] = 0.0f;
            VectorNormalize( routeForward );
        }
        } else {
            VectorSubtract( baseTargetPoint, bs->cur_ps.origin, routeForward );
            routeForward[2] = 0.0f;
        if ( VectorNormalize( routeForward ) <= 0.001f ) {
            VectorSet( routeForward, 1.0f, 0.0f, 0.0f );
        }
    }
            routeRight[0] = -routeForward[1];
            routeRight[1] = routeForward[0];
            routeRight[2] = 0.0f;

			pathRecoveryState = (bot_recovery_state_t)bs->ghostRecoveryState;
			if ( pathRecoveryState < BOT_RECOVERY_NONE || pathRecoveryState > BOT_RECOVERY_EMERGENCY_RESET_REQUEST ) {
				pathRecoveryState = BOT_RECOVERY_NONE;
			}

			Bot_PredictCollisionRisk( bs, routeForward, routeRight, 0.5f, 1.3f, &pathCollisionRisk );
			cornerPhase = baseCurvature * 2.2f;
			if ( cornerPhase > 1.0f ) {
				cornerPhase = 1.0f;
			}

			if ( pathRecoveryState != BOT_RECOVERY_NONE ) {
				preferredPathId = BOT_PATH_LINE_SAFE;
				decisionState = GHOST_DECISION_ABORT_OVERTAKE;
			} else if ( pathCollisionRisk.nearestAheadDist < 190.0f && pathCollisionRisk.nearestAheadRelSpeed > 40.0f && !pathCollisionRisk.abortOvertakeRecommended ) {
				preferredPathId = BOT_PATH_LINE_AGGRESSIVE;
				decisionState = GHOST_DECISION_PREPARE_OVERTAKE;
			} else if ( pathCollisionRisk.nearestBehindDist < 130.0f && pathCollisionRisk.nearestBehindRelSpeed > 60.0f ) {
				preferredPathId = BOT_PATH_LINE_SAFE;
				decisionState = GHOST_DECISION_DEFEND_LINE;
			} else {
				preferredPathId = BOT_PATH_LINE_BASE;
				decisionState = GHOST_DECISION_FOLLOW;
			}

			selectedPathId = Bot_SelectBotPathRouteIdWithFallback( pathRoutes, preferredPathId );
			if ( selectedPathId >= 0 ) {
				haveSelectedGuidance = Bot_BuildBotPathGuidance( pathRoutes[selectedPathId], bs, actualSpeed, lineTargetPoint,
					&lineRouteSpeed, NULL, &selectedNodeIndex, &selectedLookAheadIndex );
			}
			if ( !haveSelectedGuidance ) {
				VectorCopy( baseTargetPoint, lineTargetPoint );
				lineRouteSpeed = baseRouteSpeed;
				selectedPathId = BOT_PATH_LINE_BASE;
				selectedNodeIndex = baseNodeIndex;
				selectedLookAheadIndex = baseLookAheadIndex;
			}

			switch ( decisionState ) {
				case GHOST_DECISION_PREPARE_OVERTAKE:
				case GHOST_DECISION_OVERTAKE_INSIDE:
				case GHOST_DECISION_OVERTAKE_OUTSIDE:
					routeBlendAlpha = 0.68f;
					speedBlendAlpha = 0.64f;
					stateSpeedBias = 40.0f;
					break;
				case GHOST_DECISION_DEFEND_LINE:
					routeBlendAlpha = 0.58f;
					speedBlendAlpha = 0.50f;
					stateSpeedBias = -22.0f;
					break;
				case GHOST_DECISION_ABORT_OVERTAKE:
					routeBlendAlpha = 0.82f;
					speedBlendAlpha = 0.76f;
					stateSpeedBias = -72.0f;
					break;
				case GHOST_DECISION_FOLLOW:
				default:
					routeBlendAlpha = 0.24f;
					speedBlendAlpha = 0.24f;
					stateSpeedBias = 0.0f;
					break;
			}
			if ( selectedPathId == BOT_PATH_LINE_BASE ) {
				routeBlendAlpha *= 0.35f;
				speedBlendAlpha *= 0.35f;
			}
			if ( cornerPhase > 0.5f && preferredPathId == BOT_PATH_LINE_AGGRESSIVE ) {
				speedBlendAlpha *= 0.75f;
			}

			VectorCopy( baseTargetPoint, blendedTargetPoint );
			blendedTargetPoint[0] = baseTargetPoint[0] + ( lineTargetPoint[0] - baseTargetPoint[0] ) * routeBlendAlpha;
			blendedTargetPoint[1] = baseTargetPoint[1] + ( lineTargetPoint[1] - baseTargetPoint[1] ) * routeBlendAlpha;
			blendedTargetPoint[2] = baseTargetPoint[2] + ( lineTargetPoint[2] - baseTargetPoint[2] ) * routeBlendAlpha;
			if ( selectedPathId >= 0 && selectedPathId < BOT_PATH_LINE_FAMILY_COUNT &&
				pathRoutes[selectedPathId] && selectedNodeIndex >= 0 &&
				selectedNodeIndex < pathRoutes[selectedPathId]->numNodes ) {
				float effectiveWidth = pathRoutes[selectedPathId]->nodes[selectedNodeIndex].effectiveWidth;
				selectedWidthLimit = effectiveWidth * 0.5f - 32.0f;
				if ( selectedWidthLimit < 0.0f ) {
					selectedWidthLimit = 0.0f;
				}
			}
			selectedLateralOffset = DotProduct( routeRight, blendedTargetPoint ) - DotProduct( routeRight, baseTargetPoint );
			if ( selectedLateralOffset > selectedWidthLimit ) {
				widthClampEvent = 1;
				selectedLateralOffset = selectedWidthLimit;
			} else if ( selectedLateralOffset < -selectedWidthLimit ) {
				widthClampEvent = 1;
				selectedLateralOffset = -selectedWidthLimit;
			}
			if ( widthClampEvent ) {
				VectorMA( baseTargetPoint, selectedLateralOffset, routeRight, blendedTargetPoint );
			}

			if ( selectedPathId >= 0 && selectedPathId < BOT_PATH_LINE_FAMILY_COUNT &&
				pathRoutes[selectedPathId] && pathRoutes[selectedPathId]->numNodes > 1 &&
				selectedNodeIndex >= 0 ) {
				int speedProbeStart = selectedNodeIndex;
				int speedProbeEnd = selectedLookAheadIndex;
				int speedProbeIndex;
                
				if ( speedProbeStart >= pathRoutes[selectedPathId]->numNodes ) {
					speedProbeStart = pathRoutes[selectedPathId]->numNodes - 1;
				}
				if ( speedProbeEnd < speedProbeStart ) {
					speedProbeEnd = speedProbeStart;
				}
				if ( speedProbeEnd >= pathRoutes[selectedPathId]->numNodes ) {
					speedProbeEnd = pathRoutes[selectedPathId]->numNodes - 1;
				}
				autoSpeedActive = 1;
				targetSpeedOverrideActive = 0;

				for ( speedProbeIndex = speedProbeStart; speedProbeIndex <= speedProbeEnd; ++speedProbeIndex ) {
					if ( pathRoutes[selectedPathId]->nodes[speedProbeIndex].targetSpeed >= 0.0f ) {

						targetSpeedOverrideActive = 1;
						autoSpeedActive = 0;
						break;
					}
				}
			}

			speedFromRoute = baseRouteSpeed + ( lineRouteSpeed - baseRouteSpeed ) * speedBlendAlpha;
			speedFromRoute += stateSpeedBias;
			if ( speedFromRoute < 280.0f ) {
				speedFromRoute = 280.0f;
			}

            {
            float desiredLateralOffset;
            float blendFactor;

            desiredLateralOffset = DotProduct( routeRight, blendedTargetPoint )
                         - DotProduct( routeRight, baseTargetPoint );

            blendFactor = ( selectedPathId == BOT_PATH_LINE_BASE ) ? 0.22f : 0.35f;
            bs->botPathLateralOffset += ( desiredLateralOffset - bs->botPathLateralOffset ) * blendFactor;

            VectorMA( baseTargetPoint, bs->botPathLateralOffset, routeRight, blendedTargetPoint );
            }

			VectorSubtract( blendedTargetPoint, bs->cur_ps.origin, dir );
			dir[2] = 0.0f;
			vectoangles( dir, angles );

			if ( speedFromRoute >= actualSpeed + 55.0f ) {
				throttleChange = 1;
			} else if ( speedFromRoute + 95.0f <= actualSpeed ) {
				throttleChange = -1;
			} else {
				throttleChange = 0;
			}

			throttleChange = Bot_CheckForObstacles( bs, angles, throttleChange );
			VectorCopy( angles, bs->ideal_viewangles );
			{
				int objectiveState;
				int kothOwner;
				int kothContested;
				Bot_DebugGetObjectiveSnapshot( bs, &objectiveState, &kothOwner, &kothContested );
			Bot_DebugExportDmnetTick( bs, selectedLookAheadIndex, speedFromRoute, actualSpeed, decisionState,
				pathCollisionRisk.hasPredictedConflict, pathRecoveryState, pathRecoveryState, "", "",
				Distance( bs->cur_ps.origin, baseTargetPoint ), selectedPathId, selectedNodeIndex,
				selectedLookAheadIndex, widthClampEvent, autoSpeedActive, targetSpeedOverrideActive,
				forwardLaunchPhase ? 1 : 0, objectiveState, kothOwner, kothContested );
			}

			if ( throttleChange > 0 ) {
				trap_EA_MoveForward( bs->client );
			} else if ( throttleChange < 0 ) {
				trap_EA_MoveBack( bs->client );
			}
			return qtrue;
		}
	}

	if ( G_Ghost_GetBotRouteForVariant( routeVariant, &ghostRoute ) && ghostRoute ) {
		int bestIndex = -1;
		int i;
		int hintIndex = bs->ghostRouteIndexHint;
		vec3_t botForward;
		qboolean strictForwardOnly = forwardLaunchPhase;
		qboolean lapWrapWindow = qfalse;

		AngleVectors( bs->cur_ps.viewangles, botForward, NULL, NULL );
		botForward[2] = 0.0f;
		if ( VectorNormalize( botForward ) <= 0.001f ) {
			VectorSet( botForward, 1.0f, 0.0f, 0.0f );
		}

		if ( hintIndex >= 0 && ghostRoute->numWaypoints > 8 &&
			hintIndex >= ghostRoute->numWaypoints - 4 && nextCheckpoint <= 2 ) {
			hintIndex = -1;
			lapWrapWindow = qtrue;
		}

		bestIndex = Bot_SelectForwardWaypointIndex( ghostRoute, bs->cur_ps.origin, botForward, hintIndex,
			GHOST_ROUTE_HINT_WINDOW, strictForwardOnly );

		/* On the very first frame after spawn (hintIndex == -1, spawnInitPhase),
		   the forward-scored search can pick a waypoint slightly to the side,
		   causing an immediate right-swerve. Use the closest waypoint instead
		   so the bot starts straight and converges naturally. */
		if ( bestIndex < 0 || ( spawnInitPhase && hintIndex < 0 ) ) {
			int closestIndex = G_Ghost_SelectClosestWaypoint( ghostRoute, bs->cur_ps.origin, -1, ghostRoute->numWaypoints );
			if ( closestIndex >= 0 ) {
				bestIndex = closestIndex;
			}
		}
		if ( lapWrapWindow && bestIndex >= ghostRoute->numWaypoints - 4 ) {
			int wrapCandidate = Bot_SelectForwardWaypointIndex( ghostRoute, bs->cur_ps.origin, botForward, 0,
				GHOST_ROUTE_HINT_WINDOW * 2, qfalse );
			if ( wrapCandidate >= 0 && wrapCandidate < ghostRoute->numWaypoints - 4 ) {
				bestIndex = wrapCandidate;
			}
		}

		if ( bestIndex >= 0 ) {
			int lookAheadIndex = bestIndex;
			int speedStartIndex;
			int speedEndIndex = bestIndex;
			int segmentStartIndex;
			int segmentEndIndex;
			int lookAheadTime = ghostRoute->waypoints[bestIndex].timeOffset + GHOST_ROUTE_LOOKAHEAD_MS;
			float lookAheadDistanceSq = LOOKAHEAD_DISTANCE * LOOKAHEAD_DISTANCE;
			float cornerPhase = 0.0f;
			float avgCurvature = 0.0f;
			bs->ghostRouteIndexHint = bestIndex;

			for ( i = bestIndex + 1; i < ghostRoute->numWaypoints; ++i ) {
				vec3_t deltaToWaypoint;
				vec3_t toWaypoint;
				float distSq;
				float dotForward = 1.0f;
				VectorSubtract( ghostRoute->waypoints[i].origin, bs->cur_ps.origin, deltaToWaypoint );
				VectorCopy( deltaToWaypoint, toWaypoint );
				toWaypoint[2] = 0.0f;
				if ( VectorLengthSquared( toWaypoint ) > 1.0f ) {
					VectorNormalize( toWaypoint );
					dotForward = DotProduct( botForward, toWaypoint );
				}
				if ( dotForward < GHOST_FORWARD_DOT_SOFT_REJECT ) {
					continue;
				}
				lookAheadIndex = i;
				distSq = VectorLengthSquared( deltaToWaypoint );
				if ( ghostRoute->waypoints[i].timeOffset >= lookAheadTime || distSq >= lookAheadDistanceSq ) {
					break;
				}
			}

			actualSpeed = VectorLength( bs->cur_ps.velocity );

			speedStartIndex = bestIndex;
			if ( lookAheadIndex > speedStartIndex ) {
				speedEndIndex = lookAheadIndex - 1;
			}
			if ( speedEndIndex >= ghostRoute->numWaypoints - 1 ) {
				speedEndIndex = ghostRoute->numWaypoints - 2;
			}
			segmentStartIndex = speedStartIndex;
			segmentEndIndex = speedEndIndex;
			if ( segmentEndIndex >= ghostRoute->numSegments ) {
				segmentEndIndex = ghostRoute->numSegments - 1;
			}

			if ( ghostRoute->numSegments > 0 && segmentStartIndex >= 0 && segmentStartIndex <= segmentEndIndex ) {
				float segmentSpeedSum = 0.0f;
				float segmentCurvatureSum = 0.0f;
				int segmentSamples = 0;

				for ( i = segmentStartIndex; i <= segmentEndIndex; ++i ) {
					segmentSpeedSum += ghostRoute->segments[i].recommendedSpeed;
					segmentCurvatureSum += ghostRoute->segments[i].curvature;
					segmentSamples++;
				}

				if ( segmentSamples > 0 ) {
					speed = segmentSpeedSum / segmentSamples;
					avgCurvature = segmentCurvatureSum / segmentSamples;
				} else {
					speed = actualSpeed;
				}
			} else {
				speed = actualSpeed;
			}

			cornerPhase = avgCurvature * 2.2f;
			if ( cornerPhase > 1.0f ) {
				cornerPhase = 1.0f;
			}

			{
			vec3_t targetPoint;
			vec3_t routeForward;
			vec3_t routeRight;
			float routeForwardLen;
			float routeDistanceFromCenter;
			float brakeZone;
			float baseTargetOffset;
			float blendFactor;
			float lineSpeedScale;
			int preferredInside = qtrue;
			int segmentForProfile;
			botCollisionRisk_t collisionRisk;
			ghostRouteLineFamily_t selectedFamily = GHOST_LINE_BASE;
			qboolean chaosContext = qfalse;

			VectorSubtract( ghostRoute->waypoints[lookAheadIndex].origin, bs->cur_ps.origin, routeForward );
			routeForward[2] = 0;
			routeForwardLen = VectorNormalize( routeForward );
			if ( routeForwardLen <= 0.001f ) {
				VectorSet( routeForward, 1.0f, 0.0f, 0.0f );
			}
			routeRight[0] = -routeForward[1];
			routeRight[1] = routeForward[0];
			routeRight[2] = 0.0f;

			if ( speedEndIndex - speedStartIndex >= 1 ) {
				vec3_t segA, segB;
				float crossZ;
				VectorSubtract( ghostRoute->waypoints[speedStartIndex + 1].origin, ghostRoute->waypoints[speedStartIndex].origin, segA );
				VectorSubtract( ghostRoute->waypoints[speedEndIndex + 1].origin, ghostRoute->waypoints[speedEndIndex].origin, segB );
				crossZ = segA[0] * segB[1] - segA[1] * segB[0];
				preferredInside = ( crossZ >= 0.0f );
			}

			Bot_PredictCollisionRisk( bs, routeForward, routeRight, 0.5f, 1.5f, &collisionRisk );
			routeDistanceFromCenter = Distance( bs->cur_ps.origin, ghostRoute->waypoints[bestIndex].origin );
			recoveryState = (bot_recovery_state_t)bs->ghostRecoveryState;
			previousRecoveryState = recoveryState;
			if ( recoveryState < BOT_RECOVERY_NONE || recoveryState > BOT_RECOVERY_EMERGENCY_RESET_REQUEST ) {
				recoveryState = BOT_RECOVERY_NONE;
			}
			if ( lapWrapWindow && recoveryState != BOT_RECOVERY_NONE ) {
				Bot_SetRecoveryState( bs, BOT_RECOVERY_NONE );
				recoveryState = BOT_RECOVERY_NONE;
				recoveryEvent = "lap_wrap_recovery_clear";
				recoveryTrigger = "checkpoint_wrap";
			}

			if ( speed > actualSpeed + 80.0f ) {
				bs->ghostRecoveryThrottleIntentTime = FloatTime();
			}

			{
				float recoveryIdleTime = FloatTime() - bs->ghostRecoveryStateTime;
				qboolean recoveryArmed = ( recoveryState == BOT_RECOVERY_NONE &&
					recoveryIdleTime >= GHOST_RECOVERY_REARM_DELAY ) ? qtrue : qfalse;

				if ( recoveryArmed && !forwardLaunchPhase && FloatTime() - bs->ghostRecoveryLastSampleTime >= GHOST_RECOVERY_SAMPLE_WINDOW ) {
					float sampledProgress = Distance( bs->cur_ps.origin, bs->ghostRecoveryLastOrigin );
					if ( FloatTime() - bs->ghostRecoveryThrottleIntentTime < GHOST_RECOVERY_SAMPLE_WINDOW + 0.15f &&
						sampledProgress < GHOST_RECOVERY_MIN_PROGRESS ) {
						Bot_SetRecoveryState( bs, BOT_RECOVERY_STUCK_DETECT );
						recoveryState = BOT_RECOVERY_STUCK_DETECT;
						recoveryEvent = "stuck_detect";
						recoveryTrigger = "low_progress_under_throttle";
					}
					VectorCopy( bs->cur_ps.origin, bs->ghostRecoveryLastOrigin );
					bs->ghostRecoveryLastSampleTime = FloatTime();
				}

				if ( forwardLaunchPhase ) {
					/* Reset during launch phase so count doesn't burst when phase ends */
					bs->ghostRecoveryCollisionCount = 0;
				} else if ( collisionRisk.hasPredictedConflict && collisionRisk.nearestAheadDist < 90.0f && fabs( collisionRisk.nearestAheadLateral ) < 75.0f ) {
					bs->ghostRecoveryCollisionCount++;
				} else if ( bs->ghostRecoveryCollisionCount > 0 ) {
					bs->ghostRecoveryCollisionCount--;
				}

				if ( recoveryArmed && !lapWrapWindow && routeDistanceFromCenter > GHOST_RECOVERY_ROUTE_DIST_THRESHOLD ) {
					Bot_SetRecoveryState( bs, BOT_RECOVERY_REJOIN_ROUTE );
					recoveryState = BOT_RECOVERY_REJOIN_ROUTE;
					recoveryEvent = "off_route_rejoin";
					recoveryTrigger = "route_deviation";
				}

				/* Only trigger collision recovery if the bot is actually unable to move.
				   At race start all bots cluster together causing false collision pressure. */
				if ( recoveryArmed && !forwardLaunchPhase && bs->ghostRecoveryCollisionCount >= GHOST_RECOVERY_MAX_COLLISION_COUNT
					&& actualSpeed < 80.0f ) {
					Bot_SetRecoveryState( bs, BOT_RECOVERY_REVERSE_UNWIND );
					recoveryState = BOT_RECOVERY_REVERSE_UNWIND;
					recoveryEvent = "collision_reverse";
					recoveryTrigger = "collision_pressure";
				}
			}

			brakeZone = ( actualSpeed > speed * 1.08f || cornerPhase > 0.55f ) ? 1.0f : 0.0f;
			chaosContext = ( collisionRisk.hasPredictedConflict || collisionRisk.abortOvertakeRecommended || brakeZone > 0.8f ) ? qtrue : qfalse;
			decisionState = (ghostDecisionState_t)bs->ghostDecisionState;
			if ( decisionState < GHOST_DECISION_FOLLOW || decisionState > GHOST_DECISION_ABORT_OVERTAKE ) {
				decisionState = GHOST_DECISION_FOLLOW;
			}
			desiredOffset = 0.0f;
			speedBias = 0.0f;
			{
				float overtakeEnterDist = 170.0f + ( bs->personalityOvertakeBias * 60.0f );
				float overtakeEnterRelSpeed = 60.0f - ( bs->personalityOvertakeBias * 25.0f );
				float defendRelSpeed = 95.0f - ( bs->personalityRisk * 35.0f );
				float abortDist = 25.0f + ( ( 1.0f - bs->personalityRisk ) * 18.0f );
				float sideSafetyThreshold = 66.0f - ( bs->personalityRisk * 28.0f );
				float abortBrakeZone = 0.88f - ( bs->personalityRisk * 0.20f );
				float followClearDist = 240.0f + ( bs->personalityOvertakeBias * 55.0f );

				switch ( decisionState ) {
					default:
					case GHOST_DECISION_FOLLOW:
						if ( collisionRisk.nearestAheadDist < overtakeEnterDist && collisionRisk.nearestAheadRelSpeed > overtakeEnterRelSpeed && brakeZone < 0.5f ) {
							decisionState = GHOST_DECISION_PREPARE_OVERTAKE;
						} else if ( collisionRisk.nearestBehindDist < 120.0f && collisionRisk.nearestBehindRelSpeed > defendRelSpeed && ( brakeZone > 0.5f || cornerPhase > 0.35f ) ) {
							decisionState = GHOST_DECISION_DEFEND_LINE;
						} else if ( collisionRisk.hasPredictedConflict ) {
							decisionState = GHOST_DECISION_ABORT_OVERTAKE;
						}
						break;

					case GHOST_DECISION_PREPARE_OVERTAKE:
						if ( collisionRisk.nearestAheadDist > followClearDist || collisionRisk.nearestAheadRelSpeed < 5.0f ) {
							decisionState = GHOST_DECISION_FOLLOW;
						} else if ( brakeZone > 0.5f || collisionRisk.abortOvertakeRecommended ) {
							decisionState = GHOST_DECISION_ABORT_OVERTAKE;
						} else {
							if ( preferredInside ) {
								decisionState = GHOST_DECISION_OVERTAKE_INSIDE;
							} else {
								decisionState = GHOST_DECISION_OVERTAKE_OUTSIDE;
							}
						}
						break;

					case GHOST_DECISION_OVERTAKE_INSIDE:
					case GHOST_DECISION_OVERTAKE_OUTSIDE:
					{
						qboolean sideBlocked;
						sideBlocked = ( decisionState == GHOST_DECISION_OVERTAKE_INSIDE ) ? ( collisionRisk.sideSafetyInside < sideSafetyThreshold ) : ( collisionRisk.sideSafetyOutside < sideSafetyThreshold );
						if ( collisionRisk.nearestAheadDist < abortDist || sideBlocked || brakeZone > abortBrakeZone || collisionRisk.abortOvertakeRecommended ) {
							decisionState = GHOST_DECISION_ABORT_OVERTAKE;
						} else if ( collisionRisk.nearestAheadDist > followClearDist + 15.0f || collisionRisk.nearestAheadRelSpeed < -20.0f ) {
							decisionState = GHOST_DECISION_FOLLOW;
						}
						break;
					}

					case GHOST_DECISION_DEFEND_LINE:
						if ( collisionRisk.nearestBehindDist > 220.0f || collisionRisk.nearestBehindRelSpeed < 25.0f ) {
							decisionState = GHOST_DECISION_FOLLOW;
						}
						break;

					case GHOST_DECISION_ABORT_OVERTAKE:
						if ( collisionRisk.nearestAheadDist > 140.0f || brakeZone > 0.5f ) {
							decisionState = GHOST_DECISION_FOLLOW;
						}
						break;
				}
				if ( gametype == GT_LCS ) {
					const char *breakTrigger = NULL;
					if ( decisionState == GHOST_DECISION_PREPARE_OVERTAKE ||
						decisionState == GHOST_DECISION_OVERTAKE_INSIDE ||
						decisionState == GHOST_DECISION_OVERTAKE_OUTSIDE ) {
						decisionState = GHOST_DECISION_DEFEND_LINE;
					}
					if ( Bot_LcsShouldBreakEngagement( bs, &collisionRisk, &breakTrigger ) ) {
						decisionState = GHOST_DECISION_ABORT_OVERTAKE;
						if ( !recoveryEvent[0] ) {
							recoveryEvent = "break_engagement";
						}
						recoveryTrigger = breakTrigger ? breakTrigger : "lcs_break";
					}
				}
			}

			if ( decisionState != (ghostDecisionState_t)bs->ghostDecisionState ) {
				bs->ghostDecisionState = decisionState;
				bs->ghostDecisionStateTime = FloatTime();
			}

			switch ( decisionState ) {
				case GHOST_DECISION_PREPARE_OVERTAKE:
					desiredOffset = ( collisionRisk.nearestAheadLateral >= 0.0f ) ? -32.0f : 32.0f;
					speedBias = 25.0f + ( bs->personalityOvertakeBias * 30.0f );
					break;
				case GHOST_DECISION_OVERTAKE_INSIDE:
					desiredOffset = preferredInside ? -72.0f : 72.0f;
					speedBias = 35.0f + ( bs->personalityRisk * 45.0f );
					break;
				case GHOST_DECISION_OVERTAKE_OUTSIDE:
					desiredOffset = preferredInside ? 72.0f : -72.0f;
					speedBias = 10.0f + ( bs->personalityOvertakeBias * 25.0f );
					break;
				case GHOST_DECISION_DEFEND_LINE:
					desiredOffset = ( collisionRisk.nearestBehindLateral >= 0.0f ) ? -46.0f : 46.0f;
					speedBias = -25.0f;
					break;
				case GHOST_DECISION_ABORT_OVERTAKE:
					desiredOffset = 0.0f;
					speedBias = -90.0f;
					break;
				case GHOST_DECISION_FOLLOW:
				default:
					desiredOffset = 0.0f;
					speedBias = 0.0f;
					break;
			}

			selectedFamily = Bot_SelectGhostLineFamily( &collisionRisk, cornerPhase, chaosContext );
			if ( decisionState == GHOST_DECISION_PREPARE_OVERTAKE ||
				decisionState == GHOST_DECISION_OVERTAKE_INSIDE ||
				decisionState == GHOST_DECISION_OVERTAKE_OUTSIDE ) {
				selectedFamily = GHOST_LINE_RACE;
			} else if ( decisionState == GHOST_DECISION_DEFEND_LINE ) {
				selectedFamily = GHOST_LINE_DEFENSIVE;
			} else if ( decisionState == GHOST_DECISION_ABORT_OVERTAKE ) {
				selectedFamily = GHOST_LINE_SAFE;
			}
			if ( gametype == GT_LCS ) {
				if ( selectedFamily == GHOST_LINE_RACE ) {
					selectedFamily = GHOST_LINE_DEFENSIVE;
				}
			}

			baseTargetOffset = 0.0f;
			lineSpeedScale = 1.0f;
			if ( ghostRoute->numSegments > 0 ) {
				segmentForProfile = speedStartIndex;
				if ( segmentForProfile < 0 ) {
					segmentForProfile = 0;
				} else if ( segmentForProfile >= ghostRoute->numSegments ) {
					segmentForProfile = ghostRoute->numSegments - 1;
				}
				baseTargetOffset = ghostRoute->segments[segmentForProfile].lines[selectedFamily].lateralOffset;
				lineSpeedScale = ghostRoute->segments[segmentForProfile].lines[selectedFamily].speedScale;
			}

				if ( collisionRisk.hasPredictedConflict ) {
					if ( collisionRisk.laneSwapRecommended ) {
					desiredOffset = ( preferredInside ? 72.0f : -72.0f );
				} else if ( collisionRisk.sideSafetyInside > collisionRisk.sideSafetyOutside + 5.0f ) {
					desiredOffset = -68.0f;
				} else if ( collisionRisk.sideSafetyOutside > collisionRisk.sideSafetyInside + 5.0f ) {
					desiredOffset = 68.0f;
				}
					speedBias += collisionRisk.recommendedSpeedBias;
				}
				collisionRiskActive = collisionRisk.hasPredictedConflict;
				lcsPredictedConflict = collisionRisk.hasPredictedConflict;

			blendFactor = ( selectedFamily == GHOST_LINE_BASE ) ? 0.22f : 0.35f;
			bs->ghostDecisionLateralOffset += ( ( baseTargetOffset + desiredOffset ) - bs->ghostDecisionLateralOffset ) * blendFactor;
			VectorMA( ghostRoute->waypoints[lookAheadIndex].origin, bs->ghostDecisionLateralOffset, routeRight, targetPoint );
			VectorSubtract( targetPoint, bs->cur_ps.origin, dir );
			dir[2] = 0;
			speed *= lineSpeedScale;
			speed += speedBias;
			if ( speed < 300.0f ) {
				speed = 300.0f;
			}
			}

			/*
			 * Additional damping to reduce throttle oscillation:
			 * - low-pass filter target speed across think ticks
			 * - clamp max per-tick speed target delta
			 * - hysteresis around throttle changes
			 */
			if ( !bs->ghostTargetSpeedValid ) {
				bs->ghostTargetSpeedFiltered = speed;
				bs->ghostTargetSpeedValid = qtrue;
			} else {
				float maxDeltaPerTick = 220.0f * ( bs->thinktime > 0.0f ? bs->thinktime : 0.1f );
				float filteredTarget = bs->ghostTargetSpeedFiltered + ( speed - bs->ghostTargetSpeedFiltered ) * 0.25f;
				float delta = filteredTarget - bs->ghostTargetSpeedFiltered;

				if ( delta > maxDeltaPerTick ) {
					filteredTarget = bs->ghostTargetSpeedFiltered + maxDeltaPerTick;
				} else if ( delta < -maxDeltaPerTick ) {
					filteredTarget = bs->ghostTargetSpeedFiltered - maxDeltaPerTick;
				}

				bs->ghostTargetSpeedFiltered = filteredTarget;
			}
			speed = bs->ghostTargetSpeedFiltered;

			vectoangles( dir, angles );
			{
				float speedError = speed - actualSpeed;

				if ( speedError > 80.0f ) {
					throttleChange = 1;
				} else if ( speedError < -140.0f ) {
					throttleChange = -1;
				} else {
					throttleChange = 0;
				}
			}

			switch ( recoveryState ) {
				case BOT_RECOVERY_STUCK_DETECT:
					if ( FloatTime() - bs->ghostRecoveryStateTime > 0.35f ) {
						Bot_SetRecoveryState( bs, BOT_RECOVERY_REVERSE_UNWIND );
						recoveryState = BOT_RECOVERY_REVERSE_UNWIND;
						recoveryEvent = "stuck_to_reverse";
						recoveryTrigger = "stuck_settle_timeout";
					}
					throttleChange = 0;
					break;

				case BOT_RECOVERY_REVERSE_UNWIND:
				{
					float reverseTime = FloatTime() - bs->ghostRecoveryStateTime;
					float reverseProgress = Distance( bs->cur_ps.origin, bs->ghostRecoveryReverseStartOrigin );
					int reverseCollisionGain = bs->ghostRecoveryCollisionCount - bs->ghostRecoveryReverseStartCollisionCount;
					if ( reverseProgress < GHOST_RECOVERY_REVERSE_MIN_PROGRESS &&
						reverseCollisionGain >= GHOST_RECOVERY_REVERSE_COLLISION_SPIKE ) {
						if ( bs->ghostRecoveryReverseCycles >= GHOST_RECOVERY_MAX_REVERSE_CYCLES ) {
							Bot_SetRecoveryState( bs, BOT_RECOVERY_EMERGENCY_RESET_REQUEST );
							recoveryState = BOT_RECOVERY_EMERGENCY_RESET_REQUEST;
							recoveryEvent = "reverse_escalate_reset";
							recoveryTrigger = "reverse_no_progress_collision_spike";
						} else {
							Bot_SetRecoveryState( bs, BOT_RECOVERY_REJOIN_ROUTE );
							recoveryState = BOT_RECOVERY_REJOIN_ROUTE;
							bs->ghostRecoveryThrottleRamp = 0.0f;
							recoveryEvent = "reverse_escalate_rejoin";
							recoveryTrigger = "reverse_no_progress_collision_spike";
						}
					} else if ( reverseTime > GHOST_RECOVERY_MAX_REVERSE_TIME ) {
						Bot_SetRecoveryState( bs, BOT_RECOVERY_REJOIN_ROUTE );
						recoveryState = BOT_RECOVERY_REJOIN_ROUTE;
						bs->ghostRecoveryThrottleRamp = 0.0f;
						recoveryEvent = "reverse_to_rejoin";
						recoveryTrigger = "reverse_timeout";
					} else {
						vec3_t reverseDir;
						VectorSubtract( bs->cur_ps.origin, ghostRoute->waypoints[lookAheadIndex].origin, reverseDir );
						reverseDir[2] = 0.0f;
						vectoangles( reverseDir, angles );
						throttleChange = -1;
					}
					break;
				}

				case BOT_RECOVERY_REJOIN_ROUTE:
				{
					float currentYaw = bs->cur_ps.viewangles[YAW];
					float desiredYaw = angles[YAW];
					float steerLimit = GHOST_RECOVERY_REJOIN_STEER_LIMIT;
					float throttleForRamp;
					if ( gametype == GT_LCS ) {
						steerLimit *= 0.85f;
					}
					angles[YAW] = Bot_ClampSteeringToRecoveryLimit( currentYaw, desiredYaw, steerLimit );
					bs->ghostRecoveryThrottleRamp += GHOST_RECOVERY_REJOIN_THROTTLE_STEP;
					if ( bs->ghostRecoveryThrottleRamp > 1.0f ) {
						bs->ghostRecoveryThrottleRamp = 1.0f;
					}

					if ( gametype == GT_LCS && lcsPredictedConflict ) {

						bs->ghostRecoveryThrottleRamp -= 0.20f;
						if ( bs->ghostRecoveryThrottleRamp < 0.0f ) {
							bs->ghostRecoveryThrottleRamp = 0.0f;
						}
						if ( !recoveryEvent[0] ) {
							recoveryEvent = "rejoin_hold_avoid_contact";
							recoveryTrigger = "collision_conflict";
						}
					}
					throttleForRamp = bs->ghostRecoveryThrottleRamp;
					if ( throttleForRamp < ( gametype == GT_LCS ? 0.48f : 0.35f ) ) {
						throttleChange = 0;
					} else {
						throttleChange = 1;
					}
					if ( routeDistanceFromCenter < GHOST_RECOVERY_ROUTE_DIST_THRESHOLD * ( gametype == GT_LCS ? 0.50f : 0.58f ) &&
						bs->ghostRecoveryCollisionCount <= ( gametype == GT_LCS ? 0 : 1 ) ) {
						Bot_SetRecoveryState( bs, BOT_RECOVERY_NONE );
						recoveryState = BOT_RECOVERY_NONE;
						recoveryEvent = "rejoin_complete";
						recoveryTrigger = "route_centered";
					}
					break;
				}

				case BOT_RECOVERY_EMERGENCY_RESET_REQUEST:
					/* Keep ghostRouteIndexHint at current bestIndex - clearing to -1
					   causes the waypoint selector to jump to end-of-route waypoints */
					bs->ghostRouteIndexHint = bestIndex;
					bs->ghostDecisionLateralOffset = 0.0f;
					Bot_SetRecoveryState( bs, BOT_RECOVERY_NONE );
					recoveryState = BOT_RECOVERY_NONE;
					throttleChange = 1;
					break;

				case BOT_RECOVERY_NONE:
				default:
					break;
			}

			if ( ( recoveryState == BOT_RECOVERY_REVERSE_UNWIND || recoveryState == BOT_RECOVERY_REJOIN_ROUTE ) &&
				FloatTime() - bs->ghostRecoveryStateTime > 3.5f ) {
				if ( recoveryState == BOT_RECOVERY_REVERSE_UNWIND &&
					bs->ghostRecoveryReverseCycles < GHOST_RECOVERY_MAX_REVERSE_CYCLES ) {
					Bot_SetRecoveryState( bs, BOT_RECOVERY_REJOIN_ROUTE );
					recoveryState = BOT_RECOVERY_REJOIN_ROUTE;
					recoveryEvent = "recovery_timeout_rejoin";
					recoveryTrigger = "recovery_timeout";
				} else {
					Bot_SetRecoveryState( bs, BOT_RECOVERY_EMERGENCY_RESET_REQUEST );
					recoveryState = BOT_RECOVERY_EMERGENCY_RESET_REQUEST;
					recoveryEvent = "recovery_timeout";
					recoveryTrigger = "recovery_timeout";
				}
			}
			if ( !recoveryEvent[0] && recoveryState != previousRecoveryState ) {
				recoveryEvent = "state_changed";
				recoveryTrigger = "state_transition";
			}
			if ( spawnInitPhase && throttleChange < 0 ) {
				throttleChange = 0;
			}

			throttleChange = Bot_CheckForObstacles( bs, angles, throttleChange );
			VectorCopy( angles, bs->ideal_viewangles );
			{
				int objectiveState;
				int kothOwner;
				int kothContested;
				Bot_DebugGetObjectiveSnapshot( bs, &objectiveState, &kothOwner, &kothContested );
			Bot_DebugExportDmnetTick( bs, bestIndex, speed, actualSpeed, decisionState, collisionRiskActive,
				recoveryState, previousRecoveryState, recoveryEvent, recoveryTrigger, routeDistanceFromCenter,
				-1, bestIndex, lookAheadIndex, 0, 1, 0, forwardLaunchPhase ? 1 : 0,
				objectiveState, kothOwner, kothContested );
			}

			if( throttleChange > 0 )
				trap_EA_MoveForward( bs->client );
			else if( throttleChange < 0 )
				trap_EA_MoveBack( bs->client );

			return qtrue;
		}
	}


	/* Ghost guidance not active this tick, reset speed filter state. */
	bs->ghostTargetSpeedValid = qfalse;
	bs->ghostRouteIndexHint = -1;
	bs->ghostDecisionState = GHOST_DECISION_FOLLOW;
	bs->ghostDecisionStateTime = 0.0f;
	bs->ghostDecisionLateralOffset = 0.0f;
	bs->ghostRecoveryState = BOT_RECOVERY_NONE;
	bs->ghostRecoveryStateTime = 0.0f;
	bs->ghostRecoveryCollisionCount = 0;
	bs->ghostRecoveryThrottleRamp = 0.0f;
	bs->ghostRecoveryReverseCycles = 0;
	bs->ghostRecoveryReverseWindowStart = 0.0f;
	VectorCopy( bs->cur_ps.origin, bs->ghostRecoveryReverseStartOrigin );
	bs->ghostRecoveryReverseStartCollisionCount = 0;

	while ((ent = G_Find (ent, FOFS(classname), "rally_checkpoint")) != NULL)
	{
		if( ent->number == nextCheckpoint )
			next = ent;

		if( ent->number == lastCheckpoint )
			prev = ent;

		if( next && prev )
			break;
	}

	if( !next || !prev )
		return qtrue;

	f = SHORT2FLOAT(bs->cur_ps.stats[STAT_FRAC_TO_NEXT_CHECKPOINT]);

//	Com_Printf( "Next checkpoint: %i, Frac %f, Short %i, val %f\n", bs->cur_ps.stats[STAT_NEXT_CHECKPOINT], f, bs->cur_ps.stats[STAT_FRAC_TO_NEXT_CHECKPOINT], SHORT2FLOAT(FLOAT2SHORT(1.0f)) );

//	Com_Printf( "f %f\n", f );

	// Bot goes backward if f is 0 or 1 so limit range.
	// origin is fine but derivitives seem to be invalid.
	// --zturtleman
	if ( f > 0.95f ) {
		f = 0.95f;
	} else 	if ( f < 0.05f ) {
		f = 0.05f;
	}

	G_GetPointOnCurveBetweenCheckpoints( prev, next, f, origin );
	G_Get2ndDervOnCurveBetweenCheckpoints( prev, next, f, alpha );
	G_GetDervOnCurveBetweenCheckpoints( prev, next, f, delta );
	VectorSubtract( origin, bs->cur_ps.origin, dir );

	dir[2] = 0;
	dist = VectorLengthSquared( dir );
//	Com_Printf( "dist %f\n", dist );
	if( dist < 400000.0f )
	{
		f += 0.1f;
		if( f > 1.0f )
			f = 1.0f;
		g_entities[bs->client].client->ps.stats[STAT_FRAC_TO_NEXT_CHECKPOINT] = FLOAT2SHORT(f);
	}

	CrossProduct( delta, alpha, cross );
	dot = VectorLength( cross );

	speed = VectorNormalize( delta );
/*
	accel = VectorLength( alpha );
	dot = DotProduct( delta, alpha );
	a_normal = sqrt( accel*accel - dot*dot );
	curvature = a_normal / (speed*speed);
*/
	curvature = dot / (speed*speed*speed);

	if( curvature != 0.0f )
		speed = sqrt( CP_CURRENT_GRAVITY / curvature );
	else
		speed = 1000000.0f;

	actualSpeed = VectorLength( bs->cur_ps.velocity );
//	Com_Printf( "Radius: %f\n", 1 / curvature );
//	Com_Printf( "Recommended Speed %f, actual speed %f\n", speed, actualSpeed );

	vectoangles( dir, angles );

	if( speed >= actualSpeed )
		throttleChange = 1;
	else if( speed + 100 <= actualSpeed )
	{
		throttleChange = -1;
//		Com_Printf( "Slowing Down\n" );
	}
	else
	{
		throttleChange = 0;
//		Com_Printf( "Coasting\n" );
	}

	throttleChange = Bot_CheckForObstacles( bs, angles, throttleChange );
	VectorCopy( angles, bs->ideal_viewangles );

	if( throttleChange > 0 )
		trap_EA_MoveForward( bs->client );
	else if( throttleChange < 0 )
		trap_EA_MoveBack( bs->client );
//	else
//		coast

	return qtrue;
}
// END
