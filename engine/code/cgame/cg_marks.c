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
// cg_marks.c -- wall marks

#include "cg_local.h"

/*
===================================================================

MARK POLYS

===================================================================
*/


markPoly_t	cg_activeMarkPolys;			// double linked list
markPoly_t	*cg_freeMarkPolys;			// single linked list
markPoly_t	cg_markPolys[MAX_MARK_POLYS];
static		int	markTotal;

/*
===================
CG_InitMarkPolys

This is called at startup and for tournement restarts
===================
*/
void	CG_InitMarkPolys( void ) {
	int		i;

	memset( cg_markPolys, 0, sizeof(cg_markPolys) );

	cg_activeMarkPolys.nextMark = &cg_activeMarkPolys;
	cg_activeMarkPolys.prevMark = &cg_activeMarkPolys;
	cg_freeMarkPolys = cg_markPolys;
	for ( i = 0 ; i < MAX_MARK_POLYS - 1 ; i++ ) {
		cg_markPolys[i].nextMark = &cg_markPolys[i+1];
	}
}


/*
==================
CG_FreeMarkPoly
==================
*/
void CG_FreeMarkPoly( markPoly_t *le ) {
	if ( !le->prevMark || !le->nextMark ) {
		CG_Error( "CG_FreeLocalEntity: not active" );
	}

	// remove from the doubly linked active list
	le->prevMark->nextMark = le->nextMark;
	le->nextMark->prevMark = le->prevMark;

	// the free list is only singly linked
	le->nextMark = cg_freeMarkPolys;
	cg_freeMarkPolys = le;
}

/*
===================
CG_AllocMark

Will allways succeed, even if it requires freeing an old active mark
===================
*/
markPoly_t	*CG_AllocMark( void ) {
	markPoly_t	*le;
	int time;

	if ( !cg_freeMarkPolys ) {
		// no free entities, so free the one at the end of the chain
		// remove the oldest active entity
		time = cg_activeMarkPolys.prevMark->time;
		while (cg_activeMarkPolys.prevMark && time == cg_activeMarkPolys.prevMark->time) {
			CG_FreeMarkPoly( cg_activeMarkPolys.prevMark );
		}
	}

	le = cg_freeMarkPolys;
	cg_freeMarkPolys = cg_freeMarkPolys->nextMark;

	memset( le, 0, sizeof( *le ) );

	// link into the active list
	le->nextMark = cg_activeMarkPolys.nextMark;
	le->prevMark = &cg_activeMarkPolys;
	cg_activeMarkPolys.nextMark->prevMark = le;
	cg_activeMarkPolys.nextMark = le;
	return le;
}



/*
=================
CG_ImpactMark

origin should be a point within a unit of the plane
dir should be the plane normal

temporary marks will not be stored or randomly oriented, but immediately
passed to the renderer.
=================
*/
#define	MAX_MARK_FRAGMENTS	128
#define	MAX_MARK_POINTS		384

void CG_ImpactMark( qhandle_t markShader, const vec3_t origin, const vec3_t dir, 
				   float orientation, float red, float green, float blue, float alpha,
				   qboolean alphaFade, float radius, qboolean temporary ) {
	vec3_t			axis[3];
	float			texCoordScale;
	vec3_t			originalPoints[4];
	byte			colors[4];
	int				i, j;
	int				numFragments;
	markFragment_t	markFragments[MAX_MARK_FRAGMENTS], *mf;
	vec3_t			markPoints[MAX_MARK_POINTS];
	vec3_t			projection;

	if ( !cg_addMarks.integer ) {
		return;
	}

	if ( radius <= 0 ) {
		CG_Error( "CG_ImpactMark called with <= 0 radius" );
	}

	//if ( markTotal >= MAX_MARK_POLYS ) {
	//	return;
	//}

	// create the texture axis
	VectorNormalize2( dir, axis[0] );
	PerpendicularVector( axis[1], axis[0] );
	RotatePointAroundVector( axis[2], axis[0], axis[1], orientation );
	CrossProduct( axis[0], axis[2], axis[1] );

	texCoordScale = 0.5 * 1.0 / radius;

	// create the full polygon
	for ( i = 0 ; i < 3 ; i++ ) {
		originalPoints[0][i] = origin[i] - radius * axis[1][i] - radius * axis[2][i];
		originalPoints[1][i] = origin[i] + radius * axis[1][i] - radius * axis[2][i];
		originalPoints[2][i] = origin[i] + radius * axis[1][i] + radius * axis[2][i];
		originalPoints[3][i] = origin[i] - radius * axis[1][i] + radius * axis[2][i];
	}

	// get the fragments
	VectorScale( dir, -20, projection );
	numFragments = trap_CM_MarkFragments( 4, (void *)originalPoints,
					projection, MAX_MARK_POINTS, markPoints[0],
					MAX_MARK_FRAGMENTS, markFragments );

	colors[0] = red * 255;
	colors[1] = green * 255;
	colors[2] = blue * 255;
	colors[3] = alpha * 255;

	for ( i = 0, mf = markFragments ; i < numFragments ; i++, mf++ ) {
		polyVert_t	*v;
		polyVert_t	verts[MAX_VERTS_ON_POLY];
		markPoly_t	*mark;

		// we have an upper limit on the complexity of polygons
		// that we store persistantly
		if ( mf->numPoints > MAX_VERTS_ON_POLY ) {
			mf->numPoints = MAX_VERTS_ON_POLY;
		}
		for ( j = 0, v = verts ; j < mf->numPoints ; j++, v++ ) {
			vec3_t		delta;

			VectorCopy( markPoints[mf->firstPoint + j], v->xyz );

			VectorSubtract( v->xyz, origin, delta );
			v->st[0] = 0.5 + DotProduct( delta, axis[1] ) * texCoordScale;
			v->st[1] = 0.5 + DotProduct( delta, axis[2] ) * texCoordScale;
			*(int *)v->modulate = *(int *)colors;
		}

		// if it is a temporary (shadow) mark, add it immediately and forget about it
		if ( temporary ) {
			trap_R_AddPolyToScene( markShader, mf->numPoints, verts );
			continue;
		}

		// otherwise save it persistantly
		mark = CG_AllocMark();
		mark->time = cg.time;
		mark->alphaFade = alphaFade;
		mark->markShader = markShader;
		mark->poly.numVerts = mf->numPoints;
		mark->color[0] = red;
		mark->color[1] = green;
		mark->color[2] = blue;
		mark->color[3] = alpha;
		memcpy( mark->verts, verts, mf->numPoints * sizeof( verts[0] ) );
		markTotal++;
	}
}


// Q3Rally Code Start
/*
=================
CG_ImpactMark2

origin should be a point within a unit of the plane
up should be the plane normal
forward is the direction the mark faces

This function is just like CG_ImpactMark except it uses a forward vector
instead of a yaw angle to fix the shadows on none horizontal surfaces

=================
*/
void CG_ImpactMark2( qhandle_t markShader, const vec3_t origin, const vec3_t up, 
				   const vec3_t forward, float red, float green, float blue, float alpha,
				   qboolean alphaFade, float radius, qboolean temporary ) {
	vec3_t			axis[3];
	float			texCoordScale;
	vec3_t			originalPoints[4];
	byte			colors[4];
	int				i, j;
	int				numFragments;
	markFragment_t	markFragments[MAX_MARK_FRAGMENTS], *mf;
	vec3_t			markPoints[MAX_MARK_POINTS];
	vec3_t			projection;

	if ( !cg_addMarks.integer ) {
		return;
	}

	if ( radius <= 0 ) {
		CG_Error( "CG_ImpactMark called with <= 0 radius" );
	}

	//if ( markTotal >= MAX_MARK_POLYS ) {
	//	return;
	//}

	// create the texture axis
	VectorNormalize2( up, axis[0] );
	VectorNormalize2( forward, axis[2] );
	CrossProduct( axis[0], axis[2], axis[1] );
	CrossProduct( axis[1], axis[0], axis[2] );

	texCoordScale = 0.5 * 1.0 / radius;

	// create the full polygon
	for ( i = 0 ; i < 3 ; i++ ) {
		originalPoints[0][i] = origin[i] - radius * axis[1][i] - radius * axis[2][i];
		originalPoints[1][i] = origin[i] + radius * axis[1][i] - radius * axis[2][i];
		originalPoints[2][i] = origin[i] + radius * axis[1][i] + radius * axis[2][i];
		originalPoints[3][i] = origin[i] - radius * axis[1][i] + radius * axis[2][i];
	}

	// get the fragments
	VectorScale( up, -20, projection );
	numFragments = trap_CM_MarkFragments( 4, (void *)originalPoints,
					projection, MAX_MARK_POINTS, markPoints[0],
					MAX_MARK_FRAGMENTS, markFragments );

	colors[0] = red * 255;
	colors[1] = green * 255;
	colors[2] = blue * 255;
	colors[3] = alpha * 255;

	for ( i = 0, mf = markFragments ; i < numFragments ; i++, mf++ ) {
		polyVert_t	*v;
		polyVert_t	verts[MAX_VERTS_ON_POLY];
		markPoly_t	*mark;

		// we have an upper limit on the complexity of polygons
		// that we store persistantly
		if ( mf->numPoints > MAX_VERTS_ON_POLY ) {
			mf->numPoints = MAX_VERTS_ON_POLY;
		}
		for ( j = 0, v = verts ; j < mf->numPoints ; j++, v++ ) {
			vec3_t		delta;

			VectorCopy( markPoints[mf->firstPoint + j], v->xyz );

			VectorSubtract( v->xyz, origin, delta );
			v->st[0] = 0.5 + DotProduct( delta, axis[1] ) * texCoordScale;
			v->st[1] = 0.5 + DotProduct( delta, axis[2] ) * texCoordScale;
			*(int *)v->modulate = *(int *)colors;
		}

		// if it is a temporary (shadow) mark, add it immediately and forget about it
		if ( temporary ) {
			trap_R_AddPolyToScene( markShader, mf->numPoints, verts );
			continue;
		}

		// otherwise save it persistantly
		mark = CG_AllocMark();
		mark->time = cg.time;
		mark->alphaFade = alphaFade;
		mark->markShader = markShader;
		mark->poly.numVerts = mf->numPoints;
		mark->color[0] = red;
		mark->color[1] = green;
		mark->color[2] = blue;
		mark->color[3] = alpha;
		memcpy( mark->verts, verts, mf->numPoints * sizeof( verts[0] ) );
		markTotal++;
	}
}
// Q3Rally Code END


// Q3Rally Code Start: optimize to get this to make long skids as one mark
void CG_SkidMark( qhandle_t markShader, const vec3_t origin, const vec3_t up, 
				   const vec3_t forward, float red, float green, float blue, float alpha,
				   qboolean alphaFade, float width, float length, qboolean temporary) {

	vec3_t			axis[3];
	float			texCoordScale;
	vec3_t			originalPoints[4];
	byte			colors[4];
	int				i, j;
	int				numFragments;
	markFragment_t	markFragments[MAX_MARK_FRAGMENTS], *mf;
	vec3_t			markPoints[MAX_MARK_POINTS];
	vec3_t			projection;

	if ( !cg_addMarks.integer ) {
		return;
	}

	if ( width <= 0 ) {
		CG_Error( "CG_ImpactMark called with <= 0 width" );
	}

	// create the texture axis
	VectorNormalize2( up, axis[0] );
	VectorNormalize2( forward, axis[2] );
	CrossProduct( axis[0], axis[2], axis[1] );
	CrossProduct( axis[1], axis[0], axis[2] );

	texCoordScale = 0.5 * 1.0 / width;

	// create the full polygon
	for ( i = 0 ; i < 3 ; i++ ) {
		originalPoints[0][i] = origin[i] - width * axis[1][i] - length * axis[2][i];
		originalPoints[1][i] = origin[i] + width * axis[1][i] - length * axis[2][i];
		originalPoints[2][i] = origin[i] + width * axis[1][i] + length * axis[2][i];
		originalPoints[3][i] = origin[i] - width * axis[1][i] + length * axis[2][i];
	}

	// get the fragments
	VectorScale( up, -20, projection );
	numFragments = trap_CM_MarkFragments( 4, (void *)originalPoints,
					projection, MAX_MARK_POINTS, markPoints[0],
					MAX_MARK_FRAGMENTS, markFragments );

	colors[0] = red * 255;
	colors[1] = green * 255;
	colors[2] = blue * 255;
	colors[3] = alpha * 255;

	for ( i = 0, mf = markFragments ; i < numFragments ; i++, mf++ ) {
		polyVert_t	*v;
		polyVert_t	verts[MAX_VERTS_ON_POLY];
		markPoly_t	*mark;

		// we have an upper limit on the complexity of polygons
		// that we store persistantly
		if ( mf->numPoints > MAX_VERTS_ON_POLY ) {
			mf->numPoints = MAX_VERTS_ON_POLY;
		}
		for ( j = 0, v = verts ; j < mf->numPoints ; j++, v++ ) {
			vec3_t		delta;

			VectorCopy( markPoints[mf->firstPoint + j], v->xyz );

			VectorSubtract( v->xyz, origin, delta );
			v->st[0] = 0.5 + DotProduct( delta, axis[1] ) * texCoordScale;
			v->st[1] = 0.5 + DotProduct( delta, axis[2] ) * texCoordScale;
			*(int *)v->modulate = *(int *)colors;
		}

		// if it is a temporary (shadow) mark, add it immediately and forget about it
		if ( temporary ) {
			trap_R_AddPolyToScene( markShader, mf->numPoints, verts );
			continue;
		}

		// otherwise save it persistantly
		mark = CG_AllocMark();
		mark->time = cg.time;
		mark->alphaFade = alphaFade;
		mark->markShader = markShader;
		mark->poly.numVerts = mf->numPoints;
		mark->color[0] = red;
		mark->color[1] = green;
		mark->color[2] = blue;
		mark->color[3] = alpha;
		memcpy( mark->verts, verts, mf->numPoints * sizeof( verts[0] ) );
	}
}
// Q3Rally Code END


/*
===============
CG_SkidRibbon

Q3Rally: the skid mark of a tyre as one continuous ribbon. Called every
frame while the tyre skids at the contact point; a segment is laid down
from the last point once the tyre has moved SKID_SEGMENT units, sharing
its edge with the previous segment and continuing the texture along the
mark, so curves have no gaps or overlaps. Stops when the tyre stops
skidding, lifts off or the ground changes (CG_SkidRibbonStop).
===============
*/
#define SKID_HALF_WIDTH		7.0f
#define SKID_SEGMENT		14.0f
#define SKID_RUT_SEGMENT	40.0f
#define SKID_SNOW_STRENGTH	0.6f	// how dark a skid mark in snow is (1 = full)
#define SKID_RUT_STRENGTH	0.4f	// how dark a rolling rut in snow is
#define SKID_TEX_LENGTH		96.0f	// units per texture repeat along the mark
#define SKID_LIFETIME		20000
#define SKID_MAX_GAP		96.0f
#define SKID_GRACE_TIME		150		// msec

void CG_SkidRibbonStop( centity_t *cent, int tire ) {
	// the slip of a tyre flickers from frame to frame: short breaks do not
	// end the mark
	if ( cg.time - cent->skidRibbonTime[tire] > SKID_GRACE_TIME || cg.time < cent->skidRibbonTime[tire] ) {
		cent->skidRibbon[tire] = qfalse;
	}
}

void CG_SkidRibbon( centity_t *cent, int tire, qhandle_t shader, const vec3_t point, const vec3_t normal ) {
	vec3_t			move, side, side0, segDir, corners[4], projection, delta, c, sd;
	vec3_t			markPoints[MAX_MARK_POINTS];
	markFragment_t	markFragments[MAX_MARK_FRAGMENTS], *mf;
	polyVert_t		verts[MAX_VERTS_ON_POLY];
	markPoly_t		*mark;
	float			segLen, u, dist0, strength;
	int				i, j, numFragments;
	byte			shade;

	if ( !cg_addMarks.integer || shader <= 0 ) {
		cent->skidRibbon[tire] = qfalse;
		return;
	}
	cent->skidRibbonTime[tire] = cg.time;

	// start a new ribbon
	if ( !cent->skidRibbon[tire] || cent->skidRibbonShader[tire] != shader ) {
		cent->skidRibbon[tire] = qtrue;
		cent->skidRibbonShader[tire] = shader;
		VectorCopy( point, cent->skidRibbonPoint[tire] );
		VectorClear( cent->skidRibbonSide[tire] );
		cent->skidRibbonDist[tire] = random() * SKID_TEX_LENGTH;
		return;
	}

	VectorSubtract( point, cent->skidRibbonPoint[tire], move );
	segLen = VectorLength( move );
	if ( segLen > SKID_MAX_GAP ) {
		// jumped, teleported or lost the ground for a while
		VectorCopy( point, cent->skidRibbonPoint[tire] );
		VectorClear( cent->skidRibbonSide[tire] );
		return;
	}
	// (rolling ruts in snow are long and straight: longer segments, so
	// they do not use up the marks)
	if ( segLen < ( cent->wheelSkidding[tire] ? SKID_SEGMENT : SKID_RUT_SEGMENT ) ) {
		return;
	}

	// across the way the tyre moves, on the ground; keep it pointing to the
	// same side as the last segment so the ribbon does not twist
	VectorScale( move, 1.0f / segLen, segDir );
	CrossProduct( segDir, normal, side );
	if ( VectorNormalize( side ) < 0.001f ) {
		return;
	}
	if ( VectorLengthSquared( cent->skidRibbonSide[tire] ) < 0.5f ) {
		VectorCopy( side, cent->skidRibbonSide[tire] );
	} else if ( DotProduct( side, cent->skidRibbonSide[tire] ) < 0.0f ) {
		VectorNegate( side, side );
	}
	VectorCopy( cent->skidRibbonSide[tire], side0 );

	// the segment, a little above the ground and projected onto it
	// (R_MarkFragments needs the corners clockwise seen from above)
	VectorMA( cent->skidRibbonPoint[tire], -SKID_HALF_WIDTH, side0, corners[0] );
	VectorMA( cent->skidRibbonPoint[tire], SKID_HALF_WIDTH, side0, corners[1] );
	VectorMA( point, SKID_HALF_WIDTH, side, corners[2] );
	VectorMA( point, -SKID_HALF_WIDTH, side, corners[3] );
	for ( i = 0; i < 4; i++ ) {
		VectorMA( corners[i], 6.0f, normal, corners[i] );
	}
	VectorSubtract( corners[1], corners[0], move );
	VectorSubtract( corners[2], corners[1], delta );
	CrossProduct( move, delta, c );
	if ( DotProduct( c, normal ) > 0.0f ) {
		// counter-clockwise seen from above: turn it round
		VectorCopy( corners[1], c );
		VectorCopy( corners[3], corners[1] );
		VectorCopy( c, corners[3] );
	}
	VectorScale( normal, -20.0f, projection );

	numFragments = trap_CM_MarkFragments( 4, (void *)corners, projection, MAX_MARK_POINTS, markPoints[0],
		MAX_MARK_FRAGMENTS, markFragments );

	// in snow the marks are shallower: a skid lighter, a rolling rut
	// lighter still
	strength = 1.0f;
	if ( shader == cgs.media.SMSnowShader ) {
		strength = cent->wheelSkidding[tire] ? SKID_SNOW_STRENGTH : SKID_RUT_STRENGTH;
	}
	shade = (byte)( 255.0f * strength );

	dist0 = cent->skidRibbonDist[tire];
	for ( i = 0, mf = markFragments; i < numFragments; i++, mf++ ) {
		if ( mf->numPoints > MAX_VERTS_ON_POLY ) {
			mf->numPoints = MAX_VERTS_ON_POLY;
		}
		for ( j = 0; j < mf->numPoints; j++ ) {
			polyVert_t *v = &verts[j];

			VectorCopy( markPoints[mf->firstPoint + j], v->xyz );

			// where along the segment, and how far across the ribbon
			VectorSubtract( v->xyz, cent->skidRibbonPoint[tire], delta );
			u = Com_Clamp( 0.0f, 1.0f, DotProduct( delta, segDir ) / segLen );
			VectorMA( cent->skidRibbonPoint[tire], u * segLen, segDir, c );
			VectorScale( side0, 1.0f - u, sd );
			VectorMA( sd, u, side, sd );
			VectorNormalize( sd );
			VectorSubtract( v->xyz, c, delta );

			v->st[0] = 0.5f + DotProduct( delta, sd ) / ( 2.0f * SKID_HALF_WIDTH );
			v->st[1] = ( dist0 + u * segLen ) / SKID_TEX_LENGTH;
			v->modulate[0] = v->modulate[1] = v->modulate[2] = shade;
			v->modulate[3] = 255;
		}

		mark = CG_AllocMark();
		mark->time = cg.time;
		mark->lifeTime = SKID_LIFETIME;
		mark->alphaFade = qfalse;
		mark->markShader = shader;
		mark->poly.numVerts = mf->numPoints;
		mark->color[0] = mark->color[1] = mark->color[2] = strength;
		mark->color[3] = 1.0f;
		memcpy( mark->verts, verts, mf->numPoints * sizeof( verts[0] ) );
	}

	VectorCopy( point, cent->skidRibbonPoint[tire] );
	VectorCopy( side, cent->skidRibbonSide[tire] );
	cent->skidRibbonDist[tire] = dist0 + segLen;
	if ( cent->skidRibbonDist[tire] > SKID_TEX_LENGTH * 64.0f ) {
		cent->skidRibbonDist[tire] -= SKID_TEX_LENGTH * 64.0f;	// keep the texture coordinates small
	}
}

/*
===============
CG_AddMarks
===============
*/
#define	MARK_TOTAL_TIME		10000
#define	MARK_FADE_TIME		1000
#define	SKID_FADE_TIME		4000	// Q3Rally: skid marks fade out slowly
// Q3Rally Code Start
#define MARK_GROW_TIME		1000
// Q3Rally Code END

void CG_AddMarks( void ) {
	int			j;
	markPoly_t	*mp, *next;
	int			t;
	int			fade;
	int			life, fadeTime;

	if ( !cg_addMarks.integer ) {
		return;
	}

	mp = cg_activeMarkPolys.nextMark;
	for ( ; mp != &cg_activeMarkPolys ; mp = next ) {
		// grab next now, so if the local entity is freed we
		// still have it
		next = mp->nextMark;

		// see if it is time to completely remove it
		life = mp->lifeTime ? mp->lifeTime : MARK_TOTAL_TIME;
		if ( cg.time > mp->time + life ) {
			CG_FreeMarkPoly( mp );
			continue;
		}

		// fade out the energy bursts
		if ( mp->markShader == cgs.media.energyMarkShader ) {

			fade = 450 - 450 * ( (cg.time - mp->time ) / 3000.0 );
			if ( fade < 255 ) {
				if ( fade < 0 ) {
					fade = 0;
				}
				if ( mp->verts[0].modulate[0] != 0 ) {
					for ( j = 0 ; j < mp->poly.numVerts ; j++ ) {
						mp->verts[j].modulate[0] = mp->color[0] * fade;
						mp->verts[j].modulate[1] = mp->color[1] * fade;
						mp->verts[j].modulate[2] = mp->color[2] * fade;
					}
				}
			}
		}

		// fade all marks out with time (Q3Rally: skid marks more slowly)
		t = mp->time + life - cg.time;
		fadeTime = mp->lifeTime ? SKID_FADE_TIME : MARK_FADE_TIME;
		if ( t < fadeTime ) {
			fade = 255 * t / fadeTime;
			if ( mp->alphaFade ) {
				for ( j = 0 ; j < mp->poly.numVerts ; j++ ) {
					mp->verts[j].modulate[3] = fade;
				}
			} else {
				for ( j = 0 ; j < mp->poly.numVerts ; j++ ) {
					mp->verts[j].modulate[0] = mp->color[0] * fade;
					mp->verts[j].modulate[1] = mp->color[1] * fade;
					mp->verts[j].modulate[2] = mp->color[2] * fade;
				}
			}
		}

// Q3Rally Code Start
/* UPDATE: enable this
		if (mp->markShader == cgs.media.oilMarkShader || 
			mp->markShader == cgs.media.bioMarkShader){
			t = MARK_TOTAL_TIME - (mp->time + MARK_TOTAL_TIME - cg.time);
			if ( t < MARK_GROW_TIME ) {
				fade = 255 * t / MARK_GROW_TIME;

				for ( j = 0 ; j < mp->poly.numVerts ; j++ ) {
					mp->verts[j].modulate[0] = mp->color[0] * fade;
					mp->verts[j].modulate[1] = mp->color[1] * fade;
					mp->verts[j].modulate[2] = mp->color[2] * fade;
					mp->verts[j].modulate[3] = fade;
				}
			}
		}
*/
// Q3Rally Code END

		trap_R_AddPolyToScene( mp->markShader, mp->poly.numVerts, mp->verts );
	}
}

