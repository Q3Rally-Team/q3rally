/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.
===========================================================================
*/

/*
===========================================================================
  cg_hud_vehicle.c

  Vehicle instrument HUD elements:
    - CG_AddObjectsToScene    : helper used by mirror and minimap
    - CG_DrawRearviewMirror   : rear-view 3D render + overlay shader
    - CG_DrawMMap             : top-down minimap render
    - CG_DrawFuelGauge        : fuel bar + warning text
    - CG_DrawSpeed            : speedometer (digital bar-graph or analog dial)
===========================================================================
*/

#include "cg_local.h"
#include "cg_hud_elements.h"


/* -----------------------------------------------------------------------
   CG_AddObjectsToScene
   Helper shared by rearview mirror and minimap renders.
   ----------------------------------------------------------------------- */
void CG_AddObjectsToScene( int renderLevel ) {
	int i;

	if ( renderLevel & RL_MARKS )
		CG_AddMarks();

	if ( renderLevel & RL_SMOKE )
		CG_AddLocalEntities();

	if ( renderLevel & RL_PLAYERS || renderLevel & RL_OBJECTS ) {
		for ( i = 0; i < cg.snap->numEntities; i++ ) {
			if ( !(renderLevel & RL_OBJECTS) ) {
				if ( cg.snap->entities[i].eType != ET_PLAYER ) continue;
			}
			if ( !(renderLevel & RL_PLAYERS) ) {
				if ( cg.snap->entities[i].eType == ET_PLAYER  ) continue;
			}
			CG_AddCEntity( &cg_entities[ cg.snap->entities[i].number ] );
		}
	}
}


/* -----------------------------------------------------------------------
   CG_DrawRearviewMirror
   ----------------------------------------------------------------------- */
void CG_DrawRearviewMirror( float x, float y, float w, float h ) {
	float mx, my, mw, mh;
	int   tmp;

	if ( !cg_drawRearView.integer )
		return;
	if ( cg.snap->ps.pm_type == PM_INTERMISSION )
		return;
	if ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_SPECTATOR )
		return;

	mx = x - 8;
	my = y - 7;
	mw = w * 1.0534f;
	mh = h * 1.2f;

	CG_AdjustFrom640( &x, &y, &w, &h );

	cg.mirrorRefdef.x      = x;
	cg.mirrorRefdef.y      = y;
	cg.mirrorRefdef.width  = w;
	cg.mirrorRefdef.height = h;
	cg.mirrorRefdef.fov_x  = 70;
	tmp = cg.mirrorRefdef.width / tan( cg.mirrorRefdef.fov_x / 360 * M_PI );
	cg.mirrorRefdef.fov_y  = atan2( cg.mirrorRefdef.height, tmp ) * 360.0f / M_PI;
	cg.mirrorRefdef.time   = cg.time;
	cg.mirrorRefdef.rdflags= 0;

	CG_AddObjectsToScene( cg_rearViewRenderLevel.integer );
	trap_R_RenderScene( &cg.mirrorRefdef );
	CG_DrawPic( mx, my, mw, mh, cgs.media.rearviewMirrorShader );
}


/* -----------------------------------------------------------------------
   CG_DrawMMap
   Top-down minimap overlay.
   ----------------------------------------------------------------------- */
void CG_DrawMMap( float x, float y, float w, float h ) {
	float overlay_x, overlay_y, overlay_w, overlay_h;
	float tmp;

	if ( !cg_drawMMap.integer )
		return;
	if ( cg.snap->ps.pm_type == PM_INTERMISSION )
		return;
	if ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_SPECTATOR )
		return;

	overlay_x = x;
	overlay_y = y;
	overlay_w = w * cg_mmap_size.value;
	overlay_h = h * cg_mmap_size.value;

	CG_AdjustFrom640( &x, &y, &w, &h );

	cg.mmapRefdef.x       = x;
	cg.mmapRefdef.y       = y;
	cg.mmapRefdef.width   = w * cg_mmap_size.value;
	cg.mmapRefdef.height  = h * cg_mmap_size.value;
	cg.mmapRefdef.fov_x   = cg_mmap_fov.value;
	tmp = cg.mmapRefdef.width / tan( cg_mmap_fov.value / 360.0f * M_PI );
	cg.mmapRefdef.fov_y   = atan2( cg.mmapRefdef.height, tmp ) * 360.0f / M_PI;
	cg.mmapRefdef.time    = cg.time;
	cg.mmapRefdef.rdflags = 0;

	CG_AddObjectsToScene( cg_mmap_renderLevel.integer );

	if ( cg_mmap_renderLevel.integer & RL_PLAYERS )
		CG_AddCEntity( &cg_entities[cg.snap->ps.clientNum] );

	trap_R_RenderScene( &cg.mmapRefdef );
	CG_DrawPic( overlay_x, overlay_y, overlay_w, overlay_h, cgs.media.MMapShader );
}


/* -----------------------------------------------------------------------
   CG_DrawFuelGauge
   Horizontal fill bar with blinking icon and "Fuel Critical" warning.
   ----------------------------------------------------------------------- */
void CG_DrawFuelGauge( float x, float y, float w, float h ) {
	static qhandle_t icon   = 0;
	static qboolean  warned = qfalse;
	int    fuel;
	float  frac, warnFrac, size;
	vec4_t backColor = { 0.0f, 0.0f, 0.0f, 0.25f };
	vec4_t fuelColor = { 0.0f, 0.8f, 0.0f, 0.80f };
	vec4_t warnColor = { 1.0f, 0.0f, 0.0f, 0.80f };

	if ( !cg.snap ) return;
	if ( !icon ) icon = trap_R_RegisterShaderNoMip( "icons/fuelcan2" );

	fuel = cg.snap->ps.stats[STAT_FUEL];
	if ( fuel < 0   ) fuel = 0;
	if ( fuel > 100 ) fuel = 100;

	frac     = fuel / 100.0f;
	warnFrac = cg_fuelWarningLevel.value / 100.0f;
	size     = h * 2.0f;

	CG_DrawRect( x, y, w, h, 1, colorWhite );
	CG_FillRect( x + 1, y + 1, w - 2, h - 2, backColor );

	if ( frac < warnFrac ) {
		CG_FillRect( x + 1, y + 1, (w - 2) * frac, h - 2, warnColor );

		if ( !warned ) {
			trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
			warned = qtrue;
		}

		if ( ( cg.time >> 8 ) & 1 )
			CG_DrawPic( x - size - 4, y + ( h - size ) * 0.5f, size, size, icon );

		{
			int   textWidth = BIGCHAR_WIDTH * CG_DrawStrlen( "Fuel Critical - Refuel Now!" );
			float textX     = ( SCREEN_WIDTH - textWidth ) / 2;
			CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );
			CG_DrawStringExt( textX, SCREEN_HEIGHT * 0.30f,
			                  "Fuel Critical - Refuel Now!", warnColor,
			                  qfalse, qtrue, BIGCHAR_WIDTH, (int)(BIGCHAR_WIDTH * 1.5f), 0 );
			CG_PopScreenPlacement();
		}
	} else {
		warned = qfalse;
		CG_FillRect( x + 1, y + 1, (w - 2) * frac, h - 2, fuelColor );
		CG_DrawPic( x - size - 4, y + ( h - size ) * 0.5f, size, size, icon );
	}
}


/* -----------------------------------------------------------------------
   CG_DrawSpeed
   Speedometer: digital bar-graph (cg_speedometerMode 1) or
                analog dial with needle (cg_speedometerMode 0).
   Both modes also render the fuel gauge and RPM segments.
   ----------------------------------------------------------------------- */
float CG_DrawSpeed( float y ) {
	playerState_t *ps;
	int    vel_speed;
	vec3_t forward, origin, angles, mins, maxs;
	int    x, yorg;
	float  x2, y2, w, h;
	refdef_t   refdef;
	refEntity_t ent;

	x    = 630;
	yorg = y;

	ps = &cg.predictedPlayerState;
	AngleVectors( ps->viewangles, forward, NULL, NULL );
	vel_speed = (int)fabs( Q3VelocityToRL( DotProduct( ps->velocity, forward ) ) );

	/* ---- Digital bar-graph mode ---------------------------------------- */
	if ( cg_speedometerMode.integer ) {
		char  speedStr[32], gearStr[16], gearLabel[4];
		int   maxLen, len, rpm, segments;
		float bgClr[4] = { 0.0f, 0.0f, 0.0f, 0.25f };
		float segmentWidth;
		const float segmentHeight = 8.0f;
		const float segmentGap    = 4.0f;
		const float gaugeHeight   = 12.0f;
		const float rpmIconSize   = gaugeHeight * 2;
		const float rpmIconOffset = rpmIconSize + 4;
		const float rpmIconNudge  = 3.0f;
		float iconOffset  = gaugeHeight * 2 + 4;
		float blockWidth, blockHeight, barWidth;
		int   i, speedWidth, gearWidth;

		Com_sprintf( speedStr, sizeof(speedStr), "%i %s", vel_speed,
		             cg_metricUnits.integer ? "KPH" : "MPH" );

		if      ( cg.predictedPlayerState.stats[STAT_GEAR] == -1 )
			Q_strncpyz( gearLabel, "R", sizeof(gearLabel) );
		else if ( cg.predictedPlayerState.stats[STAT_GEAR] ==  0 )
			Q_strncpyz( gearLabel, "N", sizeof(gearLabel) );
		else
			Com_sprintf( gearLabel, sizeof(gearLabel), "%i", cg.predictedPlayerState.stats[STAT_GEAR] );

		Com_sprintf( gearStr, sizeof(gearStr), "GEAR %s", gearLabel );

		maxLen = CG_DrawStrlen( speedStr );
		len    = CG_DrawStrlen( gearStr  );
		if ( len > maxLen ) maxLen = len;

		rpm      = cg.predictedPlayerState.stats[STAT_RPM];
		segments = (CP_RPM_MAX + 999) / 1000;

		barWidth    = maxLen * GIANTCHAR_WIDTH - 15;
		blockWidth  = max( iconOffset + barWidth, rpmIconOffset + maxLen * GIANTCHAR_WIDTH );
		blockHeight = segmentHeight + 2 * GIANTCHAR_HEIGHT + gaugeHeight;

		x  = 640 - (int)blockWidth - 8;
		y -= blockHeight;

		{
			float rectX = x - 4, rectY = y - 4;
			float rectW = blockWidth + 8, rectH = blockHeight + 8;
			CG_FillRect( rectX, rectY, rectW, rectH, bgClr );
		}

		segmentWidth = (maxLen * GIANTCHAR_WIDTH - (segments - 1) * segmentGap) / segments;
		{
			static qhandle_t rpmIcon;
			if ( !rpmIcon ) rpmIcon = trap_R_RegisterShaderNoMip( "icons/rpm" );
			CG_DrawPic( x - rpmIconNudge, y, rpmIconSize, rpmIconSize, rpmIcon );
		}
		for ( i = 0; i < segments; i++ ) {
			float segX = x + rpmIconOffset + i * (segmentWidth + segmentGap);
			if ( rpm >= (i + 1) * 1000 )
				CG_FillRect( segX, y, segmentWidth, segmentHeight, colorWhite );
			else
				CG_DrawRect( segX, y, segmentWidth, segmentHeight, 1, colorWhite );
		}

		speedWidth = CG_DrawStrlen( speedStr ) * GIANTCHAR_WIDTH;
		gearWidth  = CG_DrawStrlen( gearStr  ) * GIANTCHAR_WIDTH;
		y += segmentHeight;
		CG_DrawStringExt( x + (int)blockWidth - speedWidth, (int)y, speedStr, colorWhite, qtrue, qfalse, GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );
		y += GIANTCHAR_HEIGHT;
		CG_DrawStringExt( x + (int)blockWidth - gearWidth,  (int)y, gearStr,  colorWhite, qtrue, qfalse, GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );
		y += GIANTCHAR_HEIGHT;
		CG_DrawFuelGauge( x + iconOffset, y, barWidth, gaugeHeight );

		y = yorg - 44 - blockHeight;
		return y;
	}

	/* ---- Analog dial mode ---------------------------------------------- */
	{
		const float gaugeSize   = 96.0f;
		const float fuelWidth   = 90.0f;
		const float fuelHeight  = 8.0f;
		const float gaugeSpacing= 4.0f;
		const float rpmHeight   = 8.0f;
		const float rpmSpacing  = 4.0f;
		float blockWidth  = gaugeSize;
		float blockHeight = gaugeSize + gaugeSpacing + fuelHeight + rpmSpacing + rpmHeight;
		float left, top;
		int   speedWidth;
		float speedX, speedY;
		float centerX, centerY;
		int   i;

		left = 640 - blockWidth - 8;
		top  = y - blockHeight;

		CG_DrawPic( left, top, gaugeSize, gaugeSize,
		            cg_metricUnits.integer ? cgs.media.gaugeMetric : cgs.media.gaugeImperial );

		speedWidth = CG_DrawStrlen( va("%i", vel_speed) ) * SMALLCHAR_WIDTH;
		speedX = left + (gaugeSize - speedWidth) * 0.5f;
		speedY = top  + gaugeSize - 35;
		CG_DrawSmallStringColor( (int)speedX, (int)speedY, va("%i", vel_speed), colorWhite );

		x2 = left;  y2 = top;  w = h = gaugeSize;
		CG_AdjustFrom640( &x2, &y2, &w, &h );

		memset( &refdef, 0, sizeof(refdef) );
		memset( &ent,    0, sizeof(ent   ) );

		ent.hModel        = trap_R_RegisterModel( "gfx/hud/needle.md3" );
		ent.customShader  = trap_R_RegisterShader( "gfx/hud/needle01" );
		ent.renderfx      = RF_NOSHADOW;

		trap_R_ModelBounds( ent.hModel, mins, maxs );
		origin[2] = 0;
		origin[1] = 0.5f * ( mins[1] + maxs[1] );
		origin[0] = ( maxs[2] - mins[2] ) / 0.268f;

		VectorClear( angles );
		angles[YAW]   = -90;
		angles[PITCH] = -150.0f + (300.0f * vel_speed / 200.0f);
		AnglesToAxis( angles, ent.axis );
		VectorCopy( origin, ent.origin );

		refdef.rdflags = RDF_NOWORLDMODEL;
		AxisClear( refdef.viewaxis );
		refdef.fov_x   = 30;  refdef.fov_y   = 30;
		refdef.x       = x2;  refdef.y       = y2;
		refdef.width   = w;   refdef.height  = h;
		refdef.time    = cg.time;

		trap_R_ClearScene();
		trap_R_AddRefEntityToScene( &ent );
		trap_R_RenderScene( &refdef );

		centerX = left + gaugeSize * 0.5f - 12.0f;
		centerY = top  + gaugeSize * 0.5f - 12.0f;
		CG_DrawPic( centerX, centerY, 24, 24, trap_R_RegisterShaderNoMip("gfx/hud/center01") );

		if      ( cg.predictedPlayerState.stats[STAT_GEAR] == -1 )
			CG_DrawSmallStringColor( (int)(centerX + 10), (int)(centerY + 4), "R", colorWhite );
		else if ( cg.predictedPlayerState.stats[STAT_GEAR] ==  0 )
			CG_DrawSmallStringColor( (int)(centerX + 10), (int)(centerY + 4), "N", colorWhite );
		else
			CG_DrawSmallStringColor( (int)(centerX + 10), (int)(centerY + 4),
			    va("%i", cg.predictedPlayerState.stats[STAT_GEAR]), colorWhite );

		CG_DrawFuelGauge( left + (blockWidth - fuelWidth) * 0.5f,
		                  top + gaugeSize + gaugeSpacing,
		                  fuelWidth, fuelHeight );

		{
			int   rpm      = cg.predictedPlayerState.stats[STAT_RPM];
			int   segments = (CP_RPM_MAX + 999) / 1000;
			float segGap   = 2.0f;
			float segWidth = (fuelWidth - (segments - 1) * segGap) / segments;
			float segX     = left + (blockWidth - fuelWidth) * 0.5f;
			float segY     = top + gaugeSize + gaugeSpacing + fuelHeight + rpmSpacing;
			float rpmIconSize, rpmIconX, rpmIconY;
			const float rpmIconNudge = 3.0f;
			static qhandle_t rpmIcon;

			if ( !rpmIcon ) rpmIcon = trap_R_RegisterShaderNoMip( "icons/rpm" );

			rpmIconSize = rpmHeight * (4.0f / 3.0f);
			rpmIconX    = segX - rpmIconSize - 4 - rpmIconNudge;
			rpmIconY    = segY + (rpmHeight - rpmIconSize) * 0.5f;
			CG_DrawPic( rpmIconX, rpmIconY, rpmIconSize, rpmIconSize, rpmIcon );

			for ( i = 0; i < segments; i++ ) {
				float xPos = segX + i * (segWidth + segGap);
				if ( rpm >= (i + 1) * 1000 )
					CG_FillRect( xPos, segY, segWidth, rpmHeight, colorWhite );
				else
					CG_DrawRect( xPos, segY, segWidth, rpmHeight, 1, colorWhite );
			}
		}

		y = yorg - 44 - blockHeight;
		return y;
	}
}
