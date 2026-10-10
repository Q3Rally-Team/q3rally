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
Autoball client side: match state, score bug, goal banner, ball indicator
and ball camera.

The indicator and the ball camera also work outside GT_AUTOBALL whenever a
test ball (ball_spawn) is in the snapshot, so the physics can be tried on
any map.

Cvars:  cg_autoballCam        0/1, the "ballcam" command toggles it
        cg_autoballIndicator  0/1, arrow/bracket that shows where the ball is
        cg_autoballShake      camera shake strength on goal blasts (0 = off)
        cg_autoballTrail      0/1, glowing trail behind a fast ball, fire above 150 km/h
===========================================================================
*/

#include "cg_local.h"

#define AB_SET4(v,a,b,c,d)	((v)[0]=(a),(v)[1]=(b),(v)[2]=(c),(v)[3]=(d))
#define AUTOBALL_BALL_RADIUS		75.0f
#define AUTOBALL_CAM_BLEND_MSEC		200.0f
#define AUTOBALL_CAM_MIN_DIST		120.0f	/* closer than this the yaw would spin */

/*
================
CG_Autoball_FindBall

The match ball from CS_AUTOBALLSTATUS, otherwise the first test ball in the
snapshot. Balls are SVF_BROADCAST, so they are always in the snapshot.
================
*/
centity_t *CG_Autoball_FindBall( void ) {
	int i;

	if ( !cg.snap )
		return NULL;
	if ( cgs.gametype == GT_AUTOBALL && cgs.autoballBallNum >= MAX_CLIENTS &&
		cgs.autoballBallNum < MAX_GENTITIES ) {
		centity_t *cent = &cg_entities[cgs.autoballBallNum];
		if ( cent->currentValid && cent->currentState.eType == ET_SCRIPTED )
			return cent;
	}
	for ( i = 0; i < cg.snap->numEntities; i++ ) {
		entityState_t *es = &cg.snap->entities[i];
		if ( es->eType == ET_SCRIPTED && ( es->generic1 & SCRIPTED_GENERIC1_NO_PREDICT ) )
			return &cg_entities[es->number];
	}
	return NULL;
}

/*
================
CG_ParseAutoballStatus

"state kickoffEnd ballEntity goalTeam scorerClient goalSpeedKmh"
================
*/
void CG_ParseAutoballStatus( void ) {
	char buffer[MAX_STRING_CHARS];
	char *cursor;
	int values[6];
	int i, oldState;

	Q_strncpyz( buffer, CG_ConfigString( CS_AUTOBALLSTATUS ), sizeof( buffer ) );
	cursor = buffer;
	for ( i = 0; i < 6; i++ ) {
		char *token = COM_Parse( &cursor );
		if ( !token[0] )
			return;
		values[i] = atoi( token );
	}

	oldState = cgs.autoballState;
	cgs.autoballState = values[0];
	cgs.autoballKickoffEnd = values[1];
	cgs.autoballBallNum = values[2];
	cgs.autoballGoalTeam = values[3];
	cgs.autoballScorer = values[4];
	cgs.autoballGoalSpeed = values[5];
	if ( cgs.autoballState == AUTOBALL_STATE_GOAL && oldState != AUTOBALL_STATE_GOAL )
		cgs.autoballGoalTime = cg.time;

	/* optional: goal centres (red x y z, blue x y z), intro end */
	cgs.autoballHaveGoals = qfalse;
	cgs.autoballIntroEnd = 0;
	for ( i = 0; i < 6; i++ ) {
		char *token = COM_Parse( &cursor );
		if ( !token[0] )
			return;
		cgs.autoballGoal[i / 3][i % 3] = atof( token );
	}
	if ( Distance( cgs.autoballGoal[0], cgs.autoballGoal[1] ) > 64.0f )
		cgs.autoballHaveGoals = qtrue;
	cgs.autoballIntroEnd = atoi( COM_Parse( &cursor ) );
}

/* Kick-off: hold the predicted car exactly like the server does. */
void CG_Autoball_FreezeCommand( usercmd_t *cmd ) {
	if ( cgs.gametype != GT_AUTOBALL || cgs.autoballState != AUTOBALL_STATE_KICKOFF )
		return;
	if ( cmd->serverTime >= cgs.autoballKickoffEnd )
		return;
	cmd->buttons = BUTTON_HANDBRAKE;
	cmd->forwardmove = 0;
	cmd->upmove = 0;
}

void CG_Autoball_ToggleCam_f( void ) {
	trap_Cvar_Set( "cg_autoballCam", cg_autoballCam.integer ? "0" : "1" );
	CG_Printf( "Ball camera %s\n", cg_autoballCam.integer ? "off" : "on" );
}

/*
================
CG_Autoball_ApplyBallCam

Called with cg.refdef.vieworg at the car and cg.refdefViewAngles holding the
normal chase angles, right before the third-person offset. Turns the chase
camera towards the ball, blending in and out over AUTOBALL_CAM_BLEND_MSEC.
================
*/
void CG_Autoball_ApplyBallCam( void ) {
	static float blend;
	static float ballYaw, ballPitch;
	centity_t *ball;
	float target, step;

	ball = cg_autoballCam.integer ? CG_Autoball_FindBall() : NULL;
	target = ball ? 1.0f : 0.0f;
	step = cg.frametime / AUTOBALL_CAM_BLEND_MSEC;
	if ( blend < target ) {
		blend += step;
		if ( blend > target )
			blend = target;
	} else if ( blend > target ) {
		blend -= step;
		if ( blend < target )
			blend = target;
	}
	if ( blend <= 0.0f )
		return;

	if ( ball ) {
		vec3_t delta, angles;
		float horizontal;

		VectorSubtract( ball->lerpOrigin, cg.refdef.vieworg, delta );
		horizontal = sqrt( delta[0] * delta[0] + delta[1] * delta[1] );
		if ( horizontal > AUTOBALL_CAM_MIN_DIST ) {
			vectoangles( delta, angles );
			ballYaw = angles[YAW];
			ballPitch = AngleNormalize180( angles[PITCH] );
			if ( ballPitch < -30.0f )
				ballPitch = -30.0f;
			if ( ballPitch > 20.0f )
				ballPitch = 20.0f;
		}
	}
	cg.refdefViewAngles[YAW] = LerpAngle( cg.refdefViewAngles[YAW], ballYaw, blend );
	cg.refdefViewAngles[PITCH] = LerpAngle( cg.refdefViewAngles[PITCH], ballPitch, blend );
}

/*
================
HUD helpers
================
*/
static void CG_Autoball_TeamColor( int team, float alpha, vec4_t out ) {
	if ( team == TEAM_BLUE ) {
		out[0] = 0.16f; out[1] = 0.38f; out[2] = 0.92f;
	} else {
		out[0] = 0.88f; out[1] = 0.18f; out[2] = 0.16f;
	}
	out[3] = alpha;
}

static const char *CG_Autoball_SpeedText( int kmh ) {
	if ( cg_metricUnits.integer )
		return va( "%i km/h", kmh );
	return va( "%i mph", (int)( kmh * 0.621371f + 0.5f ) );
}

static void CG_Autoball_DrawScoreBug( void ) {
	vec4_t red, blue, center, white, accent;
	int msec, seconds;
	qboolean overtime = qfalse;
	char clock[16];
	const float y = 6.0f, h = 30.0f, teamW = 56.0f, clockW = 84.0f;
	float x = 320.0f - ( teamW + clockW / 2.0f );

	CG_Autoball_TeamColor( TEAM_RED, 0.85f, red );
	CG_Autoball_TeamColor( TEAM_BLUE, 0.85f, blue );
	AB_SET4( center, 0.02f, 0.03f, 0.04f, 0.72f );
	AB_SET4( white, 1.0f, 1.0f, 1.0f, 1.0f );
	AB_SET4( accent, 1.0f, 0.78f, 0.2f, 1.0f );

	/* before the first whistle the clock stands (start time lies ahead) */
	msec = cg.time - cgs.levelStartTime;
	if ( msec < 0 || cgs.autoballState == AUTOBALL_STATE_WAITING )
		msec = 0;
	if ( cgs.timelimit > 0 ) {
		msec = cgs.timelimit * 60000 - msec;
		if ( msec < 0 ) {
			overtime = qtrue;
			msec = -msec;
		}
	}
	if ( msec < 0 )
		msec = 0;
	seconds = msec / 1000;
	/* own buffer: va() only rotates two buffers, the score va() calls below
	   would overwrite the clock text */
	Com_sprintf( clock, sizeof( clock ), "%s%i:%02i", overtime ? "+" : "", seconds / 60, seconds % 60 );

	CG_FillRect( x, y, teamW, h, red );
	CG_FillRect( x + teamW, y, clockW, h, center );
	CG_FillRect( x + teamW + clockW, y, teamW, h, blue );
	CG_DrawIngameString( (int)( x + teamW / 2 ), (int)( y + 4 ), va( "%i", cgs.scores1 ),
		UI_CENTER | UI_DROPSHADOW, 1.0f, white );
	CG_DrawIngameString( (int)( x + teamW + clockW + teamW / 2 ), (int)( y + 4 ), va( "%i", cgs.scores2 ),
		UI_CENTER | UI_DROPSHADOW, 1.0f, white );
	CG_DrawIngameString( 320, (int)( y + 8 ), clock, UI_CENTER | UI_SMALLFONT, 1.0f,
		overtime ? accent : white );
	if ( overtime ) {
		CG_DrawIngameString( 320, (int)( y + h + 3 ), "OVERTIME", UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW,
			0.75f, accent );
	} else if ( cgs.autoballState == AUTOBALL_STATE_KICKOFF ) {
		CG_DrawIngameString( 320, (int)( y + h + 3 ), "KICK-OFF", UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW,
			0.75f, white );
	}
}

static void CG_Autoball_DrawGoalBanner( void ) {
	vec4_t color, white;
	int elapsed;
	float alpha, scale;
	const char *who;

	if ( cgs.autoballState != AUTOBALL_STATE_GOAL )
		return;
	elapsed = cg.time - cgs.autoballGoalTime;
	if ( elapsed < 0 )
		elapsed = 0;
	alpha = elapsed < 3000 ? 1.0f : 1.0f - ( elapsed - 3000 ) / 1000.0f;
	if ( alpha <= 0.0f )
		return;
	/* a short punch-in, then steady */
	scale = elapsed < 250 ? 1.0f + ( 250 - elapsed ) / 250.0f * 0.25f : 1.0f;	/* stays inside 4:3 */

	CG_Autoball_TeamColor( cgs.autoballGoalTeam, alpha, color );
	AB_SET4( white, 1.0f, 1.0f, 1.0f, alpha );
	CG_DrawIngameString( 320, 140, cgs.autoballGoalTeam == TEAM_BLUE ? "BLUE SCORES!" : "RED SCORES!",
		UI_CENTER | UI_DROPSHADOW, 1.6f * scale, color );

	/* scorer: client number, -2 own goal, -1 nobody credited (old touch) */
	if ( cgs.autoballScorer >= 0 && cgs.autoballScorer < MAX_CLIENTS &&
		cgs.clientinfo[cgs.autoballScorer].infoValid ) {
		who = cgs.clientinfo[cgs.autoballScorer].name;
	} else if ( cgs.autoballScorer == -2 ) {
		who = "Own goal";
	} else {
		who = NULL;
	}
	if ( who ) {
		CG_DrawIngameString( 320, 184, va( "%s^7  -  %s", who, CG_Autoball_SpeedText( cgs.autoballGoalSpeed ) ),
			UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, 1.0f, white );
	} else {
		CG_DrawIngameString( 320, 184, CG_Autoball_SpeedText( cgs.autoballGoalSpeed ),
			UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, 1.0f, white );
	}
}

/*
================
CG_Autoball_DrawIndicator

On screen: four corner brackets around the ball. Off screen: a marker on the
screen edge in the ball's direction, with the distance.
================
*/
/* ball radius from the server (g_autoballBallScale), 75 for old servers */
static float CG_Autoball_Radius( const centity_t *cent ) {
	return cent->currentState.time2 > 0 ? (float)cent->currentState.time2 : AUTOBALL_BALL_RADIUS;
}

static void CG_Autoball_DrawIndicator( void ) {
	centity_t *ball;
	vec3_t delta;
	vec4_t color, shadow;
	float forward, left, up, focalX, focalY, sx, sy, dist;

	ball = CG_Autoball_FindBall();
	if ( !ball )
		return;

	VectorSubtract( ball->lerpOrigin, cg.refdef.vieworg, delta );
	forward = DotProduct( delta, cg.refdef.viewaxis[0] );
	left = DotProduct( delta, cg.refdef.viewaxis[1] );
	up = DotProduct( delta, cg.refdef.viewaxis[2] );
	dist = VectorLength( delta ) / CP_M_2_QU;
	focalX = 320.0f / tan( cg.refdef.fov_x * M_PI / 360.0f );
	focalY = 240.0f / tan( cg.refdef.fov_y * M_PI / 360.0f );

	AB_SET4( color, 1.0f, 0.62f, 0.12f, 0.9f );
	AB_SET4( shadow, 0.0f, 0.0f, 0.0f, 0.6f );
	CG_SetScreenPlacement( PLACE_STRETCH, PLACE_STRETCH );

	if ( forward > 1.0f ) {
		sx = 320.0f - left / forward * focalX;
		sy = 240.0f - up / forward * focalY;
		if ( sx > 24.0f && sx < 616.0f && sy > 24.0f && sy < 456.0f ) {
			float r = CG_Autoball_Radius( ball ) / forward * focalX * 1.15f;
			float len;
			if ( r < 8.0f )
				r = 8.0f;
			if ( r > 120.0f )
				r = 120.0f;
			len = r * 0.4f;
			/* four corner brackets */
			CG_FillRect( sx - r, sy - r, len, 2, color );
			CG_FillRect( sx - r, sy - r, 2, len, color );
			CG_FillRect( sx + r - len, sy - r, len, 2, color );
			CG_FillRect( sx + r - 2, sy - r, 2, len, color );
			CG_FillRect( sx - r, sy + r - 2, len, 2, color );
			CG_FillRect( sx - r, sy + r - len, 2, len, color );
			CG_FillRect( sx + r - len, sy + r - 2, len, 2, color );
			CG_FillRect( sx + r - 2, sy + r - len, 2, len, color );
			if ( dist > 25.0f ) {
				CG_DrawIngameString( (int)sx, (int)( sy + r + 4 ), va( "%im", (int)dist ),
					UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, 0.7f, color );
			}
			CG_PopScreenPlacement();
			return;
		}
	}

	{
		/* screen-space direction: right = -left, down = -up */
		float dx = -left, dy = -up, len, t;
		const char *arrow;

		if ( forward <= 0.0f && fabs( dx ) < 1.0f && fabs( dy ) < 1.0f )
			dy = 1.0f;
		len = sqrt( dx * dx + dy * dy );
		if ( len < 0.001f ) {
			dx = 0.0f;
			dy = 1.0f;
			len = 1.0f;
		}
		dx /= len;
		dy /= len;
		t = 1e9f;
		if ( fabs( dx ) > 0.001f )
			t = 280.0f / fabs( dx );
		if ( fabs( dy ) > 0.001f && 196.0f / fabs( dy ) < t )
			t = 196.0f / fabs( dy );
		sx = 320.0f + dx * t;
		sy = 240.0f + dy * t;

		if ( fabs( dx ) > fabs( dy ) )
			arrow = dx > 0.0f ? ">" : "<";
		else
			arrow = dy > 0.0f ? "v" : "^";

		CG_FillRect( sx - 15, sy - 15, 30, 30, shadow );
		CG_DrawRect( sx - 15, sy - 15, 30, 30, 2, color );
		CG_DrawIngameString( (int)sx, (int)( sy - 9 ), arrow, UI_CENTER, 0.75f, color );
		CG_DrawIngameString( (int)sx, (int)( sy + ( dy > 0.5f ? -32 : 18 ) ), va( "%im", (int)dist ),
			UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW, 0.7f, color );
	}
	CG_PopScreenPlacement();
}

/*
================
CG_Autoball_BallShadow

Dark disc straight below the ball, smaller and fainter the higher it flies,
so players can judge where an airborne ball will land.
================
*/
#define AUTOBALL_SHADOW_RANGE	1600.0f

void CG_Autoball_BallShadow( centity_t *cent ) {
	trace_t trace;
	vec3_t end;
	float height, frac, alpha;

	static qhandle_t shadowShader;

	if ( !shadowShader )
		shadowShader = trap_R_RegisterShader( "autoballShadow" );	/* round, not the car's box */
	if ( !shadowShader )
		return;
	VectorCopy( cent->lerpOrigin, end );
	end[2] -= AUTOBALL_SHADOW_RANGE;
	/* world only: a car under the ball must not swallow the shadow */
	trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, vec3_origin, vec3_origin, 0, MASK_SOLID );
	if ( trace.fraction >= 1.0f || trace.startsolid )
		return;
	height = trace.fraction * AUTOBALL_SHADOW_RANGE;
	frac = 1.0f - height / AUTOBALL_SHADOW_RANGE;
	/* the shadow shader darkens by colour, not alpha */
	alpha = 0.35f + 0.5f * frac;
	CG_ImpactMark( shadowShader, trace.endpos, trace.plane.normal, 0,
		alpha, alpha, alpha, 1.0f, qfalse,
		/* the soft round sprite fades out early: a bigger mark, same visible size */
		CG_Autoball_Radius( cent ) * ( 0.9f + 0.75f * frac ), qtrue );
}

/* seam glow colour of the ball: last touching team, dim white for none */
void CG_Autoball_BallColor( const entityState_t *s, byte *rgba ) {
	int team = ( s->generic1 & SCRIPTED_GENERIC1_TEAM_MASK ) >> SCRIPTED_GENERIC1_TEAM_SHIFT;

	if ( team == TEAM_RED ) {
		rgba[0] = 255; rgba[1] = 60; rgba[2] = 40;
	} else if ( team == TEAM_BLUE ) {
		rgba[0] = 50; rgba[1] = 110; rgba[2] = 255;
	} else {
		rgba[0] = rgba[1] = rgba[2] = 80;
	}
	rgba[3] = 255;
}

void CG_Autoball_Draw2D( void ) {
	if ( !cg.snap || cg.showScores )
		return;
	if ( cg.snap->ps.pm_type == PM_INTERMISSION )
		return;
	/* the ball arrow is for drivers: not for spectators (free, following or
	   on the TV camera) and not while a wrecked car waits for its respawn */
	if ( cg_autoballIndicator.integer && cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR &&
		!( cg.snap->ps.pm_flags & PMF_FOLLOW ) && cg.snap->ps.stats[STAT_HEALTH] > 0 )
		CG_Autoball_DrawIndicator();
	if ( cgs.gametype != GT_AUTOBALL )
		return;
	CG_SetScreenPlacement( PLACE_CENTER, PLACE_TOP );
	CG_Autoball_DrawScoreBug();
	CG_PopScreenPlacement();
	CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );
	CG_Autoball_DrawGoalBanner();
	CG_PopScreenPlacement();
}


/*
===========================================================================
Goal blast: a large explosion in the scoring team's colour plus a camera
shake that fades with distance. Triggered by EV_EXPLOSION carrying
EXPLOSION_PARM_AUTOBALL_GOAL.
===========================================================================
*/

#define AUTOBALL_SHAKE_DURATION   1400
#define AUTOBALL_SHAKE_NEAR       1200.0f
#define AUTOBALL_SHAKE_FAR        6000.0f
#define AUTOBALL_SHAKE_MIN        0.30f
#define AUTOBALL_SHAKE_ANGLE      4.0f    /* degrees at full strength */
#define AUTOBALL_SHAKE_OFFSET     8.0f    /* units at full strength */

static int   ab_shakeStart;
static float ab_shakeAmp;
static float ab_shakePhase[4];

static void CG_Autoball_StartShake( const vec3_t origin ) {
	vec3_t delta;
	float dist, amp;
	int i;

	if ( cg_autoballShake.value <= 0.0f )
		return;

	VectorSubtract( origin, cg.refdef.vieworg, delta );
	dist = VectorLength( delta );
	if ( dist <= AUTOBALL_SHAKE_NEAR ) {
		amp = 1.0f;
	} else if ( dist >= AUTOBALL_SHAKE_FAR ) {
		amp = AUTOBALL_SHAKE_MIN;
	} else {
		amp = 1.0f - ( 1.0f - AUTOBALL_SHAKE_MIN ) *
			( dist - AUTOBALL_SHAKE_NEAR ) / ( AUTOBALL_SHAKE_FAR - AUTOBALL_SHAKE_NEAR );
	}
	amp *= cg_autoballShake.value;

	/* a running shake is only replaced by a stronger one */
	if ( ab_shakeStart && cg.time - ab_shakeStart < AUTOBALL_SHAKE_DURATION ) {
		float t = (float)( cg.time - ab_shakeStart ) / AUTOBALL_SHAKE_DURATION;
		float current = ab_shakeAmp * ( 1.0f - t ) * ( 1.0f - t );
		if ( current > amp )
			return;
	}
	ab_shakeStart = cg.time;
	ab_shakeAmp = amp;
	for ( i = 0; i < 4; i++ )
		ab_shakePhase[i] = random() * 2.0f * M_PI;
}

void CG_Autoball_ApplyShake( void ) {
	float t, env, sec;
	vec3_t axis[3];

	if ( !ab_shakeStart )
		return;
	if ( cg_autoballShake.value <= 0.0f || cg.time < ab_shakeStart
		|| cg.time - ab_shakeStart >= AUTOBALL_SHAKE_DURATION ) {
		ab_shakeStart = 0;
		return;
	}

	t = (float)( cg.time - ab_shakeStart ) / AUTOBALL_SHAKE_DURATION;
	env = ab_shakeAmp * ( 1.0f - t ) * ( 1.0f - t );
	sec = cg.time * 0.001f;

	/* layered sines instead of per-frame noise: rough but not flickering */
	cg.refdefViewAngles[PITCH] += env * AUTOBALL_SHAKE_ANGLE *
		( 0.7f * sin( sec * 2.0f * M_PI * 11.0f + ab_shakePhase[0] ) + 0.3f * sin( sec * 2.0f * M_PI * 23.0f ) );
	cg.refdefViewAngles[YAW] += env * AUTOBALL_SHAKE_ANGLE * 0.6f *
		sin( sec * 2.0f * M_PI * 8.0f + ab_shakePhase[1] );
	cg.refdefViewAngles[ROLL] += env * AUTOBALL_SHAKE_ANGLE * 0.8f *
		sin( sec * 2.0f * M_PI * 6.0f + ab_shakePhase[2] );

	AnglesToAxis( cg.refdefViewAngles, axis );
	VectorMA( cg.refdef.vieworg, env * AUTOBALL_SHAKE_OFFSET *
		sin( sec * 2.0f * M_PI * 14.0f + ab_shakePhase[3] ), axis[2], cg.refdef.vieworg );
	VectorMA( cg.refdef.vieworg, env * AUTOBALL_SHAKE_OFFSET * 0.5f *
		sin( sec * 2.0f * M_PI * 9.0f + ab_shakePhase[0] ), axis[1], cg.refdef.vieworg );
}

/*
===========================================================================
Ball trail and goal lights
===========================================================================
*/

#define AB_TRAIL_MIN_KMH        100.0f   /* trail starts here */
#define AB_TRAIL_FIRE_KMH       150.0f   /* fire on top of the trail */
#define AB_TRAIL_SPACING        22.0f    /* units between two trail puffs */
#define AB_TRAIL_MAX_STEPS      10

#define AB_GOAL_LIGHT_TIME      3000
#define AB_GOAL_LIGHT_SIDE      380.0f   /* side lights, from the goal centre */

static vec3_t ab_trailLast[MAX_GENTITIES];
static int    ab_trailTime[MAX_GENTITIES];
static qhandle_t ab_trailShader;

static int   ab_goalLightStart;
static vec3_t ab_goalLightOrigin;
static vec3_t ab_goalLightSide;
static vec3_t ab_goalLightColor;

/* full-strength colour of the last touching team, white for nobody */
static void CG_Autoball_TrailColor( const entityState_t *s, vec3_t out ) {
	int team = ( s->generic1 & SCRIPTED_GENERIC1_TEAM_MASK ) >> SCRIPTED_GENERIC1_TEAM_SHIFT;

	if ( team == TEAM_RED ) {
		VectorSet( out, 1.0f, 0.25f, 0.15f );
	} else if ( team == TEAM_BLUE ) {
		VectorSet( out, 0.2f, 0.45f, 1.0f );
	} else {
		VectorSet( out, 0.85f, 0.85f, 0.85f );
	}
}

void CG_Autoball_BallTrail( centity_t *cent ) {
	int num = cent->currentState.number;
	float kmh, dist, f, radius, frac;
	vec3_t color, pos, delta, vel;
	int i, steps;

	kmh = VectorLength( cent->currentState.pos.trDelta ) / CP_M_2_QU * 3.6f;
	VectorSubtract( cent->lerpOrigin, ab_trailLast[num], delta );
	dist = VectorLength( delta );

	/* off, too slow, or the ball jumped (kick-off reset, first frame) */
	if ( !cg_autoballTrail.integer || kmh < AB_TRAIL_MIN_KMH || cg.time - ab_trailTime[num] > 250 ||
		dist > 600.0f ) {
		VectorCopy( cent->lerpOrigin, ab_trailLast[num] );
		ab_trailTime[num] = cg.time;
		return;
	}
	if ( dist < AB_TRAIL_SPACING )
		return;
	if ( !ab_trailShader )
		ab_trailShader = trap_R_RegisterShader( "autoballTrail" );

	CG_Autoball_TrailColor( &cent->currentState, color );
	f = ( kmh - AB_TRAIL_MIN_KMH ) / ( AB_TRAIL_FIRE_KMH - AB_TRAIL_MIN_KMH );
	if ( f > 1.0f )
		f = 1.0f;
	radius = cent->currentState.time2 > 0 ? (float)cent->currentState.time2 : AUTOBALL_BALL_RADIUS;

	steps = (int)( dist / AB_TRAIL_SPACING );
	if ( steps > AB_TRAIL_MAX_STEPS )
		steps = AB_TRAIL_MAX_STEPS;
	VectorClear( vel );
	for ( i = 1; i <= steps; i++ ) {
		frac = (float)i / steps;
		VectorMA( ab_trailLast[num], frac, delta, pos );

		/* glow in team colour, longer and brighter the faster the ball */
		CG_SmokePuff( pos, vel, radius * ( 0.55f + 0.25f * f ),
			color[0], color[1], color[2], 0.3f + 0.35f * f,
			220.0f + 260.0f * f, cg.time, 0, LEF_PUFF_DONT_SCALE, ab_trailShader );

		if ( kmh >= AB_TRAIL_FIRE_KMH ) {
			/* fire core and a little smoke */
			CG_SmokePuff( pos, vel, radius * 0.75f, 1.0f, 0.55f, 0.12f, 0.75f,
				320.0f, cg.time, 0, LEF_PUFF_DONT_SCALE, ab_trailShader );
			if ( ( i & 3 ) == 0 ) {
				vec3_t up;
				VectorSet( up, crandom() * 20.0f, crandom() * 20.0f, 40.0f );
				CG_SmokePuff( pos, up, radius * 0.9f, 0.25f, 0.25f, 0.25f, 0.4f,
					750.0f, cg.time, 0, 0, cgs.media.smokePuffShader );
			}
		}
	}
	if ( kmh >= AB_TRAIL_FIRE_KMH )
		trap_R_AddLightToScene( cent->lerpOrigin, 180.0f + 60.0f * random(), 1.0f, 0.6f, 0.2f );

	VectorCopy( cent->lerpOrigin, ab_trailLast[num] );
	ab_trailTime[num] = cg.time;
}

/* the ball nearest to point (the one that just scored) */
static centity_t *CG_Autoball_BallNear( const vec3_t point ) {
	centity_t *best = NULL;
	float d, bestDist = 1.0e18f;
	int i;

	if ( !cg.snap )
		return NULL;
	for ( i = 0; i < cg.snap->numEntities; i++ ) {
		entityState_t *es = &cg.snap->entities[i];
		if ( es->eType != ET_SCRIPTED || !( es->generic1 & SCRIPTED_GENERIC1_NO_PREDICT ) )
			continue;
		d = DistanceSquared( cg_entities[es->number].lerpOrigin, point );
		if ( d < bestDist ) {
			bestDist = d;
			best = &cg_entities[es->number];
		}
	}
	return best;
}

static void CG_Autoball_StartGoalLights( const vec3_t origin, int team, const vec4_t color ) {
	centity_t *ball = CG_Autoball_BallNear( origin );
	vec3_t into;

	ab_goalLightStart = cg.time;
	VectorCopy( origin, ab_goalLightOrigin );
	ab_goalLightOrigin[2] += 120.0f;
	VectorCopy( color, ab_goalLightColor );
	VectorClear( ab_goalLightSide );

	/* the server tells us where the goals are: light the goal that was hit */
	if ( cgs.autoballHaveGoals ) {
		const float *hit = cgs.autoballGoal[team == TEAM_RED ? 1 : 0];
		const float *other = cgs.autoballGoal[team == TEAM_RED ? 0 : 1];
		VectorCopy( hit, ab_goalLightOrigin );
		ab_goalLightOrigin[2] += 60.0f;
		VectorSubtract( hit, other, into );
		into[2] = 0.0f;
		if ( VectorNormalize( into ) > 0.1f ) {
			VectorSet( ab_goalLightSide, -into[1], into[0], 0.0f );
			return;
		}
	}

	/* otherwise: the ball flies into the goal, the goal line is across its path */
	if ( ball ) {
		VectorCopy( ball->currentState.pos.trDelta, into );
		into[2] = 0.0f;
		if ( VectorNormalize( into ) > 0.1f )
			VectorSet( ab_goalLightSide, -into[1], into[0], 0.0f );
	}
}

/* strobing team-coloured lights at the goal, called every frame */
void CG_Autoball_AddSceneEffects( void ) {
	int elapsed;
	float env, strobe, intensity;
	vec3_t pos;
	qboolean flip;

	if ( !ab_goalLightStart )
		return;
	elapsed = cg.time - ab_goalLightStart;
	if ( elapsed < 0 || elapsed > AB_GOAL_LIGHT_TIME ) {
		ab_goalLightStart = 0;
		return;
	}
	env = 1.0f - (float)elapsed / AB_GOAL_LIGHT_TIME;
	env = sqrt( env );
	flip = ( ( elapsed / 140 ) & 1 ) ? qtrue : qfalse;
	strobe = flip ? 1.0f : 0.35f;

	/* centre: team colour, pulsing */
	intensity = ( 380.0f + 220.0f * strobe ) * env;
	trap_R_AddLightToScene( ab_goalLightOrigin, intensity,
		ab_goalLightColor[0], ab_goalLightColor[1], ab_goalLightColor[2] );

	/* posts: alternate left/right, white flashes between the colour */
	if ( VectorLengthSquared( ab_goalLightSide ) > 0.0f ) {
		VectorMA( ab_goalLightOrigin, AB_GOAL_LIGHT_SIDE, ab_goalLightSide, pos );
		if ( flip )
			trap_R_AddLightToScene( pos, 320.0f * env, ab_goalLightColor[0], ab_goalLightColor[1], ab_goalLightColor[2] );
		else
			trap_R_AddLightToScene( pos, 260.0f * env, 1.0f, 1.0f, 1.0f );
		VectorMA( ab_goalLightOrigin, -AB_GOAL_LIGHT_SIDE, ab_goalLightSide, pos );
		if ( !flip )
			trap_R_AddLightToScene( pos, 320.0f * env, ab_goalLightColor[0], ab_goalLightColor[1], ab_goalLightColor[2] );
		else
			trap_R_AddLightToScene( pos, 260.0f * env, 1.0f, 1.0f, 1.0f );
	}
}

void CG_Autoball_GoalExplosion( vec3_t origin, int team ) {
	localEntity_t *ex;
	vec3_t up, pos, vel;
	vec4_t col;
	float ang;
	int i;

	VectorSet( up, 0, 0, 1 );
	CG_Autoball_TeamColor( team, 1.0f, col );
	CG_Autoball_StartGoalLights( origin, team, col );

	/* sound: the crowd comes from the server, add the bang here */
	trap_S_StartLocalSound( cgs.media.sfx_rockexp, CHAN_LOCAL_SOUND );

	/* fireball: a cluster of large sprite explosions */
	for ( i = 0; i < 5; i++ ) {
		VectorCopy( origin, pos );
		if ( i > 0 ) {
			pos[0] += crandom() * 90.0f;
			pos[1] += crandom() * 90.0f;
			pos[2] += random() * 110.0f;
		}
		ex = CG_MakeExplosion( pos, up, cgs.media.dishFlashModel,
			cgs.media.rocketExplosionShader, 900 + i * 150, qtrue );
		ex->radius = ( i == 0 ) ? 340.0f : 170.0f + random() * 90.0f;
		if ( i == 0 ) {
			ex->light = 900;
			ex->lightColor[0] = 0.6f + 0.4f * col[0];
			ex->lightColor[1] = 0.45f + 0.35f * col[1];
			ex->lightColor[2] = 0.2f + 0.6f * col[2];
		}
	}

	/* rising plume */
	if ( cg_oldRocket.integer == 0 ) {
		VectorSet( vel, 0, 0, 60 );
		CG_ParticleExplosion( "explode1", origin, vel, 1800, 90, 240 );
		for ( i = 0; i < 3; i++ ) {
			VectorCopy( origin, pos );
			pos[0] += crandom() * 70.0f;
			pos[1] += crandom() * 70.0f;
			VectorSet( vel, crandom() * 40.0f, crandom() * 40.0f, 90.0f );
			CG_ParticleExplosion( "explode1", pos, vel, 1400, 60, 170 );
		}
	}

	/* ground shock ring */
	for ( i = 0; i < 28; i++ ) {
		ang = i * ( 2.0f * M_PI / 28.0f );
		VectorSet( vel, cos( ang ) * 950.0f, sin( ang ) * 950.0f, 40.0f );
		VectorCopy( origin, pos );
		CG_SmokePuff( pos, vel, 70.0f, 0.85f, 0.85f, 0.85f, 0.6f,
			850.0f, cg.time, 0, 0, cgs.media.smokePuffShader );
	}

	/* lingering smoke column */
	for ( i = 0; i < 10; i++ ) {
		VectorCopy( origin, pos );
		pos[0] += crandom() * 60.0f;
		pos[1] += crandom() * 60.0f;
		VectorSet( vel, crandom() * 25.0f, crandom() * 25.0f, 60.0f + random() * 60.0f );
		CG_SmokePuff( pos, vel, 90.0f + random() * 60.0f, 0.35f, 0.35f, 0.35f, 0.55f,
			2600.0f + random() * 800.0f, cg.time, 0, 0, cgs.media.smokePuffShader );
	}

	/* confetti sparks in the scoring team's colour, plus white ones */
	CG_Particles( origin, 90, 700, 1600, 7, PT_GRAVITY,
		(byte)( col[0] * 255 ), (byte)( col[1] * 255 ), (byte)( col[2] * 255 ) );
	CG_Particles( origin, 40, 550, 1200, 5, PT_GRAVITY, 255, 230, 160 );

	CG_Autoball_StartShake( origin );
}


/*
===========================================================================
Cameras

Both run at the end of CG_CalcViewValues, after the normal view is known,
and replace cg.refdef.vieworg / cg.refdefViewAngles.

  Kick-off flight  First kick-off of a match: the server adds a few seconds
                   to the wait (g_autoballIntro) and sends when they end.
                   Until then the camera circles the field and finally
                   blends into the player's own view. cg_autoballIntro 0
                   skips it.
  TV director      Free spectators and wrecked cars waiting to respawn
                   (cg_autoballTVCam): a sideline camera follows the ball;
                   when the ball races towards a goal, and during the goal
                   celebration, a camera in that goal takes over.
===========================================================================
*/

#define AB_INTRO_BLEND          1300    /* ms: the flight blends into the normal view */
#define AB_INTRO_SWEEP          210.0f  /* degrees the flight circles the field */
#define AB_TV_GOALCAM_HOLD      1800    /* ms a goal camera is kept at least */
#define AB_TV_GOALCAM_SPEED     350.0f  /* u/s: slower balls are no "shot" */

typedef struct {
	qboolean valid;
	vec3_t  goal0, goal1;   /* goal centres the field was built from */
	vec3_t  centre;
	vec3_t  axis;           /* red goal -> blue goal */
	vec3_t  side;           /* across the field, towards the TV camera */
	float   halfLength;
	float   sideDist;       /* TV camera distance from the axis */
	float   height;         /* TV camera height above the centre */
} abField_t;

static abField_t ab_field;

static int    ab_introStart, ab_introFor;
static int    ab_tvShot;            /* -1 sideline, 0 / 1 goal camera */
static int    ab_tvShotSince;
static int    ab_tvLastTime;
static float  ab_tvAxial;
static vec3_t ab_tvLook;

/* frame-rate independent smoothing factor. Not exp(): the QVM's bg_lib
   exp() only covers 0..1 and returns 1 for negative input. */
static float CG_Autoball_Smooth( float rate, float dt ) {
	float x = rate * dt;

	if ( x <= 0.0f )
		return 0.0f;
	return x / ( 1.0f + x );
}

/* move a camera from 'from' towards 'to', stopping in front of walls */
static void CG_Autoball_ClampCam( const vec3_t from, const vec3_t to, vec3_t out ) {
	trace_t tr;
	vec3_t mins, maxs, dir;

	VectorSet( mins, -8, -8, -8 );
	VectorSet( maxs, 8, 8, 8 );
	trap_CM_BoxTrace( &tr, from, to, mins, maxs, 0, MASK_SOLID );	/* world only */
	if ( tr.fraction >= 1.0f ) {
		VectorCopy( to, out );
		return;
	}
	VectorSubtract( to, from, dir );
	VectorNormalize( dir );
	VectorMA( tr.endpos, -24.0f, dir, out );
}

/* field geometry from the goal centres the server sends */
static void CG_Autoball_SetupField( void ) {
	trace_t tr;
	vec3_t start, end, mins, maxs, cam;
	float plus, minus;

	if ( !cgs.autoballHaveGoals ) {
		ab_field.valid = qfalse;
		return;
	}
	if ( ab_field.valid && VectorCompare( ab_field.goal0, cgs.autoballGoal[0] ) &&
		VectorCompare( ab_field.goal1, cgs.autoballGoal[1] ) )
		return;

	VectorCopy( cgs.autoballGoal[0], ab_field.goal0 );
	VectorCopy( cgs.autoballGoal[1], ab_field.goal1 );
	VectorAdd( ab_field.goal0, ab_field.goal1, ab_field.centre );
	VectorScale( ab_field.centre, 0.5f, ab_field.centre );
	VectorSubtract( ab_field.goal1, ab_field.goal0, ab_field.axis );
	ab_field.axis[2] = 0.0f;
	ab_field.halfLength = VectorNormalize( ab_field.axis ) * 0.5f;
	VectorSet( ab_field.side, -ab_field.axis[1], ab_field.axis[0], 0.0f );

	/* sideline: the wider side, a bit inside the wall */
	VectorSet( mins, -16, -16, -16 );
	VectorSet( maxs, 16, 16, 16 );
	VectorCopy( ab_field.centre, start );
	start[2] += 150.0f;
	VectorMA( start, 8000.0f, ab_field.side, end );
	trap_CM_BoxTrace( &tr, start, end, mins, maxs, 0, MASK_SOLID );
	plus = tr.fraction * 8000.0f;
	VectorMA( start, -8000.0f, ab_field.side, end );
	trap_CM_BoxTrace( &tr, start, end, mins, maxs, 0, MASK_SOLID );
	minus = tr.fraction * 8000.0f;
	if ( minus > plus ) {
		VectorScale( ab_field.side, -1.0f, ab_field.side );
		plus = minus;
	}
	/* measured distance wins over the minimum: never behind the wall */
	ab_field.sideDist = plus - 260.0f;
	if ( ab_field.sideDist < plus * 0.6f )
		ab_field.sideDist = plus * 0.6f;
	if ( ab_field.sideDist > 5000.0f )
		ab_field.sideDist = 5000.0f;

	/* height: under the ceiling, if there is one */
	VectorMA( start, ab_field.sideDist, ab_field.side, cam );
	VectorCopy( cam, end );
	end[2] += 2000.0f;
	trap_CM_BoxTrace( &tr, cam, end, mins, maxs, 0, MASK_SOLID );
	ab_field.height = tr.fraction * 2000.0f - 120.0f;
	if ( ab_field.height > 900.0f )
		ab_field.height = 900.0f;
	if ( ab_field.height < 0.0f )
		ab_field.height = 0.0f;
	ab_field.height += 150.0f;	/* the measurement started 150 above the centre */
	ab_field.valid = qtrue;
}

static void CG_Autoball_LookAt( const vec3_t origin, const vec3_t target ) {
	vec3_t dir;

	VectorCopy( origin, cg.refdef.vieworg );
	VectorSubtract( target, origin, dir );
	vectoangles( dir, cg.refdefViewAngles );
}

/* true while the kick-off flight runs (the HUD stays hidden) */
qboolean CG_Autoball_IntroActive( void ) {
	return ( cgs.gametype == GT_AUTOBALL && cg_autoballIntro.integer &&
		cgs.autoballState == AUTOBALL_STATE_KICKOFF && cgs.autoballIntroEnd &&
		cg.time < cgs.autoballIntroEnd - AB_INTRO_BLEND / 2 ) ? qtrue : qfalse;
}

/* first kick-off: circle the field, end in the player's own view */
static qboolean CG_Autoball_IntroView( void ) {
	centity_t *ball;
	vec3_t centre, normalOrg, normalAng, toView, pos, start, look;
	float t, ease, blend, endYaw, yaw, radius, endRadius, height, endHeight;
	int duration;

	if ( !cg_autoballIntro.integer || cgs.autoballState != AUTOBALL_STATE_KICKOFF ||
		!cgs.autoballIntroEnd || cg.time >= cgs.autoballIntroEnd ) {
		ab_introFor = 0;
		return qfalse;
	}
	if ( ab_introFor != cgs.autoballIntroEnd ) {
		ab_introFor = cgs.autoballIntroEnd;
		ab_introStart = cg.time;
	}
	duration = cgs.autoballIntroEnd - ab_introStart;
	if ( duration < 500 )
		return qfalse;	/* joined at the very end */

	ball = CG_Autoball_FindBall();
	if ( ab_field.valid )
		VectorCopy( ab_field.centre, centre );
	else if ( ball )
		VectorCopy( ball->lerpOrigin, centre );
	else
		return qfalse;
	if ( ball )
		centre[2] = ball->lerpOrigin[2] + 60.0f;

	VectorCopy( cg.refdef.vieworg, normalOrg );
	VectorCopy( cg.refdefViewAngles, normalAng );

	t = (float)( cg.time - ab_introStart ) / duration;
	if ( t > 1.0f )
		t = 1.0f;
	ease = t * t * ( 3.0f - 2.0f * t );

	/* the orbit ends where the player's own camera is */
	VectorSubtract( normalOrg, centre, toView );
	endHeight = toView[2];
	toView[2] = 0.0f;
	endRadius = VectorLength( toView );
	endYaw = ( endRadius > 1.0f ) ? atan2( toView[1], toView[0] ) * 180.0f / M_PI : 0.0f;
	yaw = endYaw - AB_INTRO_SWEEP * ( 1.0f - ease );
	radius = ( ab_field.valid ? ab_field.halfLength * 1.1f : 2200.0f ) * ( 1.0f - ease ) + endRadius * ease;
	height = 900.0f * ( 1.0f - ease ) + endHeight * ease;

	VectorSet( pos, centre[0] + cos( DEG2RAD( yaw ) ) * radius,
		centre[1] + sin( DEG2RAD( yaw ) ) * radius, centre[2] + height );
	VectorCopy( centre, start );
	start[2] += 100.0f;
	CG_Autoball_ClampCam( start, pos, pos );
	VectorCopy( centre, look );
	CG_Autoball_LookAt( pos, look );

	/* last part: hand over to the normal view */
	blend = (float)( cg.time - ( cgs.autoballIntroEnd - AB_INTRO_BLEND ) ) / AB_INTRO_BLEND;
	if ( blend > 0.0f ) {
		int i;
		if ( blend > 1.0f )
			blend = 1.0f;
		blend = blend * blend * ( 3.0f - 2.0f * blend );
		for ( i = 0; i < 3; i++ ) {
			cg.refdef.vieworg[i] += ( normalOrg[i] - cg.refdef.vieworg[i] ) * blend;
			cg.refdefViewAngles[i] = LerpAngle( cg.refdefViewAngles[i], normalAng[i], blend );
		}
	}
	return qtrue;
}

static qboolean CG_Autoball_WantsTVCam( void ) {
	const playerState_t *ps = &cg.snap->ps;

	if ( !cg_autoballTVCam.integer || !ab_field.valid || ps->pm_type == PM_INTERMISSION )
		return qfalse;
	if ( ps->persistant[PERS_TEAM] == TEAM_SPECTATOR && !( ps->pm_flags & PMF_FOLLOW ) )
		return qtrue;		/* free spectator */
	if ( ps->stats[STAT_HEALTH] <= 0 )
		return qtrue;		/* wrecked, waiting to respawn */
	return qfalse;
}

/* goal camera wanted? returns the goal index or -1 */
static int CG_Autoball_TVGoalShot( const vec3_t ballPos, const vec3_t ballVel ) {
	vec3_t toGoal, vel;
	float dist, speed;
	int i;

	if ( cgs.autoballState == AUTOBALL_STATE_GOAL )
		return cgs.autoballGoalTeam == TEAM_RED ? 1 : 0;	/* the goal that was hit */
	VectorCopy( ballVel, vel );
	vel[2] = 0.0f;
	speed = VectorNormalize( vel );
	if ( speed < AB_TV_GOALCAM_SPEED )
		return -1;
	for ( i = 0; i < 2; i++ ) {
		VectorSubtract( i ? ab_field.goal1 : ab_field.goal0, ballPos, toGoal );
		toGoal[2] = 0.0f;
		dist = VectorNormalize( toGoal );
		if ( dist < ab_field.halfLength * 0.7f && DotProduct( vel, toGoal ) > 0.55f )
			return i;
	}
	return -1;
}

static qboolean CG_Autoball_TVView( void ) {
	centity_t *ball;
	vec3_t ballPos, pos, out, start;
	float dt, axial, target;
	int shot;
	qboolean cut = qfalse;

	if ( !CG_Autoball_WantsTVCam() ) {
		ab_tvLastTime = 0;
		return qfalse;
	}
	ball = CG_Autoball_FindBall();
	if ( !ball )
		return qfalse;
	VectorCopy( ball->lerpOrigin, ballPos );

	dt = ( cg.time - ab_tvLastTime ) * 0.001f;
	if ( !ab_tvLastTime || dt < 0.0f || dt > 0.5f ) {
		/* (re)start: no smoothing from stale values */
		ab_tvShot = -1;
		ab_tvShotSince = cg.time;
		ab_tvAxial = DotProduct( ballPos, ab_field.axis ) - DotProduct( ab_field.centre, ab_field.axis );
		cut = qtrue;
		dt = 0.0f;
	}
	ab_tvLastTime = cg.time;

	/* director: switch shots, keep a goal camera for a moment */
	shot = CG_Autoball_TVGoalShot( ballPos, ball->currentState.pos.trDelta );
	if ( shot != ab_tvShot ) {
		if ( ab_tvShot < 0 || shot >= 0 || cg.time - ab_tvShotSince > AB_TV_GOALCAM_HOLD ) {
			ab_tvShot = shot;
			ab_tvShotSince = cg.time;
			cut = qtrue;
		}
	}

	if ( ab_tvShot >= 0 ) {
		/* inside the goal, above the bar height, looking out at the play */
		const float *goal = ab_tvShot ? ab_field.goal1 : ab_field.goal0;
		VectorSubtract( goal, ab_field.centre, out );
		out[2] = 0.0f;
		VectorNormalize( out );
		VectorCopy( goal, start );
		start[2] += 40.0f;
		VectorMA( goal, 80.0f, out, pos );
		pos[2] += 170.0f;
		CG_Autoball_ClampCam( start, pos, pos );
	} else {
		/* sideline: slides along with the ball, a little behind it */
		target = DotProduct( ballPos, ab_field.axis ) - DotProduct( ab_field.centre, ab_field.axis );
		target = Com_Clamp( -ab_field.halfLength * 0.75f, ab_field.halfLength * 0.75f, target );
		ab_tvAxial += ( target - ab_tvAxial ) * CG_Autoball_Smooth( 2.5f, dt );
		axial = ab_tvAxial * 0.9f;
		VectorMA( ab_field.centre, axial, ab_field.axis, pos );
		VectorCopy( pos, start );
		start[2] += 150.0f;
		VectorMA( pos, ab_field.sideDist, ab_field.side, pos );
		pos[2] += ab_field.height;
		/* the wall may come closer away from the halfway line */
		CG_Autoball_ClampCam( start, pos, pos );
	}

	if ( cut )
		VectorCopy( ballPos, ab_tvLook );
	else {
		int i;
		float k = CG_Autoball_Smooth( 7.0f, dt );
		for ( i = 0; i < 3; i++ )
			ab_tvLook[i] += ( ballPos[i] - ab_tvLook[i] ) * k;
	}
	CG_Autoball_LookAt( pos, ab_tvLook );
	return qtrue;
}

void CG_Autoball_OverrideView( void ) {
	if ( cgs.gametype != GT_AUTOBALL || !cg.snap )
		return;
	CG_Autoball_SetupField();
	if ( CG_Autoball_IntroView() )
		return;
	CG_Autoball_TVView();
}


/*
===========================================================================
Awards under the end-of-match scoreboard
===========================================================================
*/

#define AB_AWARD_W      150.0f
#define AB_AWARD_H      50.0f
#define AB_AWARD_GAP    8.0f

static void CG_Autoball_DrawAward( float x, float y, const char *title, int client,
	const char *value, float fade ) {
	vec4_t bg, bar, white, accent;
	char name[MAX_NAME_LENGTH];
	int team;

	team = cgs.clientinfo[client].team;
	AB_SET4( bg, 0.02f, 0.03f, 0.04f, 0.72f * fade );
	CG_Autoball_TeamColor( team == TEAM_BLUE ? TEAM_BLUE : TEAM_RED, 0.9f * fade, bar );
	AB_SET4( white, 1.0f, 1.0f, 1.0f, fade );
	AB_SET4( accent, 1.0f, 0.78f, 0.2f, fade );

	CG_FillRect( x, y, AB_AWARD_W, AB_AWARD_H, bg );
	CG_FillRect( x, y, AB_AWARD_W, 3.0f, bar );
	Q_strncpyz( name, cgs.clientinfo[client].name, sizeof( name ) );
	Q_CleanStr( name );
	if ( strlen( name ) > 14 )
		name[14] = '\0';
	CG_DrawIngameString( (int)( x + AB_AWARD_W / 2 ), (int)( y + 6 ), title,
		UI_CENTER | UI_SMALLFONT, 0.6f, accent );
	CG_DrawIngameString( (int)( x + AB_AWARD_W / 2 ), (int)( y + 18 ), name,
		UI_CENTER | UI_SMALLFONT, 0.75f, white );
	CG_DrawIngameString( (int)( x + AB_AWARD_W / 2 ), (int)( y + 34 ), value,
		UI_CENTER | UI_SMALLFONT, 0.6f, white );
}

/* MVP, top scorer, best keeper, hardest shot; only the ones somebody earned */
void CG_Autoball_DrawAwards( int y, float fade ) {
	int i, count;
	int mvp = -1, scorer = -1, keeper = -1, shooter = -1;
	const char *titles[4];
	char values[4][32];	/* not va(): it only has two buffers */
	int clients[4];
	float x, top;

	if ( cgs.gametype != GT_AUTOBALL || !cg.snap || cg.snap->ps.pm_type != PM_INTERMISSION )
		return;

	for ( i = 0; i < cg.numScores; i++ ) {
		score_t *s = &cg.scores[i];
		if ( s->client < 0 || s->client >= MAX_CLIENTS )
			continue;
		if ( cgs.clientinfo[s->client].team != TEAM_RED && cgs.clientinfo[s->client].team != TEAM_BLUE )
			continue;
		if ( mvp < 0 || s->score > cg.scores[mvp].score )
			mvp = i;
		if ( s->autoballGoals > 0 && ( scorer < 0 || s->autoballGoals > cg.scores[scorer].autoballGoals ||
			( s->autoballGoals == cg.scores[scorer].autoballGoals &&
			s->autoballAssists > cg.scores[scorer].autoballAssists ) ) )
			scorer = i;
		if ( s->autoballSaves > 0 && ( keeper < 0 || s->autoballSaves > cg.scores[keeper].autoballSaves ) )
			keeper = i;
		if ( s->autoballBestShot > 0 && ( shooter < 0 || s->autoballBestShot > cg.scores[shooter].autoballBestShot ) )
			shooter = i;
	}

	count = 0;
	if ( mvp >= 0 ) {
		titles[count] = "MVP";
		clients[count] = cg.scores[mvp].client;
		Com_sprintf( values[count], sizeof( values[count] ), "%i points", cg.scores[mvp].score );
		count++;
	}
	if ( scorer >= 0 ) {
		titles[count] = "TOP SCORER";
		clients[count] = cg.scores[scorer].client;
		Com_sprintf( values[count], sizeof( values[count] ), "%i goal%s", cg.scores[scorer].autoballGoals,
			cg.scores[scorer].autoballGoals == 1 ? "" : "s" );
		count++;
	}
	if ( keeper >= 0 ) {
		titles[count] = "BEST KEEPER";
		clients[count] = cg.scores[keeper].client;
		Com_sprintf( values[count], sizeof( values[count] ), "%i save%s", cg.scores[keeper].autoballSaves,
			cg.scores[keeper].autoballSaves == 1 ? "" : "s" );
		count++;
	}
	if ( shooter >= 0 ) {
		titles[count] = "HARDEST SHOT";
		clients[count] = cg.scores[shooter].client;
		Q_strncpyz( values[count], CG_Autoball_SpeedText( cg.scores[shooter].autoballBestShot ),
			sizeof( values[count] ) );
		count++;
	}
	if ( !count )
		return;

	/* below the scoreboard, but never off the screen */
	top = (float)y + 10.0f;
	if ( top > 480.0f - AB_AWARD_H - 8.0f )
		top = 480.0f - AB_AWARD_H - 8.0f;
	x = 320.0f - ( count * AB_AWARD_W + ( count - 1 ) * AB_AWARD_GAP ) / 2.0f;
	for ( i = 0; i < count; i++ ) {
		CG_Autoball_DrawAward( x, top, titles[i], clients[i], values[i], fade );
		x += AB_AWARD_W + AB_AWARD_GAP;
	}
}
