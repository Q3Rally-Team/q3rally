/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.
===========================================================================
*/

/*
===========================================================================
  cg_hud_derby.c

  Derby-mode HUD elements:
    - CG_DrawHUD_DerbyHitImpact  : fullscreen colour flash on collision
    - CG_DrawHUD_DerbyVehicleState: integrity bar + directional zone panel
    - CG_DrawHUD_DerbyList        : compact modern driver list and integrity bars
===========================================================================
*/

#include "cg_local.h"
#include "cg_hud_elements.h"


/*
========================
CG_DrawHUD_DerbyHitImpact
Fullscreen coloured overlay that flashes when the player takes a hit.
========================
*/
void CG_DrawHUD_DerbyHitImpact( void ) {
	vec4_t  color;
	float   alpha, frac, overlayScale, damageBoost;
	int     elapsed, duration;

	if ( cgs.gametype != GT_DERBY || !cg_derbyHitFxEnable.integer )
		return;
	if ( !cg.derbyHitFxTime )
		return;

	elapsed  = cg.time - cg.derbyHitFxTime;
	duration = cg_derbyHitOverlayTime.integer;
	if ( duration < 120 ) duration = 120;
	if ( elapsed < 0 || elapsed >= duration ) return;

	frac         = 1.0f - (float)elapsed / (float)duration;
	overlayScale = cg_derbyHitOverlayScale.value;
	if ( overlayScale < 0.0f ) overlayScale = 0.0f;

	damageBoost = (float)cg.derbyHitFxDamage * (1.0f / 40.0f);
	if ( damageBoost > 0.35f ) damageBoost = 0.35f;

	switch ( cg.derbyHitFxLevel ) {
	default:
	case 0: color[0] = 1.0f; color[1] = 1.0f;  color[2] = 1.0f;  alpha = 0.14f; break;
	case 1: color[0] = 1.0f; color[1] = 0.72f; color[2] = 0.15f; alpha = 0.24f; break;
	case 2: color[0] = 1.0f; color[1] = 0.2f;  color[2] = 0.1f;  alpha = 0.34f; break;
	}

	alpha = ( alpha + damageBoost ) * overlayScale;
	if ( alpha > 0.80f ) alpha = 0.80f;
	color[3] = alpha * frac;

	CG_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color );
}


/*
============================
CG_DrawHUD_DerbyVehicleState
Bottom-left panel: integrity bar + coloured zone diagram + critical pulse.
============================
*/
void CG_DrawHUD_DerbyVehicleState( void ) {
	const int   health      = cg.snap->ps.stats[STAT_HEALTH];
	const float maxHealth   = 100.0f;
	const int   lastHitFlashMs = 1100;
	float   healthFrac;
	float   hitFrac;
	float   x, y, scale;
	float   panelW, panelH;
	float   barX, barY, barW, barH;
	float   vehicleX, vehicleY, vehicleW, vehicleH;
	float   zoneFrontRearW, zoneFrontRearH;
	float   zoneSideW, zoneSideH;
	float   zoneFrontX, zoneFrontY;
	float   zoneRearX,  zoneRearY;
	float   zoneSideY;
	float   zoneLeftX,  zoneRightX;
	float   zoneRoofX,  zoneRoofY;
	vec4_t  baseColor, statusColor, neutralColor;
	vec4_t  barBackColor, flashColor, pulseColor;
	int     hitSegment, hitElapsed;
	qboolean hasDirectionalHit, critical;
	screenPlacement_e savedHorizontalPlacement, savedVerticalPlacement;

	if ( cgs.gametype != GT_DERBY ) return;

	savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
	savedVerticalPlacement = CG_GetScreenVerticalPlacement();
	CG_SetScreenPlacement( PLACE_LEFT, PLACE_BOTTOM );

	healthFrac = health / maxHealth;
	if      ( healthFrac < 0.0f ) healthFrac = 0.0f;
	else if ( healthFrac > 1.0f ) healthFrac = 1.0f;

	scale = cg_derbyVehicleHudScale.value;
	if      ( scale < 0.5f ) scale = 0.5f;
	else if ( scale > 2.0f ) scale = 2.0f;

	panelW = 128.0f * scale;
	panelH =  96.0f * scale;
	x      = 8.0f;
	y      = 480.0f - panelH - 8.0f;

	barX = x + 8.0f  * scale;
	barY = y + 16.0f * scale;
	barW = 112.0f * scale;
	barH = 8.0f   * scale;

	vehicleW = 110.0f * scale;
	vehicleH =  60.0f * scale;
	vehicleX = x + ( panelW - vehicleW ) * 0.5f;
	vehicleY = y + 26.0f * scale;

	zoneFrontRearW = 42.0f * scale;
	zoneFrontRearH = 10.0f * scale;
	zoneSideW      = 14.0f * scale;
	zoneSideH      = 22.0f * scale;
	zoneFrontX = vehicleX + ( vehicleW - zoneFrontRearW ) * 0.5f;
	zoneFrontY = vehicleY + 2.0f * scale;
	zoneRearX  = zoneFrontX;
	zoneRearY  = vehicleY + vehicleH - zoneFrontRearH - 2.0f * scale;
	zoneSideY  = vehicleY + ( vehicleH - zoneSideH ) * 0.5f;
	zoneLeftX  = vehicleX + 6.0f * scale;
	zoneRightX = vehicleX + vehicleW - zoneSideW - 6.0f * scale;
	zoneRoofX  = zoneFrontX;
	zoneRoofY  = vehicleY + ( vehicleH - zoneFrontRearH ) * 0.5f;

	baseColor[0] = 0.02f; baseColor[1] = 0.02f; baseColor[2] = 0.02f; baseColor[3] = 0.70f;
	CG_FillRect( x, y, panelW, panelH, baseColor );
	CG_DrawRect( x, y, panelW, panelH, 1.0f * scale, colorWhite );
	if ( cgs.media.derbyHudPanelShader ) {
		trap_R_SetColor( colorWhite );
		CG_DrawPic( x, y, panelW, panelH, cgs.media.derbyHudPanelShader );
		trap_R_SetColor( NULL );
	}

	if      ( healthFrac > 0.50f ) { statusColor[0]=0.10f; statusColor[1]=0.88f; statusColor[2]=0.20f; statusColor[3]=0.88f; }
	else if ( healthFrac > 0.25f ) { statusColor[0]=0.94f; statusColor[1]=0.84f; statusColor[2]=0.08f; statusColor[3]=0.90f; }
	else                           { statusColor[0]=0.94f; statusColor[1]=0.16f; statusColor[2]=0.10f; statusColor[3]=0.94f; }

	critical = ( healthFrac <= 0.25f ) ? qtrue : qfalse;
	neutralColor[0]=0.85f; neutralColor[1]=0.85f; neutralColor[2]=0.88f; neutralColor[3]=0.55f;
	barBackColor[0]=0.10f; barBackColor[1]=0.10f; barBackColor[2]=0.12f; barBackColor[3]=0.80f;
	flashColor[0]=1.0f; flashColor[1]=1.0f; flashColor[2]=1.0f; flashColor[3]=0.85f;

	/* Direction is supplied by the actual damage event, not guessed from speed. */
	hitSegment = cg.derbyHitFxDir;
	hitElapsed = cg.time - cg.derbyHitFxTime;
	hasDirectionalHit = ( cg.derbyHitFxTime > 0 && hitElapsed >= 0 &&
	                      hitElapsed < lastHitFlashMs && hitSegment >= 0 && hitSegment <= 3 );
	if ( hasDirectionalHit ) {
		hitFrac = 1.0f - (float)hitElapsed / (float)lastHitFlashMs;
		flashColor[3] = 0.85f * hitFrac;
	} else if ( cg.derbyHitFxTime > 0 && hitElapsed >= 0 &&
	            hitElapsed < lastHitFlashMs && cg.derbyHitFxDir < 0 ) {
		hitFrac = 1.0f - (float)hitElapsed / (float)lastHitFlashMs;
		flashColor[3] = 0.55f * hitFrac;
		hasDirectionalHit = qtrue;
		hitSegment = -2;
	}
	/* Health bar */
	CG_FillRect( barX, barY, barW,              barH, barBackColor );
	CG_FillRect( barX, barY, barW * healthFrac, barH, statusColor  );
	CG_DrawRect( barX, barY, barW,              barH, 1.0f * scale, colorWhite );

	/* Vehicle silhouette */
	if ( cgs.media.derbyHudVehicleShader ) {
		vec4_t tint;
		Vector4Copy( statusColor, tint );
		tint[3] = 0.65f;
		trap_R_SetColor( tint );
		CG_DrawPic( vehicleX, vehicleY, vehicleW, vehicleH, cgs.media.derbyHudVehicleShader );
		trap_R_SetColor( NULL );
	}

	/* Zone blocks */
	CG_FillRect( zoneFrontX, zoneFrontY, zoneFrontRearW, zoneFrontRearH, neutralColor );
	CG_FillRect( zoneLeftX,  zoneSideY,  zoneSideW,      zoneSideH,      neutralColor );
	CG_FillRect( zoneRightX, zoneSideY,  zoneSideW,      zoneSideH,      neutralColor );
	CG_FillRect( zoneRearX,  zoneRearY,  zoneFrontRearW, zoneFrontRearH, neutralColor );
	if ( cg_derbyVehicleHudRoof.integer )
		CG_FillRect( zoneRoofX, zoneRoofY, zoneFrontRearW, zoneFrontRearH, neutralColor );

	/* Directional hit flash */
	if ( hasDirectionalHit ) {
		switch ( hitSegment ) {
		case 0: CG_FillRect( zoneFrontX, zoneFrontY, zoneFrontRearW, zoneFrontRearH, flashColor ); break;
		case 1: CG_FillRect( zoneLeftX,  zoneSideY,  zoneSideW,      zoneSideH,      flashColor ); break;
		case 2: CG_FillRect( zoneRightX, zoneSideY,  zoneSideW,      zoneSideH,      flashColor ); break;
		case 3: CG_FillRect( zoneRearX,  zoneRearY,  zoneFrontRearW, zoneFrontRearH, flashColor ); break;
		case -2:
			CG_FillRect( zoneFrontX, zoneFrontY, zoneFrontRearW, zoneFrontRearH, flashColor );
			CG_FillRect( zoneLeftX, zoneSideY, zoneSideW, zoneSideH, flashColor );
			CG_FillRect( zoneRightX, zoneSideY, zoneSideW, zoneSideH, flashColor );
			CG_FillRect( zoneRearX, zoneRearY, zoneFrontRearW, zoneFrontRearH, flashColor );
			break;
		default: break;
		}
	}

	CG_DrawIngameString( (int)(x + 8.0f*scale), (int)(y + 5.0f*scale),
	                     "INTEGRITY", UI_SMALLFONT, 0.75f * scale, colorWhite );

	/* Critical pulse border */
	if ( critical ) {
		float pulse = (float)( cg.time % 1000 ) * 0.001f;
		if ( pulse > 0.5f ) pulse = 1.0f - pulse;
		pulse *= 2.0f;
		pulseColor[0]=1.0f; pulseColor[1]=0.12f; pulseColor[2]=0.08f;
		pulseColor[3] = 0.20f + 0.55f * pulse;
		CG_DrawRect( x - 1.0f*scale, y - 1.0f*scale,
		             panelW + 2.0f*scale, panelH + 2.0f*scale,
		             2.0f*scale, pulseColor );
		CG_DrawIngameString( (int)(x + 18.0f*scale), (int)(y + 30.0f*scale),
		                     "!", UI_LEFT, 0.8f * scale, pulseColor );
		if ( cgs.media.derbyHudWarningShader ) {
			trap_R_SetColor( pulseColor );
			CG_DrawPic( x + 90.0f*scale, y + 6.0f*scale,
			            28.0f*scale, 28.0f*scale, cgs.media.derbyHudWarningShader );
			trap_R_SetColor( NULL );
		}
	}

	CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
}


/*
====================
CG_DrawHUD_LCSList
Compact Last Car Standing order list, hidden by the caller during results.
====================
*/
float CG_DrawHUD_LCSList( float x, float y ) {
	const float panelW = 224.0f;
	const float titleH = 20.0f;
	const float labelsH = 14.0f;
	const float rowH = 16.0f;
	const int maxRows = 8;
	int i, rows, panelH, rank, playersRemaining;
	float rowY;
	qboolean isDead, isSpectator, isLocal;
	char name[MAX_QPATH];
	const char *stateText;
	vec4_t panelColor, edgeColor, accentColor, mutedColor;
	vec4_t rowColor, nameColor, stateColor, headerColor;
	clientInfo_t *ci;
	centity_t *cent;
	screenPlacement_e savedHorizontalPlacement, savedVerticalPlacement;

	if ( !cg.snap || cg.numScores <= 0 ) return y;

	rows = cg.numScores < maxRows ? cg.numScores : maxRows;
	panelH = titleH + labelsH + rows * (int)rowH;
	playersRemaining = CG_GetPlayersRemaining( NULL );
	panelColor[0] = 0.008f; panelColor[1] = 0.012f; panelColor[2] = 0.016f; panelColor[3] = 0.42f;
	edgeColor[0] = 0.24f; edgeColor[1] = 0.34f; edgeColor[2] = 0.36f; edgeColor[3] = 0.52f;
	accentColor[0] = 0.72f; accentColor[1] = 1.00f; accentColor[2] = 0.06f; accentColor[3] = 1.00f;
	mutedColor[0] = 0.47f; mutedColor[1] = 0.62f; mutedColor[2] = 0.61f; mutedColor[3] = 1.00f;
	headerColor[0] = 0.008f; headerColor[1] = 0.012f; headerColor[2] = 0.016f; headerColor[3] = 0.72f;

	savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
	savedVerticalPlacement = CG_GetScreenVerticalPlacement();
	CG_SetScreenPlacement( PLACE_RIGHT, PLACE_TOP );
	x = SCREEN_WIDTH - panelW;

	CG_FillRect( x, y, panelW, (float)panelH + 6.0f, panelColor );
	CG_DrawRect( x, y, panelW, (float)panelH + 6.0f, 1.0f, edgeColor );
	CG_FillRect( x, y, panelW, 2.0f, accentColor );
	CG_FillRect( x, y, panelW, titleH, headerColor );
	CG_FillRect( x + 7.0f, y + titleH - 1.0f, panelW - 14.0f, 1.0f, edgeColor );
	CG_DrawIngameString( (int)(x + 8.0f), (int)(y + 5.0f), "LCS ORDER",
	                     UI_SMALLFONT, 0.56f, accentColor );

	rowY = y + titleH;
	CG_DrawIngameString( (int)(x + 8.0f), (int)(rowY + 2.0f), "POS",
	                     UI_SMALLFONT, 0.44f, mutedColor );
	CG_DrawIngameString( (int)(x + 47.0f), (int)(rowY + 2.0f), "DRIVER",
	                     UI_SMALLFONT, 0.44f, mutedColor );
	CG_DrawIngameString( (int)(x + 161.0f), (int)(rowY + 2.0f), "SCORE",
	                     UI_CENTER | UI_SMALLFONT, 0.44f, mutedColor );
	CG_DrawIngameString( (int)(x + panelW - 7.0f), (int)(rowY + 2.0f), "STATE",
	                     UI_RIGHT | UI_SMALLFONT, 0.44f, mutedColor );
	rowY += labelsH;

	for ( i = 0; i < rows; i++ ) {
		if ( cg.scores[i].client < 0 || cg.scores[i].client >= cgs.maxclients ) continue;
		ci = &cgs.clientinfo[cg.scores[i].client];
		cent = &cg_entities[cg.scores[i].client];
		isLocal = cg.scores[i].client == cg.snap->ps.clientNum;
		isSpectator = ci->team == TEAM_SPECTATOR;
		isDead = !isSpectator && ( cent->currentState.eFlags & EF_DEAD );
		rank = cg.scores[i].position > 0 ? cg.scores[i].position : i + 1;

		if ( isSpectator ) {
			stateText = "SPEC";
			Vector4Copy( mutedColor, stateColor );
		} else if ( isDead ) {
			stateText = "OUT";
			stateColor[0] = 1.0f; stateColor[1] = 0.38f; stateColor[2] = 0.20f; stateColor[3] = 0.94f;
		} else if ( playersRemaining == 1 && rank == 1 ) {
			stateText = "LAST";
			Vector4Copy( accentColor, stateColor );
		} else {
			stateText = "ALIVE";
			Vector4Copy( accentColor, stateColor );
		}

		rowColor[0] = 0.018f; rowColor[1] = 0.027f; rowColor[2] = 0.031f;
		rowColor[3] = 0.28f;
		if ( isLocal ) {
			rowColor[0] = 0.060f; rowColor[1] = 0.140f; rowColor[2] = 0.088f; rowColor[3] = 0.45f;
		}
		CG_FillRect( x + 1.0f, rowY, panelW - 2.0f, rowH, rowColor );
		if ( isLocal ) CG_FillRect( x + 1.0f, rowY, 2.0f, rowH, accentColor );
		CG_FillRect( x + 7.0f, rowY + rowH - 1.0f, panelW - 14.0f, 1.0f, edgeColor );

		if ( isDead || isSpectator ) Vector4Copy( mutedColor, nameColor );
		else if ( isLocal ) Vector4Copy( accentColor, nameColor );
		else Vector4Copy( colorWhite, nameColor );

		CG_DrawIngameString( (int)(x + 8.0f), (int)(rowY + 3.0f), va("%02d", rank),
		                     UI_SMALLFONT, 0.52f, rank == 1 ? accentColor : mutedColor );
		if ( ci->modelIcon ) CG_DrawPic( x + 29.0f, rowY + 1.0f, 14.0f, 14.0f, ci->modelIcon );
		Q_strncpyz( name, ci->name, sizeof(name) );
		while ( name[0] && CG_IngameStringWidth( name, UI_SMALLFONT, 0.52f ) > 78.0f ) {
			int nameLength = strlen( name );
			name[--nameLength] = '\0';
			if ( nameLength > 0 && name[nameLength - 1] == '^' ) name[nameLength - 1] = '\0';
		}
		CG_DrawIngameString( (int)(x + 48.0f), (int)(rowY + 3.0f), name,
		                     UI_SMALLFONT, 0.52f, nameColor );
		CG_DrawIngameString( (int)(x + 161.0f), (int)(rowY + 3.0f), va("%d", cg.scores[i].score),
		                     UI_CENTER | UI_SMALLFONT, 0.52f, colorWhite );
		CG_DrawIngameString( (int)(x + panelW - 7.0f), (int)(rowY + 3.0f), stateText,
		                     UI_RIGHT | UI_SMALLFONT, 0.52f, stateColor );
		rowY += rowH;
	}

	CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
	return y + panelH + 6.0f;
}


/*
====================
CG_DrawHUD_DerbyList
Compact modern right-side scoreboard with true current integrity.
====================
*/
void CG_DrawHUD_DerbyList( float x, float y ) {
	const float panelW = 224.0f;
	const float titleH = 20.0f;
	const float labelsH = 14.0f;
	const float rowH = 16.0f;
	const int maxRows = 8;
	int i, rows, panelH, totalTime;
	float rowY, integrity, barW;
	qboolean isDead, isSpectator, isLocal;
	char name[MAX_QPATH];
	char timerText[32];
	const char *matchTime;
	const char *stateText;
	vec4_t panelColor, edgeColor, accentColor, mutedColor;
	vec4_t rowColor, nameColor, stateColor, barBackColor, integrityColor;
	centity_t *cent;
	clientInfo_t *ci;
	screenPlacement_e savedHorizontalPlacement, savedVerticalPlacement;

	if ( !cg.snap || cg.numScores <= 0 ) return;

	cent = &cg_entities[cg.snap->ps.clientNum];
	if ( cent->finishRaceTime ) {
		totalTime = cent->finishRaceTime - cent->startRaceTime;
	} else if ( cent->startRaceTime ) {
		totalTime = cg.time - cent->startRaceTime;
	} else {
		totalTime = 0;
	}
	if ( totalTime < 0 ) totalTime = 0;
	matchTime = getStringForTime( totalTime );
	Com_sprintf( timerText, sizeof(timerText), "T: %s", matchTime );

	rows = cg.numScores < maxRows ? cg.numScores : maxRows;
	panelH = titleH + labelsH + rows * (int)rowH;
	x = SCREEN_WIDTH - panelW;

	panelColor[0] = 0.008f; panelColor[1] = 0.012f; panelColor[2] = 0.016f; panelColor[3] = 0.42f;
	edgeColor[0] = 0.24f; edgeColor[1] = 0.34f; edgeColor[2] = 0.36f; edgeColor[3] = 0.52f;
	accentColor[0] = 0.72f; accentColor[1] = 1.00f; accentColor[2] = 0.06f; accentColor[3] = 1.00f;
	mutedColor[0] = 0.47f; mutedColor[1] = 0.62f; mutedColor[2] = 0.61f; mutedColor[3] = 1.00f;
	barBackColor[0] = 0.008f; barBackColor[1] = 0.012f; barBackColor[2] = 0.016f; barBackColor[3] = 0.72f;

	savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
	savedVerticalPlacement = CG_GetScreenVerticalPlacement();
	CG_SetScreenPlacement( PLACE_RIGHT, PLACE_TOP );

	CG_FillRect( x, y, panelW, (float)panelH + 6.0f, panelColor );
	CG_DrawRect( x, y, panelW, (float)panelH + 6.0f, 1.0f, edgeColor );
	CG_FillRect( x, y, panelW, 2.0f, accentColor );
	CG_FillRect( x, y, panelW, titleH, barBackColor );
	CG_FillRect( x + 7.0f, y + titleH - 1.0f, panelW - 14.0f, 1.0f, edgeColor );
	CG_DrawIngameString( (int)(x + 8.0f), (int)(y + 5.0f), "DERBY ORDER",
	                     UI_SMALLFONT, 0.56f, accentColor );
	if ( cg_hudShowTimes.integer ) {
		CG_DrawIngameString( (int)(x + panelW - 8.0f), (int)(y + 5.0f), timerText,
		                     UI_RIGHT | UI_SMALLFONT, 0.40f, mutedColor );
	}

	rowY = y + titleH;
	CG_DrawIngameString( (int)(x + 8.0f), (int)(rowY + 2.0f), "POS",
	                     UI_SMALLFONT, 0.44f, mutedColor );
	CG_DrawIngameString( (int)(x + 43.0f), (int)(rowY + 2.0f), "DRIVER",
	                     UI_SMALLFONT, 0.44f, mutedColor );
	CG_DrawIngameString( (int)(x + 143.0f), (int)(rowY + 2.0f), "INT",
	                     UI_SMALLFONT, 0.44f, mutedColor );
	CG_DrawIngameString( (int)(x + panelW - 7.0f), (int)(rowY + 2.0f), "STATE",
	                     UI_RIGHT | UI_SMALLFONT, 0.44f, mutedColor );
	rowY += labelsH;

	for ( i = 0; i < rows; i++ ) {
		if ( cg.scores[i].client < 0 || cg.scores[i].client >= cgs.maxclients ) continue;

		ci = &cgs.clientinfo[cg.scores[i].client];
		cent = &cg_entities[cg.scores[i].client];
		isLocal = cg.scores[i].client == cg.snap->ps.clientNum;
		isSpectator = ci->team == TEAM_SPECTATOR;
		isDead = !isSpectator && ( cg.scores[i].integrity == 0 ||
		                             ( cent->currentState.eFlags & EF_DEAD ) );

		if ( isSpectator ) {
			stateText = "SPEC";
			Vector4Copy( mutedColor, stateColor );
		} else if ( isDead ) {
			stateText = "OUT";
			stateColor[0] = 1.0f; stateColor[1] = 0.38f; stateColor[2] = 0.20f; stateColor[3] = 0.94f;
		} else {
			stateText = "ALIVE";
			Vector4Copy( accentColor, stateColor );
		}

		rowColor[0] = 0.018f; rowColor[1] = 0.027f; rowColor[2] = 0.031f;
		rowColor[3] = 0.28f;
		if ( isLocal ) {
			rowColor[0] = 0.060f; rowColor[1] = 0.140f; rowColor[2] = 0.088f; rowColor[3] = 0.45f;
		}
		CG_FillRect( x + 1.0f, rowY, panelW - 2.0f, rowH, rowColor );
		if ( isLocal ) CG_FillRect( x + 1.0f, rowY, 2.0f, rowH, accentColor );
		CG_FillRect( x + 7.0f, rowY + rowH - 1.0f, panelW - 14.0f, 1.0f, edgeColor );

		if ( isDead || isSpectator ) {
			Vector4Copy( mutedColor, nameColor );
		} else if ( isLocal ) {
			Vector4Copy( accentColor, nameColor );
		} else {
			Vector4Copy( colorWhite, nameColor );
		}

		CG_DrawIngameString( (int)(x + 8.0f), (int)(rowY + 3.0f), va("%02d", i + 1),
		                     UI_SMALLFONT, 0.52f, ( i == 0 ) ? accentColor : mutedColor );
		if ( ci->modelIcon ) {
			CG_DrawPic( x + 29.0f, rowY + 1.0f, 14.0f, 14.0f, ci->modelIcon );
		}

		Q_strncpyz( name, ci->name, sizeof(name) );
		while ( name[0] && CG_IngameStringWidth( name, UI_SMALLFONT, 0.52f ) > 82.0f ) {
			int nameLength = strlen( name );
			name[--nameLength] = '\0';
			if ( nameLength > 0 && name[nameLength - 1] == '^' ) name[nameLength - 1] = '\0';
		}
		CG_DrawIngameString( (int)(x + 48.0f), (int)(rowY + 3.0f), name,
		                     UI_SMALLFONT, 0.52f, nameColor );

		integrity = ( isSpectator || cg.scores[i].integrity < 0 )
			? 0.0f : cg.scores[i].integrity / 100.0f;
		if ( integrity < 0.0f ) integrity = 0.0f;
		if ( integrity > 1.0f ) integrity = 1.0f;
		barW = 38.0f;
		CG_FillRect( x + 142.0f, rowY + 5.0f, barW, 6.0f, barBackColor );
		if ( integrity > 0.0f ) {
			if ( integrity > 0.50f ) {
				integrityColor[0] = 0.22f; integrityColor[1] = 0.82f; integrityColor[2] = 0.36f;
			} else if ( integrity > 0.25f ) {
				integrityColor[0] = 1.00f; integrityColor[1] = 0.72f; integrityColor[2] = 0.12f;
			} else {
				integrityColor[0] = 1.00f; integrityColor[1] = 0.25f; integrityColor[2] = 0.14f;
			}
			integrityColor[3] = 0.96f;
			CG_FillRect( x + 142.0f, rowY + 5.0f, barW * integrity, 6.0f, integrityColor );
		}
		CG_DrawIngameString( (int)(x + panelW - 7.0f), (int)(rowY + 3.0f), stateText,
		                     UI_RIGHT | UI_SMALLFONT, 0.52f, stateColor );
		rowY += rowH;
	}

	CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
}
