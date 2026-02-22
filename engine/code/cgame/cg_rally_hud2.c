/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

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

/*
=================
CG_DrawHUD_Times
=================
*/
void CG_DrawHUD_Times(float x, float y){
	centity_t	*cent;
	int			lapTime, lastTime, totalTime, teamTime;
	//int		bestTime;
	char		*time;
	int			i, count = 0;

	cent = &cg_entities[cg.snap->ps.clientNum];

	if (cent->finishRaceTime){
		lapTime = cent->finishRaceTime - cent->startLapTime;
		totalTime = cent->finishRaceTime - cent->startRaceTime;
	}
	else if (cent->startRaceTime){
		lapTime = cg.time - cent->startLapTime;
		totalTime = cg.time - cent->startRaceTime;
	}
	else {
		lapTime = 0;
		totalTime = 0;
	}

	if (cent->lastStartLapTime)
		lastTime = cent->startLapTime - cent->lastStartLapTime;
	else
		lastTime = 0;

	//bestTime = cent->bestLapTime;

	// draw heading
	CG_FillRect(x, y, 170, 18, bgColor);
	CG_DrawSmallStringColor(x + 12, y, "TIMES", colorWhite);

	y += 20;

        if ( cgs.laplimit > 1 ){
                // draw lap time
                CG_DrawSmallStringColor(x + 12, y, "LAP:", colorWhite);
                time = getStringForTime( lapTime );
                CG_DrawSmallStringColor(x + 102, y, time, colorWhite);

                y += 20;

                // draw last lap time
                CG_DrawSmallStringColor(x + 12, y, "LAST:", colorWhite);
                if ( lastTime ){
                        time = getStringForTime( lastTime );
                        CG_DrawSmallStringColor(x + 102, y, time, colorWhite);
                }
                else {
                        CG_DrawSmallStringColor(x + 102, y, "N/A", colorWhite);
                }

                y += 20;
        }

        // draw total time
        CG_DrawSmallStringColor(x + 12, y, "TOTAL:", colorWhite);
        time = getStringForTime( totalTime );
        CG_DrawSmallStringColor(x + 102, y, time, colorWhite);

        y += 20;

        // draw ghost playback state
        CG_DrawSmallStringColor(x + 12, y, "GHOST:", colorWhite);
        if ( cg_ghostPlayback.integer == 1 && cg.ghostPlayback.valid ) {
                const char *best = cg.personalGhostBestTime > 0 ? getStringForTime( cg.personalGhostBestTime ) : "ready";
                CG_DrawSmallStringColor(x + 102, y, va( "On (%s)", best ), colorWhite);
        } else if ( cg_ghostPlayback.integer == 2 && cg.baseGhostAvailable ) {
                const char *best = cg.baseGhostBestTime > 0 ? getStringForTime( cg.baseGhostBestTime ) : "ready";
                CG_DrawSmallStringColor(x + 102, y, va( "Base (%s)", best ), colorWhite);
        } else {
                CG_DrawSmallStringColor(x + 102, y, "Off", colorWhite);
        }

        y += 20;

        // draw team time
        if (cgs.gametype == GT_TEAM_RACING || cgs.gametype == GT_TEAM_RACING_DM){
		// get new scores for accurate team dm times
		if (cg.scoresRequestTime + 2000 < cg.time){
			cg.scoresRequestTime = cg.time;
			trap_SendClientCommand( "score" );
		}

		// add up times for team members
		teamTime = 0;
		for ( i = 0 ; i < cgs.maxclients ; i++ ) {
			if ( !cgs.clientinfo[i].infoValid )	continue;

			if ( cgs.clientinfo[i].team == cgs.clientinfo[cg.snap->ps.clientNum].team ) {
				count++;

				cent = &cg_entities[i];

				if (cent->finishRaceTime)
					teamTime += cent->finishRaceTime - cent->startLapTime;
				else if (cent->startRaceTime)
					teamTime += cg.time - cent->startRaceTime;
			}
		}

		// subtract team frag times
		if (cgs.gametype == GT_TEAM_RACING_DM){
			switch(cgs.clientinfo[cg.snap->ps.clientNum].team){
			case TEAM_RED:
				if (cgs.scores1 > 0)
					teamTime -= cgs.scores1 * TIME_BONUS_PER_FRAG;
				break;
			case TEAM_BLUE:
				if (cgs.scores2 > 0)
					teamTime -= cgs.scores2 * TIME_BONUS_PER_FRAG;
				break;
			case TEAM_GREEN:
				if (cgs.scores3 > 0)
					teamTime -= cgs.scores3 * TIME_BONUS_PER_FRAG;
				break;
			case TEAM_YELLOW:
				if (cgs.scores4 > 0)
					teamTime -= cgs.scores4 * TIME_BONUS_PER_FRAG;
				break;
			default:
				break;
			}
		}

		// average team time
		teamTime /= count;

		CG_DrawSmallStringColor(x + 12, y, "TEAM:", colorWhite);
		time = getStringForTime(teamTime);
		CG_DrawSmallStringColor(x + 102, y, time, colorWhite);
	}
}


/*
====================
CG_DrawHUD_Positions
====================
*/
void CG_DrawHUD_Positions(float x, float y){
	int		i, num_teams, team_rank;

	num_teams = 0;
	for ( i = 0 ; i < 4 ; i++ ) {
		if (!TeamCount(-1, TEAM_RED + i)) continue;

		num_teams++;
	}

	team_rank = 0;
	for ( i = 0 ; i < 4 ; i++ ) {
		if (GetTeamAtRank(i + 1) == cgs.clientinfo[cg.snap->ps.clientNum].team)
			team_rank = i+1;
	}

	// draw heading
	CG_FillRect(x, y, 170, 18, bgColor);
	CG_DrawSmallStringColor(x + 12, y, "POSITION", colorWhite);

	y += 20;

	// draw your position
	CG_DrawSmallStringColor(x + 12, y, "YOU:", colorWhite);
	CG_DrawSmallStringColor(x + 102, y, va("%i/%i", cg_entities[cg.snap->ps.clientNum].currentPosition, cgs.numRacers), colorWhite);

	y += 20;

	// draw team position
	if (cgs.gametype == GT_TEAM_RACING || cgs.gametype == GT_TEAM_RACING_DM){
		CG_DrawSmallStringColor(x + 12, y, "TEAM:", colorWhite);
		CG_DrawSmallStringColor(x + 102, y, va("%i/%i", team_rank, num_teams), colorWhite);
	}
}

/*
===============
CG_DrawHUD_Laps
===============
*/
void CG_DrawHUD_Laps(float x, float y){

        // draw heading
        CG_FillRect(x, y, 170, 18, bgColor);
        if ( cgs.gametype == GT_SPRINT ) {
                CG_DrawSmallStringColor(x + 12, y, "SPRINT:", colorWhite);
                CG_DrawSmallStringColor(x + 102, y, "POINT-TO-POINT", colorWhite);
                return;
        }

        CG_DrawSmallStringColor(x + 12, y, "LAP:", colorWhite);
        if ( cgs.laplimit > 1 )
                CG_DrawSmallStringColor(x + 102, y, va("%i/%i", cg_entities[cg.snap->ps.clientNum].currentLap, cgs.laplimit), colorWhite);
        else
                CG_DrawSmallStringColor(x + 102, y, va("%i", cg_entities[cg.snap->ps.clientNum].currentLap), colorWhite);
}

/*
=======================
CG_DrawHUD_OpponentList
=======================
*/
void CG_DrawHUD_OpponentList(float x, float y){
	centity_t	*cent, *other;
	char		player[64];
	int			i, j, num;
	float		width, height;
	int			startPos, endPos;
	char		s[64];
	vec4_t	color;
	float		lastPlaceColor[4] = { 0.8f, 0.2f, 0.2f, 0.4f };
	int			playersRemaining;
	int			lastClient;
	int			lastPosition;

	cent = &cg_entities[cg.snap->ps.clientNum];

	playersRemaining = CG_GetPlayersRemaining( &lastClient );
	lastPosition = 0;
	if ( lastClient >= 0 ) {
		lastPosition = cg_entities[lastClient].currentPosition;
		if ( lastPosition <= 0 ) {
			lastPosition = cgs.clientinfo[lastClient].position;
		}
	}

	startPos = cent->currentPosition - 4 < 1 ? 1 : cent->currentPosition - 4;
	endPos = startPos + 8 > cgs.numRacers ? cgs.numRacers : startPos + 8;
	startPos = endPos - 8 < 1 ? 1 : endPos - 8;

	width = 198;
	height = 18;

	// draw your position
	CG_FillRect(x, y, width, height, bgColor);
	CG_DrawSmallStringColor(x, y, "POS:", colorWhite);
	CG_DrawSmallStringColor(x + 82, y, va("%i/%i", cent->currentPosition, cgs.numRacers), colorWhite);

	y += 20;

	if ( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_LCS ) {
		CG_FillRect(x, y, width, height, bgColor);
		if ( playersRemaining > 0 ) {
			CG_DrawSmallStringColor(x, y, "PLAYERS REMAINING:", colorWhite);
			CG_DrawSmallStringColor(x + 150, y, va("%i", playersRemaining), colorWhite);
		} else {
			CG_DrawSmallStringColor(x, y, "PLAYERS REMAINING:", colorMdGrey);
			CG_DrawSmallStringColor(x + 150, y, "--", colorMdGrey);
		}

		y += 20;
	}

	for (i = startPos; i <= endPos; i++){
		num = -1;
		for (j = 0; j < cgs.maxclients; j++){
			other = &cg_entities[j];
			if ( !other ) continue;
//			if ( isRaceObserver(other->currentState.clientNum) ) continue;
			if ( cgs.clientinfo[other->currentState.clientNum].team == TEAM_SPECTATOR ) continue;

			if ( cgs.clientinfo[other->currentState.clientNum].position == i ) {
				num = other->currentState.clientNum;
				break;
			}
		}

		if (num < 0 || num >= cgs.maxclients)
		{
#if 0
			if( (cg.time / 1000)%2 == 0 && (cg.time / 100)%2 == 0 ) {
				Com_Printf( "DEBUG: Could not find player for position %i. Num %i.\n", i, num );
			}
#endif
			continue;
		}

		if (cgs.clientinfo[num].team == TEAM_RED){
			Vector4Copy(colorRed, color);
			color[3] = 0.5f;
		}
		else if (cgs.clientinfo[num].team == TEAM_BLUE){
			Vector4Copy(colorBlue, color);
			color[3] = 0.5f;
		}
		else if (cgs.clientinfo[num].team == TEAM_GREEN){
			Vector4Copy(colorGreen, color);
			color[3] = 0.5f;
		}
		else if (cgs.clientinfo[num].team == TEAM_YELLOW){
			Vector4Copy(colorYellow, color);
			color[3] = 0.5f;
		}
		else {
			Vector4Copy(bgColor, color);
		}

		CG_FillRect(x, y, width, height, color);

		if ( (cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_LCS)
				&& playersRemaining > 1 && lastPosition > 0 ) {
			int rowPosition = cgs.clientinfo[num].position;
			if ( rowPosition == 0 ) {
				rowPosition = cg_entities[num].currentPosition;
			}
			if ( rowPosition == lastPosition ) {
				CG_FillRect( x, y, width, height, lastPlaceColor );
			}
		}

		Q_strncpyz(player, cgs.clientinfo[num].name, 16 );
		Com_sprintf(s, sizeof(s), " %i: %s", cgs.clientinfo[num].position, player);
		CG_DrawSmallStringColor( x, y, s, colorWhite);

		y += 20;
	}
}


/*
=================
CG_DrawHUD_Scores
=================
*/
void CG_DrawHUD_Scores(float x, float y){
	int		score, w;
	char	*s;

	// frags
	// FIXME: cgs.clientinfo[cg.snap->ps.clientNum].score doesnt seem to update?
	//        always zero even when player has frags

	s = va("%i", cgs.clientinfo[cg.snap->ps.clientNum].score);
	w = CG_DrawStrlen( s ) * GIANTCHAR_WIDTH;
	CG_FillRect(x, y, 96, 72, bgColor);
	CG_DrawSmallStringColor( x + 20, y + 6, "FRAGS:", colorWhite);
        CG_DrawStringExt( x + 48 - w/2, y + 26, s, colorWhite, qtrue, qfalse, GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );

	y += 72;

	// team frags
	// UPDATE: fix this so it displays the proper score
	if (cgs.gametype >= GT_TEAM){
		switch( cg.snap->ps.persistant[PERS_TEAM] ){
		default:
		case TEAM_RED:
			score = cgs.scores1;
			break;
		case TEAM_BLUE:
			score = cgs.scores2;
			break;
		case TEAM_GREEN:
			score = cgs.scores3;
			break;
		case TEAM_YELLOW:
			score = cgs.scores4;
			break;
		}

		s = va( "%i", score );
		w = CG_DrawStrlen( s ) * GIANTCHAR_WIDTH;
		CG_FillRect(x, y, 96, 78, bgColor);
		CG_DrawSmallStringColor( x + 24, y + 6, "TEAM:", colorWhite);
                CG_DrawStringExt( x + 48 - w/2, y + 26, s, colorWhite, qtrue, qfalse, GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );

		y += 78;
	}

	// draw scores from cg_draw
	CG_DrawScores(x + 96, y);
}



/*
========================
CG_DrawHUD_DerbyHitImpact
========================
*/
static void CG_DrawHUD_DerbyHitImpact( void ) {
	vec4_t color;
	float alpha;
	float frac;
	int elapsed;
	int duration;
	float overlayScale;
	float damageBoost;

	if ( cgs.gametype != GT_DERBY || !cg_derbyHitFxEnable.integer ) {
		return;
	}

	if ( !cg.derbyHitFxTime ) {
		return;
	}

	elapsed = cg.time - cg.derbyHitFxTime;
	duration = cg_derbyHitOverlayTime.integer;
	if ( duration < 120 ) {
		duration = 120;
	}
	if ( elapsed < 0 || elapsed >= duration ) {
		return;
	}

	frac = 1.0f - (float)elapsed / (float)duration;
	overlayScale = cg_derbyHitOverlayScale.value;
	if ( overlayScale < 0.0f ) {
		overlayScale = 0.0f;
	}
	damageBoost = (float)cg.derbyHitFxDamage * (1.0f / 40.0f);
	if ( damageBoost > 0.35f ) {
		damageBoost = 0.35f;
	}
	switch ( cg.derbyHitFxLevel ) {
	default:
	case 0:
		color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f;
		alpha = 0.14f;
		break;
	case 1:
		color[0] = 1.0f; color[1] = 0.72f; color[2] = 0.15f;
		alpha = 0.24f;
		break;
	case 2:
		color[0] = 1.0f; color[1] = 0.2f; color[2] = 0.1f;
		alpha = 0.34f;
		break;
	}
	alpha = ( alpha + damageBoost ) * overlayScale;
	if ( alpha > 0.80f ) {
		alpha = 0.80f;
	}
	color[3] = alpha * frac;
	CG_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color );
}


/*
============================
CG_DrawHUD_DerbyVehicleState
============================
*/
static void CG_DrawHUD_DerbyVehicleState( void ) {
	const int health = cg.snap->ps.stats[STAT_HEALTH];
	const float maxHealth = 100.0f;
	const int lastHitFlashMs = 420;
	float healthFrac;
	float x;
	float y;
	float scale;
	float panelW;
	float panelH;
	float barX;
	float barY;
	float barW;
	float barH;
	float vehicleX;
	float vehicleY;
	float vehicleW;
	float vehicleH;
	float zoneFrontRearW;
	float zoneFrontRearH;
	float zoneSideW;
	float zoneSideH;
	float zoneFrontX;
	float zoneFrontY;
	float zoneRearX;
	float zoneRearY;
	float zoneSideY;
	float zoneLeftX;
	float zoneRightX;
	float zoneRoofX;
	float zoneRoofY;
	vec4_t baseColor;
	vec4_t statusColor;
	vec4_t neutralColor;
	vec4_t barBackColor;
	vec4_t flashColor;
	vec4_t pulseColor;
	int hitSegment;
	qboolean hasDirectionalHit;
	qboolean critical;

	if ( cgs.gametype != GT_DERBY ) {
		return;
	}

	/* bottom-left, matching CG_DrawLowerLeft */
	CG_SetScreenPlacement( PLACE_LEFT, PLACE_BOTTOM );

	healthFrac = health / maxHealth;
	if ( healthFrac < 0.0f ) {
		healthFrac = 0.0f;
	} else if ( healthFrac > 1.0f ) {
		healthFrac = 1.0f;
	}

	scale = cg_derbyVehicleHudScale.value;
	if ( scale < 0.5f ) {
		scale = 0.5f;
	} else if ( scale > 2.0f ) {
		scale = 2.0f;
	}

	panelW = 128.0f * scale;
	panelH = 96.0f * scale;

	/* bottom-left corner, 8px margin */
	x = 8.0f;
	y = 480.0f - panelH - 8.0f;

	barX = x + 8.0f * scale;
	barY = y + 16.0f * scale;
	barW = 112.0f * scale;
	barH = 8.0f * scale;

	vehicleW = 110.0f * scale;
	vehicleH = 60.0f * scale;
	vehicleX = x + ( panelW - vehicleW ) * 0.5f;
	vehicleY = y + 26.0f * scale;

	zoneFrontRearW = 42.0f * scale;
	zoneFrontRearH = 10.0f * scale;
	zoneSideW = 14.0f * scale;
	zoneSideH = 22.0f * scale;
	zoneFrontX = vehicleX + ( vehicleW - zoneFrontRearW ) * 0.5f;
	zoneFrontY = vehicleY + 2.0f * scale;
	zoneRearX = zoneFrontX;
	zoneRearY = vehicleY + vehicleH - zoneFrontRearH - 2.0f * scale;
	zoneSideY = vehicleY + ( vehicleH - zoneSideH ) * 0.5f;
	zoneLeftX = vehicleX + 6.0f * scale;
	zoneRightX = vehicleX + vehicleW - zoneSideW - 6.0f * scale;
	zoneRoofX = zoneFrontX;
	zoneRoofY = vehicleY + ( vehicleH - zoneFrontRearH ) * 0.5f;

	baseColor[0] = 0.02f; baseColor[1] = 0.02f; baseColor[2] = 0.02f; baseColor[3] = 0.70f;
	CG_FillRect( x, y, panelW, panelH, baseColor );
	CG_DrawRect( x, y, panelW, panelH, 1.0f * scale, colorWhite );
	if ( cgs.media.derbyHudPanelShader ) {
		trap_R_SetColor( colorWhite );
		CG_DrawPic( x, y, panelW, panelH, cgs.media.derbyHudPanelShader );
		trap_R_SetColor( NULL );
	}

	if ( healthFrac > 0.50f ) {
		statusColor[0] = 0.10f; statusColor[1] = 0.88f; statusColor[2] = 0.20f; statusColor[3] = 0.88f;
	} else if ( healthFrac > 0.25f ) {
		statusColor[0] = 0.94f; statusColor[1] = 0.84f; statusColor[2] = 0.08f; statusColor[3] = 0.90f;
	} else {
		statusColor[0] = 0.94f; statusColor[1] = 0.16f; statusColor[2] = 0.10f; statusColor[3] = 0.94f;
	}

	critical = ( healthFrac <= 0.25f ) ? qtrue : qfalse;
	neutralColor[0] = 0.85f; neutralColor[1] = 0.85f; neutralColor[2] = 0.88f; neutralColor[3] = 0.55f;
	barBackColor[0] = 0.10f; barBackColor[1] = 0.10f; barBackColor[2] = 0.12f; barBackColor[3] = 0.80f;
	flashColor[0] = 1.0f; flashColor[1] = 1.0f; flashColor[2] = 1.0f; flashColor[3] = 0.85f;

	hitSegment = -1;
	hasDirectionalHit = qfalse;
	{
		int curDmg = cg.snap->ps.stats[STAT_DAMAGE_TAKEN];
		if ( curDmg != cg.derbyLastDamageTaken ) {
			/* New hit detected - compute direction from velocity vs view yaw */
			vec3_t forward, right;
			float fwdDot, rightDot;
			vec3_t angles;

			VectorSet( angles, 0, cg.snap->ps.viewangles[YAW], 0 );
			AngleVectors( angles, forward, right, NULL );
			fwdDot   = DotProduct( cg.snap->ps.velocity, forward );
			rightDot = DotProduct( cg.snap->ps.velocity, right );

			cg.derbyHitFxTime   = cg.time;
			cg.derbyHitFxDamage = curDmg - cg.derbyLastDamageTaken;
			cg.derbyLastDamageTaken = curDmg;

			if ( fabs( fwdDot ) >= fabs( rightDot ) ) {
				cg.derbyHitFxDir = ( fwdDot >= 0.0f ) ? 0 : 3; /* 0=front 3=rear */
			} else {
				cg.derbyHitFxDir = ( rightDot >= 0.0f ) ? 2 : 1; /* 2=right 1=left */
			}
		}
	}
	if ( cg.derbyHitFxTime > 0 && cg.time - cg.derbyHitFxTime < lastHitFlashMs ) {
		hasDirectionalHit = qtrue;
		hitSegment = cg.derbyHitFxDir;
	}

	/* Option 1: primary integrity indicator (single health bar) */
	CG_FillRect( barX, barY, barW, barH, barBackColor );
	CG_FillRect( barX, barY, barW * healthFrac, barH, statusColor );
	CG_DrawRect( barX, barY, barW, barH, 1.0f * scale, colorWhite );

	if ( cgs.media.derbyHudVehicleShader ) {
		vec4_t tint;
		Vector4Copy( statusColor, tint );
		tint[3] = 0.65f;
		trap_R_SetColor( tint );
		CG_DrawPic( vehicleX, vehicleY, vehicleW, vehicleH, cgs.media.derbyHudVehicleShader );
		trap_R_SetColor( NULL );
	}

	/* neutral silhouette blocks (no fake per-zone damage state) */
	CG_FillRect( zoneFrontX, zoneFrontY, zoneFrontRearW, zoneFrontRearH, neutralColor ); /* front */
	CG_FillRect( zoneLeftX, zoneSideY, zoneSideW, zoneSideH, neutralColor ); /* left */
	CG_FillRect( zoneRightX, zoneSideY, zoneSideW, zoneSideH, neutralColor ); /* right */
	CG_FillRect( zoneRearX, zoneRearY, zoneFrontRearW, zoneFrontRearH, neutralColor ); /* rear */
	if ( cg_derbyVehicleHudRoof.integer ) {
		CG_FillRect( zoneRoofX, zoneRoofY, zoneFrontRearW, zoneFrontRearH, neutralColor );
	}

	/* Option 2: short last-hit direction flash */
	if ( hasDirectionalHit ) {
		switch ( hitSegment ) {
		case 0:
			CG_FillRect( zoneFrontX, zoneFrontY, zoneFrontRearW, zoneFrontRearH, flashColor );
			break;
		case 1:
			CG_FillRect( zoneLeftX, zoneSideY, zoneSideW, zoneSideH, flashColor );
			break;
		case 2:
			CG_FillRect( zoneRightX, zoneSideY, zoneSideW, zoneSideH, flashColor );
			break;
		case 3:
			CG_FillRect( zoneRearX, zoneRearY, zoneFrontRearW, zoneFrontRearH, flashColor );
			break;
		default:
			break;
		}
	}

	CG_DrawTinyStringColor( (int)( x + 8.0f * scale ), (int)( y + 6.0f * scale ), "INTEGRITY", colorWhite );

	if ( critical ) {
		float pulse = (float)( cg.time % 1000 ) * 0.001f;
		if ( pulse > 0.5f ) {
			pulse = 1.0f - pulse;
		}
		pulse *= 2.0f;
		pulseColor[0] = 1.0f; pulseColor[1] = 0.12f; pulseColor[2] = 0.08f; pulseColor[3] = 0.20f + 0.55f * pulse;
		CG_DrawRect( x - 1.0f * scale, y - 1.0f * scale, panelW + 2.0f * scale, panelH + 2.0f * scale, 2.0f * scale, pulseColor );
		CG_DrawBigStringColor( (int)( x + 18.0f * scale ), (int)( y + 30.0f * scale ), "!", pulseColor );
		if ( cgs.media.derbyHudWarningShader ) {
			trap_R_SetColor( pulseColor );
			CG_DrawPic( x + 90.0f * scale, y + 6.0f * scale, 28.0f * scale, 28.0f * scale, cgs.media.derbyHudWarningShader );
			trap_R_SetColor( NULL );
		}
	}
}

/*
====================
CG_DrawHUD_DerbyList
====================
*/
void CG_DrawHUD_DerbyList(float x, float y){
	int			i;
	vec4_t		color;
	centity_t	*cent;

	// draw heading
    x = 636 - 120;
	CG_FillRect(x, y, 120, 18, bgColor);

	// name
	CG_DrawTinyStringColor( x + 16, y, "P:", colorWhite);

	// time
//	CG_DrawTinyStringColor( x + 70, y, "TIME:", colorWhite);

	// dmg dealt
	CG_DrawTinyStringColor( x + 70, y, "DD:", colorWhite);

	// dmg taken
	CG_DrawTinyStringColor( x + 100, y, "DT:", colorWhite);

	y += 20;

	// draw top 8 players
	for (i = 0; i < 8; i++){
		if (cg.scores[i].scoreFlags < 0) continue; // score is not valid so skip it

		cent = &cg_entities[cg.scores[i].client];
		if (!cent) continue;

		CG_FillRect(x, y, 120, 18, bgColor);

		Vector4Copy(colorWhite, color);
		if (cg.scores[i].client == cg.snap->ps.clientNum){
			if (cg.snap->ps.stats[STAT_HEALTH] <= 0 || cgs.clientinfo[cg.scores[i].client].team == TEAM_SPECTATOR)
				Vector4Copy(colorMdGrey, color);
		}
		else if (cent->currentState.eFlags & EF_DEAD || cgs.clientinfo[cg.scores[i].client].team == TEAM_SPECTATOR){
			Vector4Copy(colorMdGrey, color);
		}

		// num
		CG_DrawTinyStringColor( x + 2, y, va("%i", (i+1)), color);

		// name
		CG_DrawTinyStringColor( x + 16, y, cgs.clientinfo[cg.scores[i].client].name, color);

		// time
//		CG_DrawTinyStringColor( x + 70, y, time, color);

		// dmg dealt
		CG_DrawTinyStringColor( x + 75, y, va("%i", cg.scores[i].damageDealt), color);

		// dmg taken
		CG_DrawTinyStringColor( x + 105, y, va("%i", cg.scores[i].damageTaken), color);

		y += 20;
	}
}


/*
================================
CG_DrawHUD - Draws the extra HUD
================================
*/
qboolean CG_DrawHUD( void ) {
	// don't draw anything if the menu or console is up
	if ( cg_paused.integer ) {
		return qfalse;
	}

	if ( !cg.showHUD ) {
		if ( cgs.gametype == GT_DERBY ) {
			/* keep vehicle state panel permanently visible in derby */
			CG_DrawHUD_DerbyVehicleState();
			return qfalse;
		}
		return qfalse;
	}

	// get new scores for accurate team dm times
	if ( cg.scoresRequestTime + 2000 < cg.time ){
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand( "score" );
	}

        switch(cgs.gametype){
        default:
        case GT_RACING:
        case GT_SPRINT:
        case GT_TEAM_RACING:
                CG_DrawHUD_Times(0, 112);
                CG_DrawHUD_Positions(0, 228);
                CG_DrawHUD_Laps(0, 304);

		break;

	case GT_ELIMINATION:
		CG_DrawHUD_Times(0, 112);
		CG_DrawHUD_Positions(0, 228);
		CG_DrawHUD_Laps(0, 304);
		CG_DrawHUD_OpponentList(440, 130);

		break;

	case GT_RACING_DM:
	case GT_TEAM_RACING_DM:
		CG_DrawHUD_Times(0, 112);
		CG_DrawHUD_Positions(0, 228);
		CG_DrawHUD_Laps(0, 304);
		CG_DrawHUD_Scores(264, 130);

		break;

	case GT_DEATHMATCH:
	case GT_TEAM:
	case GT_CTF:
	case GT_DOMINATION:
		CG_DrawHUD_Scores(264, 130);

		break;

	case GT_DERBY:
		CG_DrawHUD_DerbyVehicleState();
		CG_DrawHUD_DerbyList(440, 130);
		CG_DrawHUD_DerbyHitImpact();
		break;

	case GT_LCS:
		CG_DrawHUD_OpponentList(440, 130);

		break;
	}

	return qtrue;
}
