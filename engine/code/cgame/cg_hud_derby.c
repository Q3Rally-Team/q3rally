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
    - CG_DrawHUD_DerbyList        : compact scoreboard (P / DD / DT)
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
	const int   lastHitFlashMs = 420;
	float   healthFrac;
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
	int     hitSegment;
	qboolean hasDirectionalHit, critical;

	if ( cgs.gametype != GT_DERBY ) return;

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

	/* Detect new directional hit */
	hitSegment       = -1;
	hasDirectionalHit= qfalse;
	{
		int curDmg = cg.snap->ps.stats[STAT_DAMAGE_TAKEN];
		if ( curDmg != cg.derbyLastDamageTaken ) {
			vec3_t forward, right, angles;
			float  fwdDot, rightDot;

			VectorSet( angles, 0, cg.snap->ps.viewangles[YAW], 0 );
			AngleVectors( angles, forward, right, NULL );
			fwdDot   = DotProduct( cg.snap->ps.velocity, forward );
			rightDot = DotProduct( cg.snap->ps.velocity, right );

			cg.derbyHitFxTime        = cg.time;
			cg.derbyHitFxDamage      = curDmg - cg.derbyLastDamageTaken;
			cg.derbyLastDamageTaken  = curDmg;

			if ( fabs(fwdDot) >= fabs(rightDot) )
				cg.derbyHitFxDir = ( fwdDot >= 0.0f ) ? 0 : 3;   /* 0=front 3=rear */
			else
				cg.derbyHitFxDir = ( rightDot >= 0.0f ) ? 2 : 1;  /* 2=right 1=left */
		}
	}
	if ( cg.derbyHitFxTime > 0 && cg.time - cg.derbyHitFxTime < lastHitFlashMs ) {
		hasDirectionalHit = qtrue;
		hitSegment        = cg.derbyHitFxDir;
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
		default: break;
		}
	}

	CG_DrawTinyStringColor( (int)(x + 8.0f*scale), (int)(y + 6.0f*scale), "INTEGRITY", colorWhite );

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
		CG_DrawBigStringColor( (int)(x + 18.0f*scale), (int)(y + 30.0f*scale), "!", pulseColor );
		if ( cgs.media.derbyHudWarningShader ) {
			trap_R_SetColor( pulseColor );
			CG_DrawPic( x + 90.0f*scale, y + 6.0f*scale,
			            28.0f*scale, 28.0f*scale, cgs.media.derbyHudWarningShader );
			trap_R_SetColor( NULL );
		}
	}
}


/*
====================
CG_DrawHUD_DerbyList
Compact right-side scoreboard: rank / name / damage dealt / damage taken.
====================
*/
void CG_DrawHUD_DerbyList( float x, float y ) {
	int       i;
	vec4_t    color;
	centity_t *cent;

	x = 729;   /* anchor to right edge */

	/* Header row */
	CG_FillRect( x, y, 120, 16, bgColor );
	CG_DrawTinyStringColor( x + 0,  y + 4, "PLAYER:",   colorWhite );
	CG_DrawTinyStringColor( x + 60,  y + 4, "DD:",  colorWhite );
	CG_DrawTinyStringColor( x + 90, y + 4, "DT:",  colorWhite );
	y += 16;

	/* Top-8 player rows */
	for ( i = 0; i < 8; i++ ) {
		if ( cg.scores[i].scoreFlags < 0 ) continue;

		cent = &cg_entities[cg.scores[i].client];
		if ( !cent ) continue;

		CG_FillRect( x, y, 120, 16, bgColor );

		Vector4Copy( colorWhite, color );
		if ( cg.scores[i].client == cg.snap->ps.clientNum ) {
			if ( cg.snap->ps.stats[STAT_HEALTH] <= 0
			     || cgs.clientinfo[cg.scores[i].client].team == TEAM_SPECTATOR )
				Vector4Copy( colorMdGrey, color );
		} else if ( cent->currentState.eFlags & EF_DEAD
		            || cgs.clientinfo[cg.scores[i].client].team == TEAM_SPECTATOR ) {
			Vector4Copy( colorMdGrey, color );
		}

		CG_DrawTinyStringColor( x + 2,   y + 4, va("%i",  i + 1),                              color );
		CG_DrawTinyStringColor( x + 16,  y + 4, cgs.clientinfo[cg.scores[i].client].name,       color );
		CG_DrawTinyStringColor( x + 65,  y + 4, va("%i",  cg.scores[i].damageDealt),            color );
		CG_DrawTinyStringColor( x + 95, y + 4, va("%i",  cg.scores[i].damageTaken),            color );

		y += 16;
	}
}
