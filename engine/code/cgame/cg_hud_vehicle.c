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

static const vec4_t vehicleHudPanel  = { 0.018f, 0.025f, 0.030f, 0.78f };
static const vec4_t vehicleHudLine   = { 0.250f, 0.330f, 0.320f, 0.58f };
static const vec4_t vehicleHudAccent = { 0.720f, 1.000f, 0.060f, 1.00f };
static const vec4_t vehicleHudLabel  = { 0.57f, 0.70f, 0.74f, 0.90f };
static const vec4_t vehicleHudFrame  = { 0.30f, 0.43f, 0.49f, 0.82f };

static void CG_DrawVehicleHudFrame( float x, float y, float w, float h,
                                    const char *title ) {
	CG_FillRect( x, y, w, h, vehicleHudPanel );
	CG_DrawRect( x, y, w, h, 1.0f, vehicleHudLine );
	CG_FillRect( x, y, w, 2.0f, vehicleHudAccent );
	CG_FillRect( x, y, 3.0f, h, vehicleHudAccent );
	CG_DrawIngameString( (int)( x + 9.0f ), (int)( y + 4.0f ), title,
	                     UI_SMALLFONT | UI_DROPSHADOW, 0.75f,
	                     vehicleHudAccent );
}

/* -----------------------------------------------------------------------
   CG_AddKOTHHillIndicatorToScene
   Adds a pulse/ring style world indicator for the KOTH hill. Used in
   main world rendering and minimap rendering.
   ----------------------------------------------------------------------- */
void CG_AddKOTHHillIndicatorToScene( qboolean minimapPass ) {
	refEntity_t	marker;
	trace_t		tr;
	vec3_t		groundPos;
	vec3_t		beamStart;
	vec3_t		traceEnd;
	float		pulse;
	float		inversePulse;
	float		ringScale;
	float		beamHalfWidth;
	float		ringAlphaBase;
	float		ringAlphaPulse;
	float		beamAlphaBase;
	float		beamAlphaPulse;
	qhandle_t	hillMarkerShader;
	byte		tintR, tintG, tintB;

	if ( cgs.gametype != GT_KOTH ) {
		return;
	}

	if ( !cgs.kothHillOriginValid ) {
		return;
	}

	/* ---- colour by owner / state ---- */
	hillMarkerShader = cgs.media.kothHillMarkerNeutralShader;
	if ( cgs.kothContested ) {
		tintR = 255; tintG = 220; tintB = 64;
	} else if ( cgs.kothOwner == TEAM_RED ) {
		hillMarkerShader = cgs.media.kothHillMarkerRedShader;
		tintR = 255; tintG = 0;   tintB = 0;
	} else if ( cgs.kothOwner == TEAM_BLUE ) {
		hillMarkerShader = cgs.media.kothHillMarkerBlueShader;
		tintR = 0;   tintG = 12;  tintB = 255;
	} else {
		tintR = 240; tintG = 240; tintB = 240;
	}

	/* base pulse: 0..1, period ~7 s */
	pulse        = 0.5f + 0.5f * sin( (float)cg.time * 0.009f );
	inversePulse = 1.0f - pulse;

	ringScale = cg_kothRingScale.value;
	if ( ringScale <= 0.0f ) {
		ringScale = 1.0f;
	}

	beamHalfWidth = cg_kothBeamHalfWidth.value;
	if ( beamHalfWidth < 2.0f ) {
		beamHalfWidth = 2.0f;
	}

	ringAlphaBase = cg_kothRingAlphaBase.value;
	if ( ringAlphaBase < 0.0f ) {
		ringAlphaBase = 0.0f;
	} else if ( ringAlphaBase > 255.0f ) {
		ringAlphaBase = 255.0f;
	}
	ringAlphaPulse = cg_kothRingAlphaPulse.value;
	if ( ringAlphaPulse < 0.0f ) {
		ringAlphaPulse = 0.0f;
	}

	beamAlphaBase = cg_kothBeamAlphaBase.value;
	if ( beamAlphaBase < 0.0f ) {
		beamAlphaBase = 0.0f;
	} else if ( beamAlphaBase > 255.0f ) {
		beamAlphaBase = 255.0f;
	}
	beamAlphaPulse = cg_kothBeamAlphaPulse.value;
	if ( beamAlphaPulse < 0.0f ) {
		beamAlphaPulse = 0.0f;
	}

	/* ---- find ground level below hill origin ----
	   Start well above kothHillOrigin so the trace passes through
	   the hill brush (CONTENTS_TRIGGER, not solid) to the real floor.
	   MASK_PLAYERSOLID skips trigger volumes. */
	VectorCopy( cgs.kothHillOrigin, beamStart );
	beamStart[2] += 256.0f;
	VectorCopy( beamStart, traceEnd );
	traceEnd[2] -= 8192.0f;
	CG_Trace( &tr, beamStart, NULL, NULL, traceEnd, ENTITYNUM_NONE, MASK_PLAYERSOLID );
	if ( tr.fraction < 1.0f ) {
		VectorCopy( tr.endpos, groundPos );
	} else {
		VectorCopy( cgs.kothHillOrigin, groundPos );
	}

	/* ============================================================
	   1) GROUND RING  (flat horizontal poly ring in world space)
	      Built as a triangle fan: SEGMENTS quads, each quad = 2
	      tris.  Inner radius = outerRadius * INNER_FRAC gives the
	      ring band width.  A slow angular offset animates rotation.
	   ============================================================ */
	{
		#define KOTH_RING_SEGS  24
		#define KOTH_INNER_FRAC 0.72f
		polyVert_t  rv[4];
		float       outerR, innerR;
		float       rotOff;
		float       ringZ;
		byte        ringAlpha;
		int         seg;
		float       a0, a1, c0, s0, c1, s1;
		float       ox0, oy0, ox1, oy1;
		float       ix0, iy0, ix1, iy1;

		outerR    = minimapPass
			? ( cgs.kothHillRadius * ( 1.35f * ringScale ) + 10.0f * pulse )
			: ( cgs.kothHillRadius * ringScale + 8.0f * pulse );
		innerR    = outerR * KOTH_INNER_FRAC;
		rotOff    = (float)cg.time * 0.0008f;   /* slow rotation */
		ringZ     = groundPos[2] + 2.0f;         /* just above ground */
		{
			float alpha = ringAlphaBase + ringAlphaPulse * pulse;
			if ( alpha < 0.0f ) {
				alpha = 0.0f;
			} else if ( alpha > 255.0f ) {
				alpha = 255.0f;
			}
			ringAlpha = (byte)alpha;
		}

		for ( seg = 0; seg < KOTH_RING_SEGS; seg++ ) {
			a0 = rotOff + (float)seg       * (2.0f * M_PI / KOTH_RING_SEGS);
			a1 = rotOff + (float)(seg + 1) * (2.0f * M_PI / KOTH_RING_SEGS);
			c0 = cos(a0); s0 = sin(a0);
			c1 = cos(a1); s1 = sin(a1);

			/* outer vertices */
			ox0 = groundPos[0] + outerR * c0;
			oy0 = groundPos[1] + outerR * s0;
			ox1 = groundPos[0] + outerR * c1;
			oy1 = groundPos[1] + outerR * s1;
			/* inner vertices */
			ix0 = groundPos[0] + innerR * c0;
			iy0 = groundPos[1] + innerR * s0;
			ix1 = groundPos[0] + innerR * c1;
			iy1 = groundPos[1] + innerR * s1;

			/* quad as two tris wound CCW (outer0, inner0, inner1, outer1) */
			rv[0].xyz[0] = ox0; rv[0].xyz[1] = oy0; rv[0].xyz[2] = ringZ;
			rv[0].st[0] = 0.0f; rv[0].st[1] = 0.0f;
			rv[0].modulate[0] = tintR; rv[0].modulate[1] = tintG;
			rv[0].modulate[2] = tintB; rv[0].modulate[3] = ringAlpha;

			rv[1].xyz[0] = ix0; rv[1].xyz[1] = iy0; rv[1].xyz[2] = ringZ;
			rv[1].st[0] = 0.5f; rv[1].st[1] = 0.0f;
			rv[1].modulate[0] = tintR; rv[1].modulate[1] = tintG;
			rv[1].modulate[2] = tintB; rv[1].modulate[3] = ringAlpha;

			rv[2].xyz[0] = ix1; rv[2].xyz[1] = iy1; rv[2].xyz[2] = ringZ;
			rv[2].st[0] = 0.5f; rv[2].st[1] = 1.0f;
			rv[2].modulate[0] = tintR; rv[2].modulate[1] = tintG;
			rv[2].modulate[2] = tintB; rv[2].modulate[3] = ringAlpha;

			rv[3].xyz[0] = ox1; rv[3].xyz[1] = oy1; rv[3].xyz[2] = ringZ;
			rv[3].st[0] = 0.0f; rv[3].st[1] = 1.0f;
			rv[3].modulate[0] = tintR; rv[3].modulate[1] = tintG;
			rv[3].modulate[2] = tintB; rv[3].modulate[3] = ringAlpha;

			trap_R_AddPolyToScene( cgs.media.kothRingShader, 4, rv );
		}
		#undef KOTH_RING_SEGS
		#undef KOTH_INNER_FRAC
	}

	/* ============================================================
	   2) UPWARD BEAM  (two crossed vertical quads = X-shape)
	      Visible from all angles without billboard math.
	      Each quad spans groundPos.Z -> kothHillOrigin.Z.
	      Half-width BEAM_W controls thickness.
	      UV runs 0..1 bottom-to-top so tcMod scroll animates upward.
	   ============================================================ */
	{
		polyVert_t  bv[4];
		float       bz0, bz1;
		byte        beamAlpha;
		float       cx, cy;

		bz0       = groundPos[2]         + 2.0f;
		bz1       = cgs.kothHillOrigin[2];
		{
			float alpha = beamAlphaBase + beamAlphaPulse * pulse;
			if ( alpha < 0.0f ) {
				alpha = 0.0f;
			} else if ( alpha > 255.0f ) {
				alpha = 255.0f;
			}
			beamAlpha = (byte)alpha;
		}
		cx        = groundPos[0];
		cy        = groundPos[1];

		/* --- quad 1: aligned along X axis --- */
		bv[0].xyz[0]=cx-beamHalfWidth; bv[0].xyz[1]=cy; bv[0].xyz[2]=bz0;
		bv[0].st[0]=0.0f; bv[0].st[1]=1.0f;
		bv[0].modulate[0]=tintR; bv[0].modulate[1]=tintG;
		bv[0].modulate[2]=tintB; bv[0].modulate[3]=beamAlpha;

		bv[1].xyz[0]=cx+beamHalfWidth; bv[1].xyz[1]=cy; bv[1].xyz[2]=bz0;
		bv[1].st[0]=1.0f; bv[1].st[1]=1.0f;
		bv[1].modulate[0]=tintR; bv[1].modulate[1]=tintG;
		bv[1].modulate[2]=tintB; bv[1].modulate[3]=beamAlpha;

		bv[2].xyz[0]=cx+beamHalfWidth; bv[2].xyz[1]=cy; bv[2].xyz[2]=bz1;
		bv[2].st[0]=1.0f; bv[2].st[1]=0.0f;
		bv[2].modulate[0]=tintR; bv[2].modulate[1]=tintG;
		bv[2].modulate[2]=tintB; bv[2].modulate[3]=beamAlpha;

		bv[3].xyz[0]=cx-beamHalfWidth; bv[3].xyz[1]=cy; bv[3].xyz[2]=bz1;
		bv[3].st[0]=0.0f; bv[3].st[1]=0.0f;
		bv[3].modulate[0]=tintR; bv[3].modulate[1]=tintG;
		bv[3].modulate[2]=tintB; bv[3].modulate[3]=beamAlpha;

		trap_R_AddPolyToScene( cgs.media.kothBeamShader, 4, bv );

		/* --- quad 2: aligned along Y axis (crossed with quad 1) --- */
		bv[0].xyz[0]=cx; bv[0].xyz[1]=cy-beamHalfWidth; bv[0].xyz[2]=bz0;
		bv[1].xyz[0]=cx; bv[1].xyz[1]=cy+beamHalfWidth; bv[1].xyz[2]=bz0;
		bv[2].xyz[0]=cx; bv[2].xyz[1]=cy+beamHalfWidth; bv[2].xyz[2]=bz1;
		bv[3].xyz[0]=cx; bv[3].xyz[1]=cy-beamHalfWidth; bv[3].xyz[2]=bz1;
		/* st and modulate same as quad 1 - already set above */
		trap_R_AddPolyToScene( cgs.media.kothBeamShader, 4, bv );
	}

	/* ============================================================
	   3) HILL MARKER at beam tip
	      Pulses in inverse phase to the beam so one is always
	      clearly visible.
	   ============================================================ */
	Com_Memset( &marker, 0, sizeof( marker ) );
	marker.reType        = RT_SPRITE;
	VectorCopy( cgs.kothHillOrigin, marker.origin );
	marker.origin[2]    += 4.0f;
	marker.customShader  = hillMarkerShader;
	marker.shaderRGBA[0] = tintR;
	marker.shaderRGBA[1] = tintG;
	marker.shaderRGBA[2] = tintB;
	marker.shaderRGBA[3] = (byte)( 110 + 105 * inversePulse );
	marker.rotation      = 0.0f;
	marker.radius        = minimapPass
		? ( 54.0f + 10.0f * inversePulse )
		: ( 38.0f +  8.0f * inversePulse );
	trap_R_AddRefEntityToScene( &marker );
}


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
	int   tmp;
	float labelX, labelY;
	float frameX, frameY, frameW, frameH;
	screenPlacement_e savedHorizontalPlacement, savedVerticalPlacement;

	if ( !cg_drawRearView.integer )
		return;
	if ( cg.snap->ps.pm_type == PM_INTERMISSION )
		return;
	if ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_SPECTATOR )
		return;

	savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
	savedVerticalPlacement = CG_GetScreenVerticalPlacement();
	CG_SetScreenPlacement( PLACE_CENTER, PLACE_TOP );
	frameX = x - 1.0f;
	frameY = y - 1.0f;
	frameW = w + 2.0f;
	frameH = h + 2.0f;
	labelX = x + 5.0f;
	labelY = y + 3.0f;
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
	CG_DrawRect( frameX, frameY, frameW, frameH, 1.0f, vehicleHudFrame );
	CG_DrawIngameString( (int)labelX, (int)labelY,
	                     "REAR VIEW", UI_SMALLFONT | UI_DROPSHADOW,
	                     0.58f, vehicleHudLabel );
	CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
}


/* -----------------------------------------------------------------------
   CG_DrawMMap
   Top-down minimap overlay.
   ----------------------------------------------------------------------- */
void CG_DrawMMap( float x, float y, float w, float h ) {
	float overlay_x, overlay_y, frameX, frameY, frameW, frameH;
	float tmp;
	screenPlacement_e savedHorizontalPlacement, savedVerticalPlacement;

	if ( !cg_drawMMap.integer )
		return;
	if ( cg.snap->ps.pm_type == PM_INTERMISSION )
		return;
	if ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_SPECTATOR )
		return;

	savedHorizontalPlacement = CG_GetScreenHorizontalPlacement();
	savedVerticalPlacement = CG_GetScreenVerticalPlacement();
	CG_SetScreenPlacement( PLACE_LEFT, PLACE_TOP );
	overlay_x = x;
	overlay_y = y;
	frameX = x - 1.0f;
	frameY = y - 1.0f;
	frameW = w * cg_mmap_size.value + 2.0f;
	frameH = h * cg_mmap_size.value + 2.0f;
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
	CG_AddKOTHHillIndicatorToScene( qtrue );

	if ( cg_mmap_renderLevel.integer & RL_PLAYERS )
		CG_AddCEntity( &cg_entities[cg.snap->ps.clientNum] );

	trap_R_RenderScene( &cg.mmapRefdef );
	CG_DrawRect( frameX, frameY, frameW, frameH, 1.0f, vehicleHudFrame );
	CG_DrawIngameString( (int)( overlay_x + 7.0f ), (int)( overlay_y + 4.0f ),
	                     "MAP", UI_SMALLFONT | UI_DROPSHADOW,
	                     0.58f, vehicleHudLabel );
	CG_SetScreenPlacement( savedHorizontalPlacement, savedVerticalPlacement );
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
			CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );
			CG_DrawIngameString( (int)( SCREEN_WIDTH * 0.5f ),
			                      (int)( SCREEN_HEIGHT * 0.30f ),
			                      "FUEL CRITICAL - REFUEL NOW!",
			                      UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW,
			                      0.9f, warnColor );
			CG_PopScreenPlacement();
		}
	} else {
		warned = qfalse;
		CG_FillRect( x + 1, y + 1, (w - 2) * frac, h - 2, fuelColor );
		CG_DrawPic( x - size - 4, y + ( h - size ) * 0.5f, size, size, icon );
	}
}

#define RPM_GAUGE_RED_FRACTION 0.85f

static float CG_RPMGaugeFraction( int rpm ) {
	float frac;

	if ( rpm <= 0 ) {
		return 0.0f;
	}
	if ( rpm >= CP_RPM_MAX ) {
		return 1.0f;
	}

	frac = rpm / (float)CP_RPM_MAX;
	if ( frac < 0.0f ) {
		return 0.0f;
	}
	if ( frac > 1.0f ) {
		return 1.0f;
	}
	return frac;
}

static void CG_DrawRPMGaugeBar( float x, float y, float width, float height, int rpm ) {
	float frac;
	float innerX, innerY, innerW, innerH;
	float fillW, redStartW;
	vec4_t backColor = { 0.0f, 0.0f, 0.0f, 0.25f };
	vec4_t revColor = { 1.0f, 0.12f, 0.02f, 1.0f };

	if ( width <= 2.0f || height <= 2.0f ) {
		return;
	}

	frac = CG_RPMGaugeFraction( rpm );
	innerX = x + 1.0f;
	innerY = y + 1.0f;
	innerW = width - 2.0f;
	innerH = height - 2.0f;
	fillW = innerW * frac;
	redStartW = innerW * RPM_GAUGE_RED_FRACTION;

	CG_DrawRect( x, y, width, height, 1, colorWhite );
	CG_FillRect( innerX, innerY, innerW, innerH, backColor );

	if ( fillW <= 0.0f ) {
		return;
	}
	if ( fillW <= redStartW ) {
		CG_FillRect( innerX, innerY, fillW, innerH, colorWhite );
	} else {
		CG_FillRect( innerX, innerY, redStartW, innerH, colorWhite );
		CG_FillRect( innerX + redStartW, innerY, fillW - redStartW, innerH, revColor );
	}
}


/* -----------------------------------------------------------------------
   CG_DrawSpeed
   Speedometer: digital bar-graph (cg_speedometerMode 1) or
                analog dial with needle (cg_speedometerMode 0).
   Both modes also render the fuel gauge and RPM bar.
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
		int   maxLen, len, rpm;
		float bgClr[4] = { 0.0f, 0.0f, 0.0f, 0.25f };
		const float segmentHeight = 8.0f;
		const float gaugeHeight   = 12.0f;
		const float rpmIconSize   = gaugeHeight * 2;
		const float rpmIconOffset = rpmIconSize + 4;
		const float rpmIconNudge  = 3.0f;
		float iconOffset  = gaugeHeight * 2 + 4;
		float blockWidth, blockHeight, barWidth;
		int   speedWidth, gearWidth;

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

		barWidth    = maxLen * 12 - 8;
		blockWidth  = max( iconOffset + barWidth, rpmIconOffset + maxLen * 12 );
		blockHeight = segmentHeight + 2 * GIANTCHAR_HEIGHT + gaugeHeight;
		blockHeight += 18.0f;

		x  = 640 - (int)blockWidth - 8;
		y -= blockHeight;

		{
			float rectX = x - 4, rectY = y - 4;
			float rectW = blockWidth + 8, rectH = blockHeight + 8;
			CG_DrawVehicleHudFrame( rectX, rectY, rectW, rectH, "SPEED" );
			CG_FillRect( rectX + 4, rectY + 20, rectW - 8,
			             rectH - 24, bgClr );
		}
		y += 18;

		{
			static qhandle_t rpmIcon;
			if ( !rpmIcon ) rpmIcon = trap_R_RegisterShaderNoMip( "icons/rpm" );
			CG_DrawPic( x - rpmIconNudge, y, rpmIconSize, rpmIconSize, rpmIcon );
		}
		CG_DrawRPMGaugeBar( x + rpmIconOffset, y, maxLen * 12, segmentHeight, rpm );

		speedWidth = CG_IngameStringWidth( speedStr, UI_SMALLFONT, 0.75f );
		gearWidth  = CG_IngameStringWidth( gearStr, UI_SMALLFONT, 0.75f );
		y += segmentHeight;
		CG_DrawIngameString( x + (int)blockWidth - speedWidth, (int)y,
		                     speedStr, UI_SMALLFONT | UI_DROPSHADOW,
		                     0.75f, colorWhite );
		y += 12;
		CG_DrawIngameString( x + (int)blockWidth - gearWidth, (int)y,
		                     gearStr, UI_SMALLFONT | UI_DROPSHADOW,
		                     0.75f, colorWhite );
		y += 12;
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
		char gearText[8];

		left = 640 - blockWidth - 8;
		blockHeight += 18.0f;
		top  = y - blockHeight;

		CG_DrawVehicleHudFrame( left - 4, top - 4, blockWidth + 8,
		                        blockHeight + 8, "SPEED" );
		top += 18.0f;
		CG_DrawPic( left, top, gaugeSize, gaugeSize,
		            cg_metricUnits.integer ? cgs.media.gaugeMetric : cgs.media.gaugeImperial );

		speedWidth = CG_IngameStringWidth( va("%i", vel_speed),
		                                  UI_SMALLFONT, 0.75f );
		speedX = left + (gaugeSize - speedWidth) * 0.5f;
		speedY = top  + gaugeSize - 35;
		CG_DrawIngameString( (int)speedX, (int)speedY, va("%i", vel_speed),
		                     UI_SMALLFONT | UI_DROPSHADOW, 0.75f, colorWhite );

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
			Q_strncpyz( gearText, "R", sizeof(gearText) );
		else if ( cg.predictedPlayerState.stats[STAT_GEAR] ==  0 )
			Q_strncpyz( gearText, "N", sizeof(gearText) );
		else
			Com_sprintf( gearText, sizeof(gearText), "%i",
			             cg.predictedPlayerState.stats[STAT_GEAR] );
		CG_DrawIngameString( (int)( centerX + 12.0f ),
		                     (int)( centerY + 5.0f ), gearText,
		                     UI_CENTER | UI_SMALLFONT | UI_DROPSHADOW,
		                     0.75f, colorWhite );

		CG_DrawFuelGauge( left + (blockWidth - fuelWidth) * 0.5f,
		                  top + gaugeSize + gaugeSpacing,
		                  fuelWidth, fuelHeight );

		{
			int   rpm      = cg.predictedPlayerState.stats[STAT_RPM];
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

			CG_DrawRPMGaugeBar( segX, segY, fuelWidth, rpmHeight, rpm );
		}

		y = yorg - 44 - blockHeight;
		return y;
	}
}
