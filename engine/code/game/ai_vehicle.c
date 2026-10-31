/*
 * Q3Rally vehicle driver. The planner chooses a destination and speed; this
 * module owns the steering/throttle command, independently of weapon aiming.
 * Copyright (C) 2026 Q3Rally Team. GPL version 2 or later.
 */
#include "g_local.h"
#include "../botlib/botlib.h"
#include "../botlib/be_aas.h"
#include "../botlib/be_ai_goal.h"
#include "../botlib/be_ai_move.h"
#include "ai_main.h"
#include "ai_dmq3.h"

static qboolean BotVehicleCanLeaveOverlap( bot_state_t *bs, const vec3_t direction,
	const vec3_t mins, const vec3_t maxs ) {
	vec3_t trial, away, nextAway;
	qboolean overlaps = qfalse;
	int client, axis;
	VectorMA( bs->cur_ps.origin, 40, direction, trial );
	for ( client = 0; client < level.maxclients; client++ ) {
		gentity_t *other = &g_entities[client];
		if ( client == bs->client || !other->inuse || !other->client ||
			!(other->r.contents & CONTENTS_BODY) ) continue;
		for ( axis = 0; axis < 3; axis++ ) {
			if ( bs->cur_ps.origin[axis] + maxs[axis] < other->r.absmin[axis] ||
				bs->cur_ps.origin[axis] + mins[axis] > other->r.absmax[axis] ) break;
		}
		if ( axis != 3 ) continue;
		overlaps = qtrue;
		VectorSubtract( bs->cur_ps.origin, other->r.currentOrigin, away );
		VectorSubtract( trial, other->r.currentOrigin, nextAway );
		away[2] = nextAway[2] = 0;
		if ( VectorLengthSquared( nextAway ) <= VectorLengthSquared( away ) + Square( 8 ) ) return qfalse;
	}
	return overlaps;
}

static float BotVehicleClearance( bot_state_t *bs, float yaw, float distance,
	const trace_t *road, qboolean brakingCorridor, int *blockEntity ) {
	vec3_t angles, forward, right, start, end, mins, maxs, probe, support, normal;
	trace_t trace, centerGround;
	float clearance;

	VectorSet( angles, 0, yaw, 0 );
	AngleVectors( angles, forward, NULL, NULL );
	VectorSet( right, -forward[1], forward[0], 0 );
	VectorCopy( bs->cur_ps.origin, start );
	/* Sweep the vehicle width, with enough vertical tolerance for road seams. */
	VectorSet( mins, -CAR_WIDTH * 0.5f - 4, -CAR_WIDTH * 0.5f - 4, -CAR_HEIGHT * 0.5f + 8 );
	VectorSet( maxs, CAR_WIDTH * 0.5f + 4, CAR_WIDTH * 0.5f + 4, CAR_HEIGHT * 0.5f - 2 );
	VectorMA( start, distance + (CAR_LENGTH - CAR_WIDTH) * 0.5f, forward, end );
	trap_Trace( &trace, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID | CONTENTS_BOTCLIP );
	clearance = trace.fraction;
	if ( trace.startsolid || trace.allsolid ) {
		clearance = 0;
		/* Permit an escape from overlapping engine boxes only when the world
		 * sweep is clear and the move increases distance to every such car. */
		if ( trace.startsolid && !trace.allsolid && trace.fraction >= 1 &&
			BotVehicleCanLeaveOverlap( bs, forward, mins, maxs ) ) {
			trap_Trace( &trace, start, mins, maxs, end, bs->client,
				(MASK_PLAYERSOLID | CONTENTS_BOTCLIP) & ~CONTENTS_BODY );
			if ( !trace.startsolid && !trace.allsolid ) clearance = trace.fraction * 0.7f;
		}
	} else if ( trace.fraction < 1 && trace.plane.normal[2] > 0.55f ) {
		clearance = 1; /* The swept lower corner may touch an uphill road. */
	}
	if ( blockEntity ) {
		*blockEntity = trace.entityNum;
	}
	if ( road ) {
		float probeDistance = distance * clearance;
		int sample, lane;
		qboolean unsafe = qfalse;
		VectorCopy( road->endpos, support );
		VectorCopy( road->plane.normal, normal );
		/* Check the braking corridor, not only the first 180 units. Sampling
		 * along it also detects a pit before a safe-looking far bank. */
		if ( probeDistance < 70 ) probeDistance = 70;
		for ( sample = 1; sample <= 3; sample++ ) {
			VectorMA( bs->cur_ps.origin, probeDistance * sample / 3, forward, probe );
			for ( lane = 0; lane < 3; lane++ ) {
				float predictedHeight;
				VectorMA( probe, lane == 1 ? CAR_WIDTH * 0.5f : lane == 2 ? -CAR_WIDTH * 0.5f : 0, right, start );
				predictedHeight = support[2] - ((start[0] - support[0]) * normal[0] +
					(start[1] - support[1]) * normal[1]) / normal[2];
				start[2] = predictedHeight + 64;
				VectorCopy( start, end );
				end[2] = predictedHeight - 112;
				trap_Trace( &trace, start, NULL, NULL, end, bs->client,
					(MASK_PLAYERSOLID | CONTENTS_BOTCLIP) & ~CONTENTS_BODY );
				if ( trace.fraction >= 1 || trace.startsolid || trace.plane.normal[2] < 0.45f ||
					(trap_PointContents( trace.endpos, bs->client ) & (CONTENTS_LAVA | CONTENTS_SLIME)) ) {
					unsafe = qtrue;
					break;
				}
				if ( lane == 0 ) centerGround = trace;
			}
			if ( unsafe ) {
				/* Forward braking uses the remaining road distance. Detour and
				 * reverse lanes must be safe over their entire requested length. */
				clearance = brakingCorridor ? Com_Clamp( 0, clearance,
					(probeDistance * sample / 3 - 70) / distance ) : 0;
				if ( blockEntity ) *blockEntity = ENTITYNUM_WORLD;
				break;
			}
			VectorCopy( centerGround.endpos, support );
			VectorCopy( centerGround.plane.normal, normal );
		}
	}
	return clearance;
}

static qboolean BotVehicleBlockerIsEnemy( bot_state_t *bs, int entityNum ) {
	if ( entityNum < 0 || entityNum >= ENTITYNUM_WORLD || bs->enemy < 0 ) return qfalse;
	if ( g_entities[entityNum].flags & FL_EXTRA_BBOX ) entityNum = g_entities[entityNum].r.ownerNum;
	return entityNum == bs->enemy && entityNum >= 0 && entityNum < MAX_CLIENTS &&
		g_entities[entityNum].client != NULL;
}

void BotVehicleDrive( bot_state_t *bs, const vec3_t target, float targetSpeed,
	bot_moveresult_t *result ) {
	bot_vehicle_state_t *drive = &bs->vehicle;
	vec3_t delta, angles, forward, end;
	trace_t ground;
	const trace_t *road;
	float speed, yaw, horizon, sideHorizon, brakeHorizon, front, left, right, rear, targetClear, yawError;
	qboolean onRoad;
	int blockEntity = ENTITYNUM_NONE;
	int targetBlockEntity = ENTITYNUM_NONE;
	int oldCommandTime = drive->commandTime;

	if ( (gametype != GT_DEATHMATCH && gametype != GT_CTF) || bs->cur_ps.pm_type != PM_NORMAL ) {
		return;
	}
	speed = VectorLength( bs->cur_ps.velocity );
	VectorSubtract( target, bs->cur_ps.origin, delta );
	delta[2] = 0;
	yaw = VectorLengthSquared( delta ) > 1 ? vectoyaw( delta ) : bs->cur_ps.viewangles[YAW];
	targetSpeed = Com_Clamp( 0, 1400, targetSpeed );
	/* Stop requests and node changes do not inherit a previous movement command. */
	drive->active = qtrue;
	drive->commandTime = level.time;
	if ( !drive->progressTime || level.time - oldCommandTime > 500 ) {
		drive->progressTime = level.time;
		VectorCopy( bs->cur_ps.origin, drive->progressOrigin );
		drive->recovery = BOT_VEHICLE_DRIVE;
		drive->avoidUntil = 0;
	}
	VectorCopy( bs->cur_ps.origin, end );
	end[2] -= 100;
	/* Overlapping vehicle bounds must not hide the road and disable recovery. */
	trap_Trace( &ground, bs->cur_ps.origin, NULL, NULL, end, bs->client,
		(MASK_PLAYERSOLID | CONTENTS_BOTCLIP) & ~CONTENTS_BODY );
	onRoad = ground.fraction < 1 && ground.plane.normal[2] > 0.45f;
	drive->onRoad = onRoad;
	road = onRoad ? &ground : NULL;
	horizon = Com_Clamp( 160, 850, 160 + speed * 0.30f + speed * speed / 4000.0f );
	/* Tight CTF arenas need local escape checks; a full-speed braking horizon
	 * often includes several trees even when the immediate turn is open. */
	sideHorizon = gametype == GT_CTF ? Com_Clamp( 160, 320, horizon * 0.5f ) : horizon;
	brakeHorizon = horizon;
	front = BotVehicleClearance( bs, bs->cur_ps.viewangles[YAW], horizon, road, qtrue, &blockEntity );
	left = BotVehicleClearance( bs, bs->cur_ps.viewangles[YAW] + 50, sideHorizon, road, qfalse, NULL );
	right = BotVehicleClearance( bs, bs->cur_ps.viewangles[YAW] - 50, sideHorizon, road, qfalse, NULL );
	rear = BotVehicleClearance( bs, bs->cur_ps.viewangles[YAW] + 180, 150, road, qfalse, NULL );
	targetClear = BotVehicleClearance( bs, yaw, horizon, road, qfalse, &targetBlockEntity );
	drive->leftClearance = left;
	drive->rightClearance = right;
	drive->rearClearance = rear;
	drive->targetClearance = targetClear;
	drive->targetBlockerEntity = targetBlockEntity;
	/* A distant opponent in our firing lane is not yet an immediate detour.
	 * Keep closing with braking room; close contact, other cars and unsafe
	 * ground still use the normal, continuously checked avoidance below. */
	if ( onRoad && fabs( AngleSubtract( yaw, bs->cur_ps.viewangles[YAW] ) ) < 18 &&
		front * horizon > 140 && BotVehicleBlockerIsEnemy( bs, blockEntity ) &&
		(targetClear >= 0.6f || (targetClear * horizon > 140 &&
		BotVehicleBlockerIsEnemy( bs, targetBlockEntity ))) ) {
		float closingSpeed = sqrt( 2 * 1800.0f * (front * horizon - 100) );
		if ( targetSpeed > closingSpeed ) targetSpeed = closingSpeed;
		front = 1;
		if ( BotVehicleBlockerIsEnemy( bs, targetBlockEntity ) ) targetClear = 1;
	}
	drive->blocked = front < 0.85f || targetClear < 0.6f;
	drive->clearance = front;
	if ( targetSpeed <= 0 ) {
		drive->targetSpeed = 0;
		drive->desiredYaw = bs->cur_ps.viewangles[YAW];
		drive->progressTime = level.time;
		drive->recovery = BOT_VEHICLE_DRIVE;
		return;
	}
	if ( DistanceSquared( bs->cur_ps.origin, drive->progressOrigin ) > Square( 48 ) ) {
		drive->progressTime = level.time;
		VectorCopy( bs->cur_ps.origin, drive->progressOrigin );
		if ( drive->recovery == BOT_VEHICLE_DRIVE ) drive->recoveries = 0;
	}
	if ( drive->recovery != BOT_VEHICLE_DRIVE && level.time >= drive->recoveryUntil ) {
		drive->recovery = drive->recovery == BOT_VEHICLE_REVERSE ? BOT_VEHICLE_TURN : BOT_VEHICLE_DRIVE;
		drive->recoveryUntil = level.time + 800;
		drive->progressTime = level.time;
		VectorCopy( bs->cur_ps.origin, drive->progressOrigin );
	}
	if ( drive->recovery == BOT_VEHICLE_DRIVE && onRoad && speed < 100 &&
		level.time - drive->progressTime > 1200 ) {
		drive->turnSign = left > right + 0.05f ? 1 : right > left + 0.05f ? -1 :
			(drive->turnSign ? -drive->turnSign : (bs->client & 1) ? 1 : -1);
		drive->recovery = rear > 0.55f ? BOT_VEHICLE_REVERSE : BOT_VEHICLE_TURN;
		drive->recoveryUntil = level.time + 850;
		drive->recoveries++;
		if ( drive->recoveries >= 3 ) {
			/* Ask the bot to reconsider its route after repeated failed recovery. */
			trap_BotResetAvoidReach( bs->ms );
			bs->ltg_time = 0;
			bs->nbg_time = 0;
			drive->recoveries = 0;
		}
	}
	if ( drive->recovery == BOT_VEHICLE_REVERSE ) {
		/* Nose target, not travel direction: mouse steering reverses in reverse gear. */
		yaw = bs->cur_ps.viewangles[YAW] + drive->turnSign * 35;
		{
			float reverseRoom = rear * 150 - 18;
			targetSpeed = reverseRoom > 0 ? -Com_Clamp( 0, 220,
				sqrt( 2 * 1000.0f * reverseRoom ) ) : 0;
		}
	} else if ( drive->recovery == BOT_VEHICLE_TURN ) {
		float turnClear = BotVehicleClearance( bs, bs->cur_ps.viewangles[YAW] +
			drive->turnSign * 50, 180, road, qfalse, NULL );
		float turnRoom = turnClear * 180 - 18;
		yaw = bs->cur_ps.viewangles[YAW] + drive->turnSign * 50;
		targetSpeed = turnRoom > 0 ? Com_Clamp( 0, 220,
			sqrt( 2 * 1000.0f * turnRoom ) ) : 0;
	} else {
		if ( front < 0.85f || targetClear < 0.6f ) {
			float blockedCorridor = front < targetClear ? front : targetClear;
			float detourThreshold = gametype == GT_CTF ? 0.10f : 0.35f;
			float detourMargin = gametype == GT_CTF ? 0.05f : 0.10f;
			int side;
			float freeSide;
			/* Commit briefly to a safe passing side. Tiny changes in vehicle
			 * positions must not alternate steering every think frame. */
			if ( level.time < drive->avoidUntil &&
				(drive->avoidSign > 0 ? left : right) > blockedCorridor + 0.10f &&
				(drive->avoidSign > 0 ? left : right) > 0.35f ) {
				side = drive->avoidSign;
			} else {
				float targetTurn = AngleSubtract( yaw, bs->cur_ps.viewangles[YAW] );
				if ( fabs( left - right ) < 0.12f ) {
					side = fabs( targetTurn ) > 10 ? (targetTurn > 0 ? 1 : -1) :
						((bs->client & 1) ? 1 : -1);
				} else side = left > right ? 1 : -1;
			}
			freeSide = side > 0 ? left : right;
			/* A preferred side never overrides the floor/clearance checks. */
			if ( freeSide <= detourThreshold || freeSide <= blockedCorridor + detourMargin ) {
				side = -side;
				freeSide = side > 0 ? left : right;
			}
			if ( freeSide > blockedCorridor + detourMargin && freeSide > detourThreshold ) {
				if ( side != drive->avoidSign || level.time >= drive->avoidUntil ) {
					drive->avoidSign = side;
					drive->avoidUntil = level.time + 650;
				}
				yaw = bs->cur_ps.viewangles[YAW] + side * 50;
				if ( gametype == GT_CTF ) targetSpeed = targetSpeed > 260 ? 260 : targetSpeed;
				else targetSpeed = targetSpeed > 520 ? 520 : targetSpeed;
				targetClear = freeSide;
				brakeHorizon = sideHorizon;
			} else if ( front >= 0.85f ) {
				/* A clear current lane does not make an unsafe requested turn
				 * safe. Stay in this lane until a checked detour is available. */
				yaw = bs->cur_ps.viewangles[YAW];
				if ( targetSpeed > 340 ) targetSpeed = 340;
				targetClear = front;
			} else {
				targetSpeed *= front;
				targetClear = front;
			}
		}
		yawError = fabs( AngleSubtract( yaw, bs->cur_ps.viewangles[YAW] ) );
		if ( yawError > 70 && targetSpeed > 340 ) targetSpeed = 340;
		else if ( yawError > 35 && targetSpeed > 600 ) targetSpeed = 600;
		/* Brake against the corridor we're actually steering into. Using the
		 * blocked straight-ahead lane here cancels every safe side detour. */
		if ( targetClear < 0.85f ) {
			float safeDistance = targetClear * brakeHorizon -
				(gametype == GT_CTF ? 18 : 45);
			float safeSpeed = safeDistance > 0 ? sqrt( 2 * 1800.0f * safeDistance ) : 0;
			if ( targetSpeed > safeSpeed ) targetSpeed = safeSpeed;
		}
	}
	/* WheelAngle folds targets beyond 90 degrees. Ask for a turning arc, never
	 * a backward-looking mouse angle that would straighten the wheels. */
	drive->desiredYaw = AngleNormalize360( bs->cur_ps.viewangles[YAW] +
		Com_Clamp( -70, 70, AngleSubtract( yaw, bs->cur_ps.viewangles[YAW] ) ) );
	drive->targetSpeed = targetSpeed;
	VectorSet( angles, 0, drive->desiredYaw, 0 );
	VectorCopy( angles, bs->ideal_viewangles );
	AngleVectors( angles, forward, NULL, NULL );
	if ( result ) {
		VectorCopy( forward, result->movedir );
		result->blocked = drive->blocked;
		result->blockentity = blockEntity;
	}
}

static int BotVehicleFindCTFReentry( bot_state_t *bs, bot_goal_t *goal,
	int travelFlags, vec3_t target ) {
	static const vec3_t directions[8] = {
		{ 1, 0, 0 }, { 0.7071f, 0.7071f, 0 }, { 0, 1, 0 },
		{ -0.7071f, 0.7071f, 0 }, { -1, 0, 0 }, { -0.7071f, -0.7071f, 0 },
		{ 0, -1, 0 }, { 0.7071f, -0.7071f, 0 }
	};
	vec3_t candidate, delta;
	float bestScore = 1.0e30f;
	int bestArea = 0;
	int radius, direction;

	if ( !goal || !goal->areanum ) return 0;
	for ( radius = 64; radius <= 256; radius += 64 ) {
		for ( direction = 0; direction < 8; direction++ ) {
			int area, travelTime;
			float distance, score;
			VectorMA( bs->cur_ps.origin, radius, directions[direction], candidate );
			area = BotPointAreaNum( candidate );
			if ( !area ) continue;
			travelTime = trap_AAS_AreaTravelTimeToGoalArea( area, candidate,
				goal->areanum, travelFlags );
			if ( travelTime <= 0 && area != goal->areanum ) continue;
			VectorSubtract( candidate, bs->cur_ps.origin, delta );
			distance = VectorLength( delta );
			score = (travelTime > 0 ? travelTime : 1) + distance * 0.5f;
			if ( score < bestScore ) {
				bestScore = score;
				bestArea = area;
				VectorCopy( candidate, target );
			}
		}
	}
	return bestArea;
}

void BotVehicleMoveToGoal( bot_state_t *bs, bot_moveresult_t *result,
	bot_goal_t *goal, int travelFlags ) {
	vec3_t target, direction;
	float lookAhead, targetSpeed;
	int reentryArea;

	trap_BotMoveToGoal( result, bs->ms, goal, travelFlags );
	if ( gametype != GT_DEATHMATCH && gametype != GT_CTF ) return;
	bs->vehicle.reentryArea = 0;
	if ( gametype == GT_CTF && !BotPointAreaNum( bs->cur_ps.origin ) &&
		(reentryArea = BotVehicleFindCTFReentry( bs, goal, travelFlags, target )) != 0 ) {
		/* The vehicle has drifted outside AAS. Rejoin the closest area that
		 * still has a route to the current flag before resuming normal guidance. */
		bs->vehicle.reentryArea = reentryArea;
		BotVehicleDrive( bs, target, 260, result );
		return;
	}
	lookAhead = Com_Clamp( 120, 300, 120 + VectorLength( bs->cur_ps.velocity ) * 0.15f );
	if ( !trap_BotMovementViewTarget( bs->ms, goal, travelFlags, lookAhead, target ) ) {
		VectorCopy( result->movedir, direction );
		if ( VectorNormalize( direction ) > 0.01f ) VectorMA( bs->origin, lookAhead, direction, target );
		else VectorCopy( goal->origin, target );
	}
	if ( gametype == GT_CTF ) {
		/* CTF bots use vehicle steering too, but their AAS goals are foot-scale
		 * and the maps are tighter than the open DM arenas. Keep their cruise
		 * speed conservative while BotVehicleDrive handles collision recovery. */
		targetSpeed = 600 + 50 * Com_Clamp( 1, 5, bs->settings.skill );
	} else {
		targetSpeed = 650 + 70 * Com_Clamp( 1, 5, bs->settings.skill );
	}
	if ( DistanceSquared( bs->origin, goal->origin ) < Square( 160 ) ) targetSpeed = 300;
	if ( result->flags & MOVERESULT_WAITING ) targetSpeed = 0;
	BotVehicleDrive( bs, target, targetSpeed, result );
}

void BotVehicleUpdateInput( bot_state_t *bs, usercmd_t *cmd ) {
	bot_vehicle_state_t *drive = &bs->vehicle;
	playerState_t *ps;
	vec3_t angles, forward;
	float speed, throttle, steering;
	int i;

	if ( (gametype != GT_DEATHMATCH && gametype != GT_CTF) || !drive->active ) return;
	if ( level.time - drive->commandTime > 300 || bs->cur_ps.pm_type != PM_NORMAL ||
		bs->client < 0 || bs->client >= level.maxclients || !g_entities[bs->client].client ) {
		cmd->forwardmove = cmd->rightmove = 0;
		drive->active = qfalse;
		return;
	}
	ps = &g_entities[bs->client].client->ps;
	if ( ps->pm_type != PM_NORMAL || ps->stats[STAT_HEALTH] <= 0 ) {
		cmd->forwardmove = cmd->rightmove = 0;
		drive->active = qfalse;
		return;
	}
	VectorSet( angles, 0, ps->viewangles[YAW], 0 );
	AngleVectors( angles, forward, NULL, NULL );
	speed = DotProduct( ps->velocity, forward );
	throttle = (drive->targetSpeed - speed) / 220;
	/* A stop request brakes while moving, then holds without backing up. */
	if ( drive->targetSpeed == 0 && fabs( speed ) < 35 ) throttle = 0;
	cmd->forwardmove = (signed char)(127 * Com_Clamp( -1, 1, throttle ));
	cmd->rightmove = 0;
	if ( g_entities[bs->client].client->pers.controlMode == CT_MOUSE ) {
		VectorSet( angles, 0, drive->desiredYaw, 0 );
		for ( i = 0; i < 3; i++ ) cmd->angles[i] = ANGLE2SHORT( angles[i] ) - ps->delta_angles[i];
	} else {
		steering = WheelAngle( ps->viewangles[YAW], drive->desiredYaw );
		if ( ps->stats[STAT_GEAR] < 0 ) steering = -steering;
		cmd->rightmove = (signed char)(127 * Com_Clamp( -1, 1, steering / 20 ));
	}
	cmd->buttons &= ~BUTTON_HANDBRAKE;
	if ( drive->targetSpeed == 0 && fabs( speed ) < 35 ) cmd->buttons |= BUTTON_HANDBRAKE;
	if ( level.time - drive->lastLogTime >= 500 ) {
		drive->lastLogTime = level.time;
		if ( trap_Cvar_VariableIntegerValue( "g_botDmDebug" ) ) {
			float aimError = 0;
			if ( bs->enemy >= 0 ) {
				VectorSubtract( bs->aimtarget, ps->origin, forward );
				aimError = fabs( AngleSubtract( vectoyaw( forward ), ps->viewangles[YAW] ) );
			}
			G_LogPrintf( "BotDM: time=%d client=%d enemy=%d phase=%d recovery=%d speed=%.1f targetSpeed=%.1f blocked=%d firing=%d aimError=%.1f hits=%d kills=%d deaths=%d x=%.1f y=%.1f z=%.1f weapon=%d health=%d shots=%d shotHits=%d avoid=%d fuel=%d vx=%.1f vy=%.1f vz=%.1f yaw=%.1f gear=%d\n",
				level.time, bs->client, bs->enemy, bs->rallyCombatPhase, drive->recovery,
				fabs( speed ), drive->targetSpeed, drive->blocked, (cmd->buttons & BUTTON_ATTACK) ? 1 : 0,
				aimError, ps->persistant[PERS_HITS], bs->num_kills, bs->num_deaths,
				ps->origin[0], ps->origin[1], ps->origin[2], bs->weaponnum, ps->stats[STAT_HEALTH],
				g_entities[bs->client].client->accuracy_shots, g_entities[bs->client].client->accuracy_hits,
				drive->avoidSign, ps->stats[STAT_FUEL], ps->velocity[0], ps->velocity[1], ps->velocity[2],
				ps->viewangles[YAW], ps->stats[STAT_GEAR] );
		}
	}
}
