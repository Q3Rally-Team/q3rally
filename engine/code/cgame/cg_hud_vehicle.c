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
===========================================================================
*/

#include "cg_local.h"
#include "cg_hud_elements.h"

static const vec4_t vehicleHudLabel  = { 0.57f, 0.70f, 0.74f, 0.90f };
static const vec4_t vehicleHudFrame  = { 0.30f, 0.43f, 0.49f, 0.82f };

/* -----------------------------------------------------------------------
   King of the Hill indicator
   A golden crown floats above the hill centre (models/mapobjects/koth/
   crown.md3); its gems and velvet cap glow in the colour of the owner.
   Bollards (post.md3) stand on the border of the hill zone; while a team
   captures the hill they light up one after another in its colour, so the
   ring shows the progress. Bollards near a car sink into the ground.
   Used in the main world pass and in the minimap pass.
   ----------------------------------------------------------------------- */

#define KOTH_POSTS_MIN      12
#define KOTH_POSTS_MAX      32
#define KOTH_POST_SPACING   110.0f
#define KOTH_POST_HEIGHT    54.0f
#define KOTH_POST_DUCK      100.0f		/* bollards within this distance of a car sink */

static struct {
	vec3_t	origin;
	float	half[2];
	int		count;
	vec3_t	pos[KOTH_POSTS_MAX];
	float	groundZ;
	qboolean show[KOTH_POSTS_MAX];	/* false where the border runs off the hill (round top, square zone) */
	qboolean valid;
} kothCache;

static void CG_KOTH_Lerp( const vec3_t a, const vec3_t b, float f, vec3_t out ) {
	out[0] = a[0] + ( b[0] - a[0] ) * f;
	out[1] = a[1] + ( b[1] - a[1] ) * f;
	out[2] = a[2] + ( b[2] - a[2] ) * f;
}

static void CG_KOTH_TeamColor( int team, vec3_t out ) {
	if ( team == TEAM_RED ) {
		VectorSet( out, 1.0f, 0.16f, 0.10f );
	} else if ( team == TEAM_BLUE ) {
		VectorSet( out, 0.16f, 0.36f, 1.0f );
	} else {
		VectorSet( out, 0.92f, 0.90f, 0.82f );
	}
}

static float CG_KOTH_GroundZ( float x, float y, float fromZ ) {
	trace_t	tr;
	vec3_t	start, end;

	VectorSet( start, x, y, fromZ );
	VectorSet( end, x, y, fromZ - 4096.0f );
	CG_Trace( &tr, start, NULL, NULL, end, ENTITYNUM_NONE, MASK_PLAYERSOLID );
	if ( tr.startsolid ) {
		/* started inside something (a pole in the middle of the hill): the floor is beside it */
		start[0] += 40.0f;
		end[0] += 40.0f;
		CG_Trace( &tr, start, NULL, NULL, end, ENTITYNUM_NONE, MASK_PLAYERSOLID );
	}
	return ( tr.fraction < 1.0f && !tr.startsolid ) ? tr.endpos[2] : fromZ - 256.0f;
}

/* Ground point under the hill centre and the bollard positions along the
   border of the zone (a box: CS_KOTHSTATUS sends its half extents). Only
   recomputed when the hill changes. */
static void CG_KOTH_UpdateCache( void ) {
	float	hx, hy, perimeter, step, d;
	int		i;

	hx = cgs.kothHillHalf[0] > 0.0f ? cgs.kothHillHalf[0] : cgs.kothHillRadius;
	hy = cgs.kothHillHalf[1] > 0.0f ? cgs.kothHillHalf[1] : cgs.kothHillRadius;
	if ( kothCache.valid && VectorCompare( kothCache.origin, cgs.kothHillOrigin ) &&
		kothCache.half[0] == hx && kothCache.half[1] == hy ) {
		return;
	}
	VectorCopy( cgs.kothHillOrigin, kothCache.origin );
	kothCache.half[0] = hx;
	kothCache.half[1] = hy;
	kothCache.groundZ = CG_KOTH_GroundZ( cgs.kothHillOrigin[0], cgs.kothHillOrigin[1],
		cgs.kothHillOrigin[2] + 256.0f );

	perimeter = 4.0f * ( hx + hy );
	kothCache.count = (int)( perimeter / KOTH_POST_SPACING + 0.5f );
	kothCache.count = ( kothCache.count + 3 ) & ~3;		/* a multiple of 4: a post on every corner of a square */
	if ( kothCache.count < KOTH_POSTS_MIN ) kothCache.count = KOTH_POSTS_MIN;
	if ( kothCache.count > KOTH_POSTS_MAX ) kothCache.count = KOTH_POSTS_MAX;
	step = perimeter / kothCache.count;
	/* walk the border counter-clockwise, starting in the middle of the north side */
	for ( i = 0; i < kothCache.count; i++ ) {
		float x, y;
		d = i * step + hx;
		if ( d >= perimeter ) {
			d -= perimeter;
		}
		if ( d < 2.0f * hx ) {
			x = hx - d;			y = hy;
		} else if ( d < 2.0f * hx + 2.0f * hy ) {
			x = -hx;			y = hy - ( d - 2.0f * hx );
		} else if ( d < 4.0f * hx + 2.0f * hy ) {
			x = -hx + ( d - 2.0f * hx - 2.0f * hy );	y = -hy;
		} else {
			x = hx;				y = -hy + ( d - 4.0f * hx - 2.0f * hy );
		}
		kothCache.pos[i][0] = cgs.kothHillOrigin[0] + x;
		kothCache.pos[i][1] = cgs.kothHillOrigin[1] + y;
		kothCache.pos[i][2] = CG_KOTH_GroundZ( kothCache.pos[i][0], kothCache.pos[i][1],
			cgs.kothHillOrigin[2] + 256.0f );
		kothCache.show[i] = fabs( kothCache.pos[i][2] - kothCache.groundZ ) < 72.0f;
	}
	kothCache.valid = qtrue;
}

/* XY distance from a point to the nearest car (other players and our own). */
static float CG_KOTH_NearestCar( const vec3_t p ) {
	float	best = 1e9f, d;
	int		i;

	if ( cg.snap ) {
		for ( i = 0; i < cg.snap->numEntities; i++ ) {
			centity_t *cent = &cg_entities[ cg.snap->entities[i].number ];
			if ( cg.snap->entities[i].eType != ET_PLAYER ) {
				continue;
			}
			d = Square( cent->lerpOrigin[0] - p[0] ) + Square( cent->lerpOrigin[1] - p[1] );
			if ( d < best ) best = d;
		}
		if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
			d = Square( cg.predictedPlayerState.origin[0] - p[0] ) +
				Square( cg.predictedPlayerState.origin[1] - p[1] );
			if ( d < best ) best = d;
		}
	}
	return sqrt( best );
}

static void CG_KOTH_Sprite( const vec3_t origin, float radius, const vec3_t color, float alpha ) {
	refEntity_t	ent;

	if ( alpha <= 0.0f ) {
		return;
	}
	Com_Memset( &ent, 0, sizeof( ent ) );
	ent.reType = RT_SPRITE;
	VectorCopy( origin, ent.origin );
	ent.radius = radius;
	ent.customShader = cgs.media.kothGlowShader;
	ent.shaderRGBA[0] = (byte)( 255 * Com_Clamp( 0, 1, color[0] * alpha ) );
	ent.shaderRGBA[1] = (byte)( 255 * Com_Clamp( 0, 1, color[1] * alpha ) );
	ent.shaderRGBA[2] = (byte)( 255 * Com_Clamp( 0, 1, color[2] * alpha ) );
	ent.shaderRGBA[3] = 255;
	trap_R_AddRefEntityToScene( &ent );
}

void CG_AddKOTHHillIndicatorToScene( qboolean minimapPass ) {
	refEntity_t	ent;
	vec3_t		owner, capturer, yellow, gem, angles, c;
	float		t, scale, hop, hopAge, flicker, lit, pct;
	int			i;
	qboolean	capturing;

	if ( cgs.gametype != GT_KOTH || !cgs.kothHillOriginValid || !cgs.media.kothCrownModel ) {
		return;
	}
	CG_KOTH_UpdateCache();

	t = cg.time * 0.001f;
	pct = Com_Clamp( 0, 100, cgs.kothCapturePct ) / 100.0f;
	capturing = !cgs.kothContested && ( cgs.kothCapturingTeam == TEAM_RED || cgs.kothCapturingTeam == TEAM_BLUE ) &&
		cgs.kothCapturingTeam != cgs.kothOwner && pct > 0.0f && pct < 1.0f;
	CG_KOTH_TeamColor( cgs.kothOwner, owner );
	CG_KOTH_TeamColor( capturing ? cgs.kothCapturingTeam : cgs.kothOwner, capturer );
	VectorSet( yellow, 1.0f, 0.82f, 0.18f );
	flicker = 0.5f + 0.5f * sin( t * 25.0f );

	/* ---- gem colour ---- */
	if ( cgs.kothContested ) {
		CG_KOTH_Lerp( owner, yellow, flicker, gem );
	} else if ( capturing ) {
		/* pulse towards the capturing team, faster and stronger as it gets close */
		float k = pct * ( 0.6f + 0.4f * sin( t * ( 6.0f + 14.0f * pct ) ) );
		CG_KOTH_Lerp( owner, capturer, Com_Clamp( 0, 1, k ), gem );
	} else {
		VectorCopy( owner, gem );
		if ( cgs.kothOwner == TEAM_FREE ) {
			VectorScale( gem, 0.92f + 0.08f * sin( t * 2.0f ), gem );
		}
	}

	/* ---- crown ---- */
	scale = Com_Clamp( 1.3f, 2.4f, cgs.kothHillRadius / 200.0f );
	hopAge = cg.kothOwnerChangeTime ? ( cg.time - cg.kothOwnerChangeTime ) / 1400.0f : 1.0f;
	hop = ( hopAge >= 0.0f && hopAge < 1.0f ) ? sin( hopAge * M_PI ) : 0.0f;

	Com_Memset( &ent, 0, sizeof( ent ) );
	ent.reType = RT_MODEL;
	ent.hModel = cgs.media.kothCrownModel;
	ent.origin[0] = cgs.kothHillOrigin[0];
	ent.origin[1] = cgs.kothHillOrigin[1];
	ent.origin[2] = kothCache.groundZ + 110.0f + 45.0f * scale + 7.0f * sin( t * 1.7f ) + 70.0f * hop;
	angles[YAW] = AngleMod( t * 32.0f + ( hop > 0.0f ? 720.0f * ( 1.0f - Square( 1.0f - hopAge ) ) : 0.0f ) );
	angles[PITCH] = 3.0f * sin( t * 0.9f );
	angles[ROLL] = 3.0f * cos( t * 0.7f );
	if ( cgs.kothContested ) {			/* rattles while both teams are on the hill */
		angles[PITCH] += 5.0f * sin( t * 31.0f );
		angles[ROLL] += 5.0f * cos( t * 27.0f );
		ent.origin[2] += 3.0f * sin( t * 43.0f );
	}
	if ( minimapPass ) {
		scale *= 3.5f;
	}
	scale *= 1.0f + 0.18f * hop;
	AnglesToAxis( angles, ent.axis );
	VectorScale( ent.axis[0], scale, ent.axis[0] );
	VectorScale( ent.axis[1], scale, ent.axis[1] );
	VectorScale( ent.axis[2], scale, ent.axis[2] );
	ent.nonNormalizedAxes = qtrue;
	/* the model origin is the bottom of the band: put its middle (z 40) at
	   the hover point so it turns and tilts about its centre */
	VectorMA( ent.origin, -40.0f, ent.axis[2], ent.origin );
	VectorCopy( ent.origin, ent.lightingOrigin );
	ent.renderfx = RF_MINLIGHT | RF_LIGHTING_ORIGIN;
	ent.shaderRGBA[0] = (byte)( 255 * Com_Clamp( 0, 1, gem[0] ) );
	ent.shaderRGBA[1] = (byte)( 255 * Com_Clamp( 0, 1, gem[1] ) );
	ent.shaderRGBA[2] = (byte)( 255 * Com_Clamp( 0, 1, gem[2] ) );
	ent.shaderRGBA[3] = 255;
	trap_R_AddRefEntityToScene( &ent );

	/* aura behind the crown and a light on the hill */
	VectorMA( ent.origin, 40.0f, ent.axis[2], c );
	CG_KOTH_Sprite( c, 70.0f * scale, gem, 0.35f + 0.15f * sin( t * 3.0f ) + 0.4f * hop );
	if ( !minimapPass ) {
		trap_R_AddLightToScene( c, 260.0f + 40.0f * sin( t * 3.0f ) + 200.0f * hop, gem[0], gem[1], gem[2] );
	}

	/* ---- bollards on the zone border ---- */
	if ( !cgs.media.kothPostModel ) {
		return;
	}
	lit = capturing ? pct * kothCache.count : 0.0f;
	for ( i = 0; i < kothCache.count; i++ ) {
		vec3_t	col, glow;
		float	sink = 0.0f, on, d;

		if ( !kothCache.show[i] ) {
			continue;
		}
		if ( cgs.kothContested ) {
			CG_KOTH_Lerp( owner, yellow, ( ( i + (int)( t * 6.0f ) ) & 1 ) ? 1.0f : 0.15f, col );
		} else if ( capturing ) {
			on = Com_Clamp( 0, 1, lit - i );
			VectorScale( owner, cgs.kothOwner == TEAM_FREE ? 0.4f : 0.55f, col );
			CG_KOTH_Lerp( col, capturer, on, col );
		} else if ( cgs.kothOwner == TEAM_FREE ) {
			VectorScale( owner, 0.62f + 0.1f * sin( t * 2.0f ), col );
		} else {
			/* owned: a slow wave runs around the ring */
			float w = 0.75f + 0.25f * sin( t * 3.0f - i * ( 2.0f * M_PI / kothCache.count ) * 2.0f );
			VectorScale( owner, w, col );
		}

		if ( !minimapPass ) {
			d = CG_KOTH_NearestCar( kothCache.pos[i] );
			if ( d < KOTH_POST_DUCK ) {
				sink = ( 1.0f - d / KOTH_POST_DUCK ) * ( KOTH_POST_HEIGHT + 4.0f );
			}
		}

		Com_Memset( &ent, 0, sizeof( ent ) );
		ent.reType = RT_MODEL;
		ent.hModel = cgs.media.kothPostModel;
		VectorCopy( kothCache.pos[i], ent.origin );
		ent.origin[2] -= sink;
		AxisClear( ent.axis );
		if ( minimapPass ) {
			VectorScale( ent.axis[0], 2.0f, ent.axis[0] );
			VectorScale( ent.axis[1], 2.0f, ent.axis[1] );
			VectorScale( ent.axis[2], 2.0f, ent.axis[2] );
			ent.nonNormalizedAxes = qtrue;
		}
		VectorCopy( kothCache.pos[i], ent.lightingOrigin );
		ent.lightingOrigin[2] += 32.0f;
		ent.renderfx = RF_MINLIGHT | RF_LIGHTING_ORIGIN;
		ent.shaderRGBA[0] = (byte)( 255 * Com_Clamp( 0, 1, col[0] ) );
		ent.shaderRGBA[1] = (byte)( 255 * Com_Clamp( 0, 1, col[1] ) );
		ent.shaderRGBA[2] = (byte)( 255 * Com_Clamp( 0, 1, col[2] ) );
		ent.shaderRGBA[3] = 255;
		trap_R_AddRefEntityToScene( &ent );

		/* glow around the lamp */
		VectorCopy( ent.origin, glow );
		glow[2] += ( minimapPass ? 2.0f : 1.0f ) * 42.0f;
		CG_KOTH_Sprite( glow, minimapPass ? 56.0f : 30.0f, col, 0.8f * ( 1.0f - sink / KOTH_POST_HEIGHT ) );
	}
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
