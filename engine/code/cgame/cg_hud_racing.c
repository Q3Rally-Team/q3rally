/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.
===========================================================================
*/

/*
===========================================================================
  cg_hud_racing.c

  All racing-specific HUD elements:
    - Checkpoint arrow / wrong-way warning
    - Times (lap, total, best, ghost)
    - Ghost split delta
    - Laps
    - Distance to finish
    - Current position
    - Elimination timeline
    - Car ahead/behind list
    - Panel-style variants: CG_DrawHUD_Times, _Positions, _Laps,
                            _OpponentList, _Scores
===========================================================================
*/

#include "cg_local.h"
#include "cg_hud_elements.h"

/* Shared constants (kept in sync with cg_hud_core.c via cg_local.h defines) */
#define HUD_RIGHT_EDGE          636.0f
#define HUD_COLUMN_SPACING        4.0f
#define HUD_TEXT_INSET            6.0f
#define HUD_ROW_HEIGHT          ((float)TINYCHAR_HEIGHT + 4.0f)


/* -----------------------------------------------------------------------
   CG_DrawArrowToCheckpoint
   Draws the 3D arrow model and "WRONG WAY!" flash.
   ----------------------------------------------------------------------- */
float CG_DrawArrowToCheckpoint( float y ) {
	centity_t	*cent;
	vec3_t		forward, origin, angles;
	int			i;
	float		angle1, angle2, angleDiff;
	int			x, w;
	float		fx, fy, fw, fh;
	float		*color;
	refdef_t	refdef;
	refEntity_t	ent;
	vec3_t		mins, maxs, v;

	if ( cg_entities[cg.snap->ps.clientNum].finishRaceTime )
		return y;

	for ( i = 0; i < MAX_GENTITIES; i++ ) {
		cent = &cg_entities[i];
		if ( cent->currentState.eType != ET_CHECKPOINT ) continue;
		if ( cent->currentState.weapon != cg.snap->ps.stats[STAT_NEXT_CHECKPOINT] ) continue;
		break;
	}

	if ( i == MAX_GENTITIES )
		return y;

	trap_R_ModelBounds( cgs.inlineDrawModel[cent->currentState.modelindex], mins, maxs );

	if ( cent->currentState.frame == 0 ) {
		VectorAdd( mins, cent->currentState.origin, mins );
		VectorAdd( maxs, cent->currentState.origin, maxs );
	}

	for ( i = 0; i < 3; i++ ) {
		if      ( cg.predictedPlayerState.origin[i] < mins[i] ) v[i] = mins[i] - cg.predictedPlayerState.origin[i];
		else if ( cg.predictedPlayerState.origin[i] > maxs[i] ) v[i] = maxs[i] - cg.predictedPlayerState.origin[i];
		else                                                      v[i] = 0;
	}

	angle2 = ( v[0] == 0 && v[1] == 0 && v[2] == 0 )
	         ? cg.predictedPlayerState.viewangles[YAW]
	         : vectoyaw( v );

	if ( cg_checkpointArrowMode.integer == 1 ) {
		AngleVectors( cg.refdefViewAngles, forward, NULL, NULL );
		angle1     = vectoyaw( forward );
		angleDiff  = AngleDifference( angle1, angle2 );

		VectorSet( origin, 80, 0, 20 );
		VectorClear( angles );
		angles[YAW] = -angleDiff;

		fx = 320 - 64;  fy = 16;  fw = 128;  fh = 96;
		CG_AdjustFrom640( &fx, &fy, &fw, &fh );

		memset( &refdef, 0, sizeof( refdef ) );
		memset( &ent,    0, sizeof( ent    ) );

		AnglesToAxis( angles, ent.axis );
		VectorCopy( origin, ent.origin );
		VectorCopy( origin, ent.lightingOrigin );
		ent.hModel   = cgs.media.checkpointArrow;
		ent.renderfx = RF_NOSHADOW;

		refdef.rdflags = RDF_NOWORLDMODEL;
		vectoangles( origin, angles );
		AnglesToAxis( angles, refdef.viewaxis );
		refdef.fov_x  = 40;
		refdef.fov_y  = 30;
		refdef.x      = fx;  refdef.y      = fy;
		refdef.width  = fw;  refdef.height = fh;
		refdef.time   = cg.time;

		trap_R_ClearScene();
		trap_R_AddRefEntityToScene( &ent );
		trap_R_RenderScene( &refdef );
	}

	AngleVectors( cg.predictedPlayerEntity.lerpAngles, forward, NULL, NULL );
	angle1    = vectoyaw( forward );
	angleDiff = AngleDifference( angle1, angle2 );

	if ( fabs( angleDiff ) > 100 ) {
		cg.wrongWayTime = cg.time;
		if ( !cg.wrongWayStartTime )
			cg.wrongWayStartTime = cg.time;
	} else {
		cg.wrongWayStartTime = 0;
	}

	if ( !cg.wrongWayStartTime || cg.wrongWayStartTime > cg.time - 2000 )
		return y;

	w = BIGCHAR_WIDTH * CG_DrawStrlen( "WRONG WAY!" );
	x = ( SCREEN_WIDTH - w ) / 2;

	color = CG_FadeColor( cg.wrongWayTime, 300 );
	if ( !color )
		return y;

	trap_R_SetColor( color );
	CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );
	CG_DrawStringExt( x, SCREEN_HEIGHT * .30f, "WRONG WAY!", color,
	                  qfalse, qtrue, BIGCHAR_WIDTH, (int)(BIGCHAR_WIDTH * 1.5), 0 );
	CG_PopScreenPlacement();

	return y;
}


/* -----------------------------------------------------------------------
   CG_DrawTimes  (right-column compact panel)
   ----------------------------------------------------------------------- */
float CG_DrawTimes( float y ) {
	centity_t	*cent;
	int			lapTime, totalTime;
	float		x;
	char		s[128];
	char		*time;
	const float	columnWidth = CG_GetEliminationColumnWidth();
	const float	rowHeight   = HUD_ROW_HEIGHT;

	cent = &cg_entities[cg.snap->ps.clientNum];

	if ( cent->finishRaceTime ) {
		lapTime   = cent->finishRaceTime - cent->startLapTime;
		totalTime = cent->finishRaceTime - cent->startRaceTime;
	} else if ( cent->startRaceTime ) {
		lapTime   = cg.time - cent->startLapTime;
		totalTime = cg.time - cent->startRaceTime;
	} else {
		lapTime = totalTime = 0;
	}

	x = HUD_RIGHT_EDGE - columnWidth;

	if ( cgs.laplimit > 1 && cgs.gametype != GT_DERBY && cgs.gametype != GT_LCS ) {
		time = getStringForTime( cent->bestLapTime );
		Com_sprintf( s, sizeof(s), "B: %s", time );
		CG_FillRect( x, y, columnWidth, rowHeight, bgColor );
		CG_DrawTinyStringColor( x + HUD_TEXT_INSET, y + 4, s, colorWhite );
		y += rowHeight;

		time = getStringForTime( lapTime );
		Com_sprintf( s, sizeof(s), "L: %s", time );
		CG_FillRect( x, y, columnWidth, rowHeight, bgColor );
		CG_DrawTinyStringColor( x + HUD_TEXT_INSET, y + 4, s, colorWhite );
		y += rowHeight;
	}

	time = getStringForTime( totalTime );
	Com_sprintf( s, sizeof(s), "T: %s", time );
	CG_FillRect( x, y, columnWidth, rowHeight, bgColor );
	CG_DrawTinyStringColor( x + HUD_TEXT_INSET, y + 4, s, colorWhite );
	y += rowHeight;

	return y;
}


/* -----------------------------------------------------------------------
   CG_DrawGhostSplitDelta
   ----------------------------------------------------------------------- */
float CG_DrawGhostSplitDelta( float y ) {
	char		s[64];
	float		x;
	int			deltaMs, absMs, seconds, millis;
	vec4_t		deltaColor;
	const float	columnWidth = CG_GetEliminationColumnWidth();
	const float	rowHeight   = HUD_ROW_HEIGHT;

	if ( !cg.ghostSplitDeltaValid )
		return y;

	if ( cg.ghostSplitDeltaTime <= 0 || cg.time - cg.ghostSplitDeltaTime > 4000 ) {
		if ( !cg.snap || !cg_entities[cg.snap->ps.clientNum].finishRaceTime )
			return y;
	}

	deltaMs = cg.ghostSplitDeltaMs;
	absMs   = deltaMs < 0 ? -deltaMs : deltaMs;
	seconds = absMs / 1000;
	millis  = absMs % 1000;

	Com_sprintf( s, sizeof(s), "D: %c%d.%03d", deltaMs < 0 ? '-' : '+', seconds, millis );

	if      ( deltaMs < 0 ) Vector4Copy( colorGreen, deltaColor );
	else if ( deltaMs > 0 ) Vector4Copy( colorRed,   deltaColor );
	else                    Vector4Copy( colorWhite,  deltaColor );

	x = HUD_RIGHT_EDGE - columnWidth;
	CG_FillRect( x, y, columnWidth, rowHeight, bgColor );
	CG_DrawTinyStringColor( x + HUD_TEXT_INSET, y + 4, s, deltaColor );
	y += rowHeight;

	return y;
}


/* -----------------------------------------------------------------------
   CG_DrawLaps
   ----------------------------------------------------------------------- */
float CG_DrawLaps( float y ) {
	centity_t	*cent;
	char		s[64];
	float		x;
	const float	columnWidth = CG_GetEliminationColumnWidth();
	const float	rowHeight   = HUD_ROW_HEIGHT;

	cent = &cg_entities[cg.snap->ps.clientNum];

	if ( cgs.gametype == GT_SPRINT ) {
		Q_strncpyz( s, "SPRINT", sizeof(s) );
	} else if ( cgs.laplimit > 1 ) {
		Com_sprintf( s, sizeof(s), "LAP: %i/%i", cent->currentLap, cgs.laplimit );
	} else {
		Com_sprintf( s, sizeof(s), "LAP: %i", cent->currentLap );
	}

	x = HUD_RIGHT_EDGE - columnWidth;
	CG_FillRect( x, y, columnWidth, rowHeight, bgColor );
	CG_DrawTinyStringColor( x + HUD_TEXT_INSET, y + 4, s, colorWhite );
	y += rowHeight;

	return y;
}


/* -----------------------------------------------------------------------
   CG_DrawDistanceToFinish
   ----------------------------------------------------------------------- */
float CG_DrawDistanceToFinish( float y ) {
	char		s[64];
	float		x, dist;
	const float	columnWidth = CG_GetEliminationColumnWidth();
	const float	rowHeight   = HUD_ROW_HEIGHT;

	dist = cg.snap->ps.stats[STAT_DISTANCE_REMAIN];

	if ( cg_distanceFormat.integer == 1 && cgs.trackLength > 0.0f ) {
		float percent = dist / cgs.trackLength * 100.0f;
		Com_sprintf( s, sizeof(s), "DIST: %.1f%%", percent );
	} else {
		Com_sprintf( s, sizeof(s), "DIST: %dm", (int)dist );
	}

	x = HUD_RIGHT_EDGE - columnWidth;
	CG_FillRect( x, y, columnWidth, rowHeight, bgColor );
	CG_DrawTinyStringColor( x + HUD_TEXT_INSET, y + 4, s, colorWhite );
	y += rowHeight;

	return y;
}


/* -----------------------------------------------------------------------
   CG_DrawCurrentPosition
   ----------------------------------------------------------------------- */
void CG_DrawCurrentPosition( float y ) {
	centity_t	*cent;
	int			pos, remaining;
	char		s[64];
	float		baseX, textX, textY;
	float		width, height;
	qboolean	showPosition;
	const float	columnWidth = CG_GetEliminationColumnWidth();
	const float	rowHeight   = HUD_ROW_HEIGHT;

	cent = &cg_entities[cg.snap->ps.clientNum];

	pos       = cent->currentPosition;
	remaining = CG_GetPlayersRemaining( NULL );

	baseX        = (HUD_RIGHT_EDGE - columnWidth) - HUD_COLUMN_SPACING - columnWidth;
	width        = columnWidth;
	showPosition = ( cgs.gametype != GT_LCS );
	height       = showPosition ? 36.0f : rowHeight;

	CG_FillRect( baseX, y, width, height, bgColor );

	textX = baseX + HUD_TEXT_INSET;
	textY = y + 4;

	if ( showPosition ) {
		CG_DrawTinyStringColor( textX, textY, "POS:", colorWhite );
		CG_DrawTinyStringColor( textX + TINYCHAR_WIDTH * 5, textY,
		                        va("%i/%i", pos, cgs.numRacers), colorWhite );
		textY += rowHeight;
	}

	if ( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_LCS ) {
		if ( remaining > 0 )
			Com_sprintf( s, sizeof(s), "PLAYERS LEFT: %02i", remaining );
		else
			Com_sprintf( s, sizeof(s), "PLAYERS LEFT: --" );
		CG_DrawTinyStringColor( textX, textY, s, colorWhite );
	}
}


/* -----------------------------------------------------------------------
   CG_DrawEliminationTimeline
   ----------------------------------------------------------------------- */
float CG_DrawEliminationTimeline( float y ) {
	int			i;
	float		x;
	char		line[64];
	const float	columnWidth = CG_GetEliminationColumnWidth();
	const float	rowHeight   = HUD_ROW_HEIGHT;

	if ( !cg_elimTimeline.integer )                                       return y;
	if ( cgs.gametype != GT_ELIMINATION && cgs.gametype != GT_LCS )       return y;
	if ( cg.elimTimelineCount <= 0 )                                      return y;

	x = HUD_RIGHT_EDGE - columnWidth;

	for ( i = cg.elimTimelineCount - 1; i >= 0; --i ) {
		const cgElimTimelineEvent_t *event = &cg.elimTimelineEvents[i];
		const char *name;
		int elapsedSeconds = 0;

		if ( event->clientNum < 0 || event->clientNum >= MAX_CLIENTS ) continue;

		name = cgs.clientinfo[event->clientNum].name;
		if ( !name || !name[0] ) name = va("#%d", event->clientNum);

		if ( event->timestamp > 0 && cg.time > event->timestamp )
			elapsedSeconds = ( cg.time - event->timestamp ) / 1000;

		Com_sprintf( line, sizeof(line), "R%02d LEFT%02d %s (%is)",
		             event->round, event->remaining, name, elapsedSeconds );

		CG_FillRect( x, y, columnWidth, rowHeight, bgColor );
		CG_DrawTinyStringColor( x + HUD_TEXT_INSET, y + 4, line, colorWhite );
		y += rowHeight;
	}

	return y;
}


/* -----------------------------------------------------------------------
   CG_DrawCarAheadAndBehind
   ----------------------------------------------------------------------- */
float CG_DrawCarAheadAndBehind( float y ) {
	centity_t	*cent, *other;
	char		player[64], s[64];
	int			i, j, num;
	float		x, width, height;
	int			startPos, endPos;
	float		background[4]    = { 0,    0,    0,    0.5f };
	float		selected[4]      = { 0.75, 0.0,  0.0,  0.5f };
	float		lastPlaceColor[4]= { 0.8f, 0.2f, 0.2f, 0.4f };
	int			playersRemaining, lastClient, lastPosition;
	const float	columnWidth = CG_GetEliminationColumnWidth();

	cent             = &cg_entities[cg.snap->ps.clientNum];
	playersRemaining = CG_GetPlayersRemaining( &lastClient );
	lastPosition     = 0;

	if ( lastClient >= 0 ) {
		lastPosition = cg_entities[lastClient].currentPosition;
		if ( lastPosition <= 0 )
			lastPosition = cgs.clientinfo[lastClient].position;
	}

	startPos = cent->currentPosition - 4 < 1            ? 1              : cent->currentPosition - 4;
	endPos   = startPos + 8 > cgs.numRacers              ? cgs.numRacers  : startPos + 8;
	startPos = endPos   - 8 < 1                          ? 1              : endPos - 8;

	x      = HUD_RIGHT_EDGE - columnWidth;
	width  = columnWidth;
	height = TINYCHAR_HEIGHT;

	if ( endPos >= startPos )
		y += HUD_ROW_HEIGHT - (float)TINYCHAR_HEIGHT;

	for ( i = startPos; i <= endPos; i++ ) {
		num = -1;
		for ( j = 0; j < cgs.maxclients; j++ ) {
			other = &cg_entities[j];
			if ( !other ) continue;
			if ( other->currentPosition == i ) {
				num = other->currentState.clientNum;
			}
		}

		if ( num < 0 || num > cgs.maxclients ) continue;

		CG_FillRect( x, y, width, height, background );

		if ( (cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_LCS)
		     && playersRemaining > 1 && lastPosition > 0 ) {
			int rowPosition = cg_entities[num].currentPosition;
			if ( rowPosition <= 0 ) rowPosition = cgs.clientinfo[num].position;
			if ( rowPosition == lastPosition )
				CG_FillRect( x, y, width, height, lastPlaceColor );
		}

		if ( num == cent->currentState.clientNum )
			CG_FillRect( x, y, width, height, selected );

		Q_strncpyz( player, cgs.clientinfo[num].name, 16 );
		{
			int rowPosition = cg_entities[num].currentPosition;
			if ( rowPosition <= 0 ) rowPosition = cgs.clientinfo[num].position;
			Com_sprintf( s, sizeof(s), "%i-%s", rowPosition, player );
		}
		CG_DrawTinyStringColor( x + HUD_TEXT_INSET, y, s, colorWhite );

		y += TINYCHAR_HEIGHT;
	}

	return y;
}


/* =======================================================================
   PANEL-STYLE HUD FUNCTIONS  (left-side panels, larger text)
   ======================================================================= */

/*
================
CG_DrawHUD_Times
================
*/
void CG_DrawHUD_Times( float x, float y ) {
	centity_t	*cent;
	int			lapTime, lastTime, totalTime, teamTime;
	char		*time;
	int			i, count = 0;

	cent = &cg_entities[cg.snap->ps.clientNum];

	if ( cent->finishRaceTime ) {
		lapTime   = cent->finishRaceTime - cent->startLapTime;
		totalTime = cent->finishRaceTime - cent->startRaceTime;
	} else if ( cent->startRaceTime ) {
		lapTime   = cg.time - cent->startLapTime;
		totalTime = cg.time - cent->startRaceTime;
	} else {
		lapTime = totalTime = 0;
	}

	lastTime = cent->lastStartLapTime ? cent->startLapTime - cent->lastStartLapTime : 0;

	CG_FillRect( x, y, 170, 18, bgColor );
	CG_DrawSmallStringColor( x + 12, y, "TIMES", colorWhite );
	y += 20;

	if ( cgs.laplimit > 1 ) {
		CG_DrawSmallStringColor( x + 12, y, "LAP:", colorWhite );
		time = getStringForTime( lapTime );
		CG_DrawSmallStringColor( x + 102, y, time, colorWhite );
		y += 20;

		CG_DrawSmallStringColor( x + 12, y, "LAST:", colorWhite );
		if ( lastTime ) {
			time = getStringForTime( lastTime );
			CG_DrawSmallStringColor( x + 102, y, time, colorWhite );
		} else {
			CG_DrawSmallStringColor( x + 102, y, "N/A", colorWhite );
		}
		y += 20;
	}

	CG_DrawSmallStringColor( x + 12, y, "TOTAL:", colorWhite );
	time = getStringForTime( totalTime );
	CG_DrawSmallStringColor( x + 102, y, time, colorWhite );
	y += 20;

	/* Ghost playback state */
	CG_DrawSmallStringColor( x + 12, y, "GHOST:", colorWhite );
	if ( cg_ghostPlayback.integer == 1 && cg.ghostPlayback.valid ) {
		const char *best = cg.personalGhostBestTime > 0
		                   ? getStringForTime( cg.personalGhostBestTime ) : "ready";
		CG_DrawSmallStringColor( x + 102, y, va("On (%s)", best), colorWhite );
	} else if ( cg_ghostPlayback.integer == 2 && cg.baseGhostAvailable ) {
		const char *best = cg.baseGhostBestTime > 0
		                   ? getStringForTime( cg.baseGhostBestTime ) : "ready";
		CG_DrawSmallStringColor( x + 102, y, va("Base (%s)", best), colorWhite );
	} else {
		CG_DrawSmallStringColor( x + 102, y, "Off", colorWhite );
	}
	y += 20;

	/* Team time */
	if ( cgs.gametype == GT_TEAM_RACING || cgs.gametype == GT_TEAM_RACING_DM ) {
		teamTime = 0;
		for ( i = 0; i < cgs.maxclients; i++ ) {
			if ( !cgs.clientinfo[i].infoValid ) continue;
			if ( cgs.clientinfo[i].team == cgs.clientinfo[cg.snap->ps.clientNum].team ) {
				count++;
				cent = &cg_entities[i];
				if      ( cent->finishRaceTime ) teamTime += cent->finishRaceTime - cent->startLapTime;
				else if ( cent->startRaceTime  ) teamTime += cg.time             - cent->startRaceTime;
			}
		}

		if ( cgs.gametype == GT_TEAM_RACING_DM ) {
			switch ( cgs.clientinfo[cg.snap->ps.clientNum].team ) {
			case TEAM_RED:    if ( cgs.scores1 > 0 ) teamTime -= cgs.scores1 * TIME_BONUS_PER_FRAG; break;
			case TEAM_BLUE:   if ( cgs.scores2 > 0 ) teamTime -= cgs.scores2 * TIME_BONUS_PER_FRAG; break;
			case TEAM_GREEN:  if ( cgs.scores3 > 0 ) teamTime -= cgs.scores3 * TIME_BONUS_PER_FRAG; break;
			case TEAM_YELLOW: if ( cgs.scores4 > 0 ) teamTime -= cgs.scores4 * TIME_BONUS_PER_FRAG; break;
			default: break;
			}
		}

		if ( count > 0 ) teamTime /= count;

		CG_DrawSmallStringColor( x + 12, y, "TEAM:", colorWhite );
		time = getStringForTime( teamTime );
		CG_DrawSmallStringColor( x + 102, y, time, colorWhite );
	}
}


/*
====================
CG_DrawHUD_Positions
====================
*/
void CG_DrawHUD_Positions( float x, float y ) {
	int i, num_teams = 0, team_rank = 0;

	for ( i = 0; i < 4; i++ ) {
		if ( !TeamCount( -1, TEAM_RED + i ) ) continue;
		num_teams++;
	}
	for ( i = 0; i < 4; i++ ) {
		if ( GetTeamAtRank( i + 1 ) == cgs.clientinfo[cg.snap->ps.clientNum].team )
			team_rank = i + 1;
	}

	CG_FillRect( x, y, 170, 18, bgColor );
	CG_DrawSmallStringColor( x + 12, y, "POSITION", colorWhite );
	y += 20;

	CG_DrawSmallStringColor( x + 12, y, "YOU:", colorWhite );
	CG_DrawSmallStringColor( x + 102, y,
	    va("%i/%i", cg_entities[cg.snap->ps.clientNum].currentPosition, cgs.numRacers),
	    colorWhite );
	y += 20;

	if ( cgs.gametype == GT_TEAM_RACING || cgs.gametype == GT_TEAM_RACING_DM ) {
		CG_DrawSmallStringColor( x + 12, y, "TEAM:", colorWhite );
		CG_DrawSmallStringColor( x + 102, y, va("%i/%i", team_rank, num_teams), colorWhite );
	}
}


/*
===============
CG_DrawHUD_Laps
===============
*/
void CG_DrawHUD_Laps( float x, float y ) {
	CG_FillRect( x, y, 170, 18, bgColor );

	if ( cgs.gametype == GT_SPRINT ) {
		CG_DrawSmallStringColor( x + 12, y, "SPRINT:", colorWhite );
		CG_DrawSmallStringColor( x + 102, y, "POINT-TO-POINT", colorWhite );
		return;
	}

	CG_DrawSmallStringColor( x + 12, y, "LAP:", colorWhite );
	if ( cgs.laplimit > 1 )
		CG_DrawSmallStringColor( x + 102, y,
		    va("%i/%i", cg_entities[cg.snap->ps.clientNum].currentLap, cgs.laplimit),
		    colorWhite );
	else
		CG_DrawSmallStringColor( x + 102, y,
		    va("%i", cg_entities[cg.snap->ps.clientNum].currentLap),
		    colorWhite );
}


/*
=======================
CG_DrawHUD_OpponentList
=======================
*/
void CG_DrawHUD_OpponentList( float x, float y ) {
	centity_t	*cent, *other;
	char		player[64], s[64];
	int			i, j, num;
	float		width = 198, height = 18;
	int			startPos, endPos;
	vec4_t		color;
	float		lastPlaceColor[4] = { 0.8f, 0.2f, 0.2f, 0.4f };
	int			playersRemaining, lastClient, lastPosition;

	cent             = &cg_entities[cg.snap->ps.clientNum];
	playersRemaining = CG_GetPlayersRemaining( &lastClient );
	lastPosition     = 0;

	if ( lastClient >= 0 ) {
		lastPosition = cg_entities[lastClient].currentPosition;
		if ( lastPosition <= 0 )
			lastPosition = cgs.clientinfo[lastClient].position;
	}

	startPos = cent->currentPosition - 4 < 1           ? 1             : cent->currentPosition - 4;
	endPos   = startPos + 8 > cgs.numRacers             ? cgs.numRacers : startPos + 8;
	startPos = endPos   - 8 < 1                         ? 1             : endPos - 8;

	CG_FillRect( x, y, width, height, bgColor );
	CG_DrawSmallStringColor( x,      y, "POS:", colorWhite );
	CG_DrawSmallStringColor( x + 82, y, va("%i/%i", cent->currentPosition, cgs.numRacers), colorWhite );
	y += 20;

	if ( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_LCS ) {
		CG_FillRect( x, y, width, height, bgColor );
		if ( playersRemaining > 0 ) {
			CG_DrawSmallStringColor( x,       y, "PLAYERS REMAINING:", colorWhite );
			CG_DrawSmallStringColor( x + 150, y, va("%i", playersRemaining), colorWhite );
		} else {
			CG_DrawSmallStringColor( x,       y, "PLAYERS REMAINING:", colorMdGrey );
			CG_DrawSmallStringColor( x + 150, y, "--", colorMdGrey );
		}
		y += 20;
	}

	for ( i = startPos; i <= endPos; i++ ) {
		num = -1;
		for ( j = 0; j < cgs.maxclients; j++ ) {
			other = &cg_entities[j];
			if ( !other ) continue;
			if ( cgs.clientinfo[other->currentState.clientNum].team == TEAM_SPECTATOR ) continue;
			if ( cgs.clientinfo[other->currentState.clientNum].position == i ) {
				num = other->currentState.clientNum;
				break;
			}
		}

		if ( num < 0 || num >= cgs.maxclients ) continue;

		if      ( cgs.clientinfo[num].team == TEAM_RED    ) { Vector4Copy( colorRed,    color ); color[3] = 0.5f; }
		else if ( cgs.clientinfo[num].team == TEAM_BLUE   ) { Vector4Copy( colorBlue,   color ); color[3] = 0.5f; }
		else if ( cgs.clientinfo[num].team == TEAM_GREEN  ) { Vector4Copy( colorGreen,  color ); color[3] = 0.5f; }
		else if ( cgs.clientinfo[num].team == TEAM_YELLOW ) { Vector4Copy( colorYellow, color ); color[3] = 0.5f; }
		else                                                  { Vector4Copy( bgColor,     color );                  }

		CG_FillRect( x, y, width, height, color );

		if ( (cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_LCS)
		     && playersRemaining > 1 && lastPosition > 0 ) {
			int rowPosition = cgs.clientinfo[num].position;
			if ( rowPosition == 0 ) rowPosition = cg_entities[num].currentPosition;
			if ( rowPosition == lastPosition )
				CG_FillRect( x, y, width, height, lastPlaceColor );
		}

		Q_strncpyz( player, cgs.clientinfo[num].name, 16 );
		Com_sprintf( s, sizeof(s), " %i: %s", cgs.clientinfo[num].position, player );
		CG_DrawSmallStringColor( x, y, s, colorWhite );

		y += 20;
	}
}


/*
=================
CG_DrawHUD_Scores
=================
*/
void CG_DrawHUD_Scores( float x, float y ) {
	int		score, w;
	char	*s;

	s = va("%i", cgs.clientinfo[cg.snap->ps.clientNum].score);
	w = CG_DrawStrlen( s ) * GIANTCHAR_WIDTH;

	CG_FillRect( x, y, 96, 72, bgColor );
	CG_DrawSmallStringColor( x + 20, y + 6, "FRAGS:", colorWhite );
	CG_DrawStringExt( x + 48 - w/2, y + 26, s, colorWhite, qtrue, qfalse,
	                  GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );
	y += 72;

	if ( cgs.gametype >= GT_TEAM ) {
		switch ( cg.snap->ps.persistant[PERS_TEAM] ) {
		default:
		case TEAM_RED:    score = cgs.scores1; break;
		case TEAM_BLUE:   score = cgs.scores2; break;
		case TEAM_GREEN:  score = cgs.scores3; break;
		case TEAM_YELLOW: score = cgs.scores4; break;
		}

		s = va("%i", score);
		w = CG_DrawStrlen( s ) * GIANTCHAR_WIDTH;
		CG_FillRect( x, y, 96, 78, bgColor );
		CG_DrawSmallStringColor( x + 24, y + 6, "TEAM:", colorWhite );
		CG_DrawStringExt( x + 48 - w/2, y + 26, s, colorWhite, qtrue, qfalse,
		                  GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );
		y += 78;
	}

	CG_DrawScores( x + 96, y );
}
