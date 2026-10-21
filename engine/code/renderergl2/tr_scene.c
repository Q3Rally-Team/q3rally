/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "tr_local.h"

int			r_firstSceneDrawSurf;

int			r_numdlights;
int			r_firstSceneDlight;

int			r_numentities;
int			r_firstSceneEntity;

int			r_numpolys;
int			r_firstScenePoly;

int			r_numpolyverts;


/*
====================
R_InitNextFrame

====================
*/
void R_InitNextFrame( void ) {
	backEndData->commands.used = 0;

	r_firstSceneDrawSurf = 0;

	r_numdlights = 0;
	r_firstSceneDlight = 0;

	r_numentities = 0;
	r_firstSceneEntity = 0;

	r_numpolys = 0;
	r_firstScenePoly = 0;

	r_numpolyverts = 0;
}


/*
====================
RE_ClearScene

====================
*/
void RE_ClearScene( void ) {
	r_firstSceneDlight = r_numdlights;
	r_firstSceneEntity = r_numentities;
	r_firstScenePoly = r_numpolys;
}

/*
===========================================================================

DISCRETE POLYS

===========================================================================
*/

/*
=====================
R_AddPolygonSurfaces

Adds all the scene's polys into this view's drawsurf list
=====================
*/
void R_AddPolygonSurfaces( void ) {
	int			i;
	shader_t	*sh;
	srfPoly_t	*poly;
	int		fogMask;

	tr.currentEntityNum = REFENTITYNUM_WORLD;
	tr.shiftedEntityNum = tr.currentEntityNum << QSORT_REFENTITYNUM_SHIFT;
	fogMask = -((tr.refdef.rdflags & RDF_NOFOG) == 0);

	for ( i = 0, poly = tr.refdef.polys; i < tr.refdef.numPolys ; i++, poly++ ) {
		sh = R_GetShaderByHandle( poly->hShader );
		R_AddDrawSurf( ( void * )poly, sh, poly->fogIndex & fogMask, qfalse, qfalse, 0 /*cubeMap*/  );
	}
}

/*
=====================
RE_AddPolyToScene

=====================
*/
void RE_AddPolyToScene( qhandle_t hShader, int numVerts, const polyVert_t *verts, int numPolys ) {
	srfPoly_t	*poly;
	int			i, j;
	int			fogIndex;
	fog_t		*fog;
	vec3_t		bounds[2];

	if ( !tr.registered ) {
		return;
	}

	if ( !hShader ) {
		// This isn't a useful warning, and an hShader of zero isn't a null shader, it's
		// the default shader.
		//ri.Printf( PRINT_WARNING, "WARNING: RE_AddPolyToScene: NULL poly shader\n");
		//return;
	}

	for ( j = 0; j < numPolys; j++ ) {
		if ( r_numpolyverts + numVerts > max_polyverts || r_numpolys >= max_polys ) {
      /*
      NOTE TTimo this was initially a PRINT_WARNING
      but it happens a lot with high fighting scenes and particles
      since we don't plan on changing the const and making for room for those effects
      simply cut this message to developer only
      */
			ri.Printf( PRINT_DEVELOPER, "WARNING: RE_AddPolyToScene: r_max_polys or r_max_polyverts reached\n");
			return;
		}

		poly = &backEndData->polys[r_numpolys];
		poly->surfaceType = SF_POLY;
		poly->hShader = hShader;
		poly->numVerts = numVerts;
		poly->verts = &backEndData->polyVerts[r_numpolyverts];
		
		Com_Memcpy( poly->verts, &verts[numVerts*j], numVerts * sizeof( *verts ) );

		if ( glConfig.hardwareType == GLHW_RAGEPRO ) {
			poly->verts->modulate[0] = 255;
			poly->verts->modulate[1] = 255;
			poly->verts->modulate[2] = 255;
			poly->verts->modulate[3] = 255;
		}
		// done.
		r_numpolys++;
		r_numpolyverts += numVerts;

		// if no world is loaded
		if ( tr.world == NULL ) {
			fogIndex = 0;
		}
		// see if it is in a fog volume
		else if ( tr.world->numfogs == 1 ) {
			fogIndex = 0;
		} else {
			// find which fog volume the poly is in
			VectorCopy( poly->verts[0].xyz, bounds[0] );
			VectorCopy( poly->verts[0].xyz, bounds[1] );
			for ( i = 1 ; i < poly->numVerts ; i++ ) {
				AddPointToBounds( poly->verts[i].xyz, bounds[0], bounds[1] );
			}
			for ( fogIndex = 1 ; fogIndex < tr.world->numfogs ; fogIndex++ ) {
				fog = &tr.world->fogs[fogIndex]; 
				if ( bounds[1][0] >= fog->bounds[0][0]
					&& bounds[1][1] >= fog->bounds[0][1]
					&& bounds[1][2] >= fog->bounds[0][2]
					&& bounds[0][0] <= fog->bounds[1][0]
					&& bounds[0][1] <= fog->bounds[1][1]
					&& bounds[0][2] <= fog->bounds[1][2] ) {
					break;
				}
			}
			if ( fogIndex == tr.world->numfogs ) {
				fogIndex = 0;
			}
		}
		poly->fogIndex = fogIndex;
	}
}


//=================================================================================


/*
=====================
RE_AddRefEntityToScene

=====================
*/
void RE_AddRefEntityToScene( const refEntity_t *ent ) {
	vec3_t cross;

	if ( !tr.registered ) {
		return;
	}
	if ( r_numentities >= MAX_REFENTITIES ) {
		ri.Printf(PRINT_DEVELOPER, "RE_AddRefEntityToScene: Dropping refEntity, reached MAX_REFENTITIES\n");
		return;
	}
	if ( Q_isnan(ent->origin[0]) || Q_isnan(ent->origin[1]) || Q_isnan(ent->origin[2]) ) {
		static qboolean firstTime = qtrue;
		if (firstTime) {
			firstTime = qfalse;
			ri.Printf( PRINT_WARNING, "RE_AddRefEntityToScene passed a refEntity which has an origin with a NaN component\n");
		}
		return;
	}
	if ( (int)ent->reType < 0 || ent->reType >= RT_MAX_REF_ENTITY_TYPE ) {
		ri.Error( ERR_DROP, "RE_AddRefEntityToScene: bad reType %i", ent->reType );
	}

	backEndData->entities[r_numentities].e = *ent;
	backEndData->entities[r_numentities].lightingCalculated = qfalse;

	CrossProduct(ent->axis[0], ent->axis[1], cross);
	backEndData->entities[r_numentities].mirrored = (DotProduct(ent->axis[2], cross) < 0.f);

	r_numentities++;
}


/*
=====================
RE_AddDynamicLightToScene

=====================
*/
void RE_AddDynamicLightToScene( const vec3_t org, float intensity, float r, float g, float b, int additive ) {
	dlight_t	*dl;

	if ( !tr.registered ) {
		return;
	}
	if ( r_numdlights >= MAX_DLIGHTS ) {
		return;
	}
	if ( intensity <= 0 ) {
		return;
	}
	// these cards don't have the correct blend mode
	if ( glConfig.hardwareType == GLHW_RIVA128 || glConfig.hardwareType == GLHW_PERMEDIA2 ) {
		return;
	}
	dl = &backEndData->dlights[r_numdlights++];
	VectorCopy (org, dl->origin);
	dl->radius = intensity;
	dl->color[0] = r;
	dl->color[1] = g;
	dl->color[2] = b;
	dl->additive = additive;
	dl->spotCosOuter = -2.0f;
	dl->spotShadow = -1;
	dl->spotFlags = 0;
}

/*
=====================
RE_AddSpotLightToScene

Q3Rally: a spot light (car head, tail, brake and reverse lights). Lit like a dlight with the surface
colours (an extra pass per surface, tr_shade.c), cone shaped, and the
nearest ones cast shadows (not with SPOTLIGHT_NOSHADOW). With r_headlights 0
it falls back to additive point lights, the look of the old lights.
=====================
*/
/*
=====================
RE_SetWeatherGrid

Q3Rally: the sky grid of the weather (bg_weather.c) as a texture: the
height of the highest surface seen from the sky, + 32768, in red (high
byte) and green (low byte). The wet ground pass reads it texel by texel.
=====================
*/
void RE_SetWeatherGrid( const short *heights, int width, int height, const float *params ) {
	byte *pic;
	int i, v;

	tr.weatherGridImage = NULL;
	VectorSet4( tr.weatherGrid, 0.0f, 0.0f, 0.0f, 0.0f );

	if ( !heights || !params || width <= 0 || height <= 0 || params[2] <= 0.0f ) {
		return;
	}

	pic = ri.Hunk_AllocateTempMemory( width * height * 4 );
	for ( i = 0; i < width * height; i++ ) {
		v = heights[i] + 32768;
		pic[i * 4 + 0] = ( v >> 8 ) & 255;
		pic[i * 4 + 1] = v & 255;
		pic[i * 4 + 2] = 255;
		pic[i * 4 + 3] = 255;
	}

	tr.weatherGridImage = R_CreateImage( "*weathergrid", pic, width, height, IMGTYPE_COLORALPHA,
		IMGFLAG_NO_COMPRESSION | IMGFLAG_NOLIGHTSCALE | IMGFLAG_CLAMPTOEDGE, GL_RGBA8 );
	ri.Hunk_FreeTempMemory( pic );

	VectorSet4( tr.weatherGrid, params[0], params[1], 1.0f / params[2], 1.0f );
	VectorSet4( tr.weatherGridSize, width, height, 1.0f / width, 1.0f / height );
}

void RE_AddSpotLightToScene( const vec3_t org, const vec3_t dir, float radius, float coneAngle, float r, float g, float b, int flags ) {
	dlight_t	*dl;
	vec3_t		d, pos;
	float		half;

	if ( !tr.registered || radius <= 0.0f ) {
		return;
	}

	VectorCopy( dir, d );
	if ( VectorNormalize( d ) < 0.001f ) {
		return;
	}

	if ( !r_headlights->integer || !tr.spotlightShader.program ) {
		if ( radius < 1000.0f ) {
			// short lights (tail, brake, reverse): one point light behind the car
			VectorMA( org, 40, d, pos );
			RE_AddDynamicLightToScene( pos, radius * 0.22f, r * 2.0f, g * 2.0f, b * 2.0f, qtrue );
			return;
		}
		VectorMA( org, 50, d, pos );
		RE_AddDynamicLightToScene( pos, 100, r * 1.2f, g * 1.2f, b * 1.2f, qtrue );
		VectorMA( org, 150, d, pos );
		RE_AddDynamicLightToScene( pos, 200, r * 0.8f, g * 0.8f, b * 0.8f, qtrue );
		VectorMA( org, 250, d, pos );
		RE_AddDynamicLightToScene( pos, 300, r * 0.5f, g * 0.5f, b * 0.5f, qtrue );
		return;
	}

	if ( r_numdlights >= MAX_DLIGHTS ) {
		return;
	}

	dl = &backEndData->dlights[r_numdlights++];
	VectorCopy( org, dl->origin );
	dl->radius = radius;
	dl->color[0] = r;
	dl->color[1] = g;
	dl->color[2] = b;
	dl->additive = qfalse;

	half = Com_Clamp( 5.0f, 85.0f, coneAngle * 0.5f );
	VectorCopy( d, dl->spotDir );
	dl->spotCosOuter = cos( DEG2RAD( half ) );
	dl->spotCosInner = cos( DEG2RAD( half * 0.55f ) );
	dl->spotShadow = -1;
	dl->spotFlags = flags;
}

/*
=====================
RE_AddLightToScene

=====================
*/
void RE_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b ) {
	RE_AddDynamicLightToScene( org, intensity, r, g, b, qfalse );
}

/*
=====================
RE_AddAdditiveLightToScene

=====================
*/
void RE_AddAdditiveLightToScene( const vec3_t org, float intensity, float r, float g, float b ) {
	RE_AddDynamicLightToScene( org, intensity, r, g, b, qtrue );
}


void RE_BeginScene(const refdef_t *fd)
{
	Com_Memcpy( tr.refdef.text, fd->text, sizeof( tr.refdef.text ) );

	tr.refdef.x = fd->x;
	tr.refdef.y = fd->y;
	tr.refdef.width = fd->width;
	tr.refdef.height = fd->height;
	tr.refdef.fov_x = fd->fov_x;
	tr.refdef.fov_y = fd->fov_y;

	VectorCopy( fd->vieworg, tr.refdef.vieworg );
	VectorCopy( fd->viewaxis[0], tr.refdef.viewaxis[0] );
	VectorCopy( fd->viewaxis[1], tr.refdef.viewaxis[1] );
	VectorCopy( fd->viewaxis[2], tr.refdef.viewaxis[2] );

	tr.refdef.time = fd->time;
	tr.refdef.rdflags = fd->rdflags;

	// Q3Rally: wet ground
	tr.refdef.wetness = r_wetness->value >= 0.0f ? r_wetness->value : fd->wetness;
	if ( !( tr.refdef.wetness > 0.0f ) ) {		// also catches NaN from an old cgame
		tr.refdef.wetness = 0.0f;
	} else if ( tr.refdef.wetness > 1.0f ) {
		tr.refdef.wetness = 1.0f;
	}

	// the weather where the sky is (r_wetness tests the whole map)
	{
		int i;

		for ( i = 0; i < 4; i++ ) {
			float v = fd->weather[i];
			tr.refdef.weather[i] = ( r_wetness->value < 0.0f && v > 0.0f ) ? MIN( v, 1.0f ) : 0.0f;	// also NaN
		}
		if ( !tr.weatherGridImage ) {
			tr.refdef.weather[0] = tr.refdef.weather[1] = tr.refdef.weather[2] = tr.refdef.weather[3] = 0.0f;
		}

		// the sky clouds over with the rain or snow (and clears with it)
		tr.refdef.overcast = MIN( 1.0f, MAX( tr.refdef.weather[2], tr.refdef.weather[3] ) * 1.15f );
		tr.refdef.overcastSnow = tr.refdef.weather[3] > tr.refdef.weather[2] ? 1.0f : 0.0f;
	}

	// copy the areamask data over and note if it has changed, which
	// will force a reset of the visible leafs even if the view hasn't moved
	tr.refdef.areamaskModified = qfalse;
	if ( ! (tr.refdef.rdflags & RDF_NOWORLDMODEL) ) {
		int		areaDiff;
		int		i;

		// compare the area bits
		areaDiff = 0;
		for (i = 0 ; i < MAX_MAP_AREA_BYTES/4 ; i++) {
			areaDiff |= ((int *)tr.refdef.areamask)[i] ^ ((int *)fd->areamask)[i];
			((int *)tr.refdef.areamask)[i] = ((int *)fd->areamask)[i];
		}

		if ( areaDiff ) {
			// a door just opened or something
			tr.refdef.areamaskModified = qtrue;
		}
	}

	tr.refdef.sunDir[3] = 0.0f;
	tr.refdef.sunCol[3] = 1.0f;
	tr.refdef.sunAmbCol[3] = 1.0f;

	VectorCopy(tr.sunDirection, tr.refdef.sunDir);
	if ( (tr.refdef.rdflags & RDF_NOWORLDMODEL) || !(r_depthPrepass->value) ){
		VectorSet(tr.refdef.sunCol, 0, 0, 0);
		VectorSet(tr.refdef.sunAmbCol, 0, 0, 0);
	}
	else
	{
		float scale = (1 << r_mapOverBrightBits->integer) / 255.0f;

		if (r_forceSun->integer)
			VectorScale(tr.sunLight, scale * r_forceSunLightScale->value, tr.refdef.sunCol);
		else
			VectorScale(tr.sunLight, scale, tr.refdef.sunCol);

		if (r_sunlightMode->integer == 1)
		{
			tr.refdef.sunAmbCol[0] =
			tr.refdef.sunAmbCol[1] =
			tr.refdef.sunAmbCol[2] = r_forceSun->integer ? r_forceSunAmbientScale->value : tr.sunShadowScale;
		}
		else
		{
			if (r_forceSun->integer)
				VectorScale(tr.sunLight, scale * r_forceSunAmbientScale->value, tr.refdef.sunAmbCol);
			else
				VectorScale(tr.sunLight, scale * tr.sunShadowScale, tr.refdef.sunAmbCol);
		}

		// Q3Rally: under clouds the sun is weak and its shadows fade (the
		// sun in the lightmap stays; the tone mapping darkens the scene)
		if (tr.refdef.overcast > 0.0f)
		{
			float c = tr.refdef.overcast;

			VectorScale(tr.refdef.sunCol, 1.0f - 0.75f * c, tr.refdef.sunCol);
			if (r_sunlightMode->integer == 1)
			{
				float amb = tr.refdef.sunAmbCol[0] + (1.0f - tr.refdef.sunAmbCol[0]) * 0.8f * c;

				VectorSet(tr.refdef.sunAmbCol, amb, amb, amb);
			}
		}
	}

	if (r_forceAutoExposure->integer)
	{
		tr.refdef.autoExposureMinMax[0] = r_forceAutoExposureMin->value;
		tr.refdef.autoExposureMinMax[1] = r_forceAutoExposureMax->value;
	}
	else
	{
		tr.refdef.autoExposureMinMax[0] = tr.autoExposureMinMax[0];
		tr.refdef.autoExposureMinMax[1] = tr.autoExposureMinMax[1];
	}

	if (r_forceToneMap->integer)
	{
		tr.refdef.toneMinAvgMaxLinear[0] = pow(2, r_forceToneMapMin->value);
		tr.refdef.toneMinAvgMaxLinear[1] = pow(2, r_forceToneMapAvg->value);
		tr.refdef.toneMinAvgMaxLinear[2] = pow(2, r_forceToneMapMax->value);
	}
	else
	{
		tr.refdef.toneMinAvgMaxLinear[0] = pow(2, tr.toneMinAvgMaxLevel[0]);
		tr.refdef.toneMinAvgMaxLinear[1] = pow(2, tr.toneMinAvgMaxLevel[1]);
		tr.refdef.toneMinAvgMaxLinear[2] = pow(2, tr.toneMinAvgMaxLevel[2]);
	}

	// Makro - copy exta info if present
	if (fd->rdflags & RDF_EXTRA) {
		const refdefex_t* extra = (const refdefex_t*) (fd+1);

		tr.refdef.blurFactor = extra->blurFactor;

		if (fd->rdflags & RDF_SUNLIGHT)
		{
			VectorCopy(extra->sunDir,    tr.refdef.sunDir);
			VectorCopy(extra->sunCol,    tr.refdef.sunCol);
			VectorCopy(extra->sunAmbCol, tr.refdef.sunAmbCol);
		}
	} 
	else
	{
		tr.refdef.blurFactor = 0.0f;
	}

	// derived info

	tr.refdef.floatTime = tr.refdef.time * 0.001;

	tr.refdef.numDrawSurfs = r_firstSceneDrawSurf;
	tr.refdef.drawSurfs = backEndData->drawSurfs;

	tr.refdef.num_entities = r_numentities - r_firstSceneEntity;
	tr.refdef.entities = &backEndData->entities[r_firstSceneEntity];

	tr.refdef.num_dlights = r_numdlights - r_firstSceneDlight;
	tr.refdef.dlights = &backEndData->dlights[r_firstSceneDlight];

	tr.refdef.numPolys = r_numpolys - r_firstScenePoly;
	tr.refdef.polys = &backEndData->polys[r_firstScenePoly];

	tr.refdef.num_pshadows = 0;
	tr.refdef.pshadows = &backEndData->pshadows[0];

	// turn off dynamic lighting globally by clearing all the
	// dlights if it needs to be disabled or if vertex lighting is enabled
	if ( r_dynamiclight->integer == 0 ||
		 r_vertexLight->integer == 1 ||
		 glConfig.hardwareType == GLHW_PERMEDIA2 ) {
		tr.refdef.num_dlights = 0;
	}

	// a single frame may have multiple scenes draw inside it --
	// a 3D game view, 3D status bar renderings, 3D menus, etc.
	// They need to be distinguished by the light flare code, because
	// the visibility state for a given surface may be different in
	// each scene / view.
	tr.frameSceneNum++;
	tr.sceneCount++;
}


void RE_EndScene(void)
{
	// the next scene rendered in this frame will tack on after this one
	r_firstSceneDrawSurf = tr.refdef.numDrawSurfs;
	r_firstSceneEntity = r_numentities;
	r_firstSceneDlight = r_numdlights;
	r_firstScenePoly = r_numpolys;
}

/*
@@@@@@@@@@@@@@@@@@@@@
RE_RenderScene

Draw a 3D view into a part of the window, then return
to 2D drawing.

Rendering a scene may require multiple views to be rendered
to handle mirrors,
@@@@@@@@@@@@@@@@@@@@@
*/
/*
=================
R_UpdateColorLut

Q3Rally: the colour grading LUT for the tone mapping: r_colorLut, else the
map's (worldspawn "colorlut", else q3r_colorLut in the sky shader), else
the default look. A LUT is a strip of N slices of N x N (N*N wide, N high):
red across a slice, green down, blue from slice to slice.
=================
*/
#define DEFAULT_COLOR_LUT	"gfx/luts/q3r_default"

void R_UpdateColorLut( void ) {
	const char *name;
	image_t *image;

	if ( r_colorLut->string[0] ) {
		name = r_colorLut->string;
	} else if ( tr.worldColorLut[0] ) {
		name = tr.worldColorLut;
	} else if ( tr.shaderColorLut[0] ) {
		name = tr.shaderColorLut;
	} else {
		name = DEFAULT_COLOR_LUT;
	}

	if ( !Q_stricmp( name, tr.colorLutName ) ) {
		return;
	}
	Q_strncpyz( tr.colorLutName, name, sizeof( tr.colorLutName ) );
	tr.colorLutImage = NULL;

	if ( !Q_stricmp( name, "none" ) ) {
		return;
	}

	image = R_FindImageFile( name, IMGTYPE_COLORALPHA, IMGFLAG_NOLIGHTSCALE | IMGFLAG_NO_COMPRESSION | IMGFLAG_CLAMPTOEDGE );
	if ( !image ) {
		ri.Printf( PRINT_WARNING, "WARNING: colour grading LUT '%s' not found\n", name );
		return;
	}
	if ( image->height < 2 || image->height > 64 || image->width != image->height * image->height ) {
		ri.Printf( PRINT_WARNING, "WARNING: colour grading LUT '%s' is %ix%i, needs N*N x N\n", name, image->width, image->height );
		return;
	}

	tr.colorLutImage = image;
	ri.Printf( PRINT_DEVELOPER, "colour grading LUT: %s\n", name );
}

void RE_RenderScene( const refdef_t *fd ) {
	viewParms_t		parms;
	int				startTime;

	if ( !tr.registered ) {
		return;
	}
	GLimp_LogComment( "====== RE_RenderScene =====\n" );

	if ( r_norefresh->integer ) {
		return;
	}

	if ( tr.world ) {
		R_UpdateColorLut();		// Q3Rally
	}

	startTime = ri.Milliseconds();

	if (!tr.world && !( fd->rdflags & RDF_NOWORLDMODEL ) ) {
		ri.Error (ERR_DROP, "R_RenderScene: NULL worldmodel");
	}

	RE_BeginScene(fd);

	// SmileTheory: playing with shadow mapping
	if (!( fd->rdflags & RDF_NOWORLDMODEL ) && tr.refdef.num_dlights && r_dlightMode->integer >= 2)
	{
		R_RenderDlightCubemaps(fd);
	}

	/* playing with more shadows */
	if(glRefConfig.framebufferObject && !( fd->rdflags & RDF_NOWORLDMODEL ) && r_shadows->integer == 4)
	{
		R_RenderPshadowMaps(fd);
	}

	// playing with even more shadows
	if(glRefConfig.framebufferObject && r_sunlightMode->integer && !( fd->rdflags & RDF_NOWORLDMODEL ) && (r_forceSun->integer || tr.sunShadows))
	{
		if (r_shadowCascadeZFar->integer != 0)
		{
			R_RenderSunShadowMaps(fd, 0);
			R_RenderSunShadowMaps(fd, 1);
			R_RenderSunShadowMaps(fd, 2);
		}
		else
		{
			Mat4Zero(tr.refdef.sunShadowMvp[0]);
			Mat4Zero(tr.refdef.sunShadowMvp[1]);
			Mat4Zero(tr.refdef.sunShadowMvp[2]);
		}

		// only rerender last cascade if sun has changed position
		if (r_forceSun->integer == 2 || !VectorCompare(tr.refdef.sunDir, tr.lastCascadeSunDirection))
		{
			VectorCopy(tr.refdef.sunDir, tr.lastCascadeSunDirection);
			R_RenderSunShadowMaps(fd, 3);
			Mat4Copy(tr.refdef.sunShadowMvp[3], tr.lastCascadeSunMvp);
		}
		else
		{
			Mat4Copy(tr.lastCascadeSunMvp, tr.refdef.sunShadowMvp[3]);
		}
	}

	// Q3Rally: spot lights (headlights): which ones they are, and the shadow
	// maps of the nearest
	{
		int i;

		tr.refdef.spotMask = 0;
		for (i = 0; i < tr.refdef.num_dlights && i < 32; i++)
		{
			tr.refdef.dlights[i].spotShadow = -1;
			if (tr.refdef.dlights[i].spotCosOuter > -1.0f)
				tr.refdef.spotMask |= 1 << i;
		}

		if (tr.refdef.spotMask && !( fd->rdflags & RDF_NOWORLDMODEL ))
			R_RenderSpotShadowMaps();
	}

	// Q3Rally: the cube map the car paint reflects (before the lamp shadows,
	// it renders without them)
	R_RenderCarReflection();

	// Q3Rally: lamps of the map cast model shadows
	R_SelectLampShadows();
	if (tr.refdef.numLampShadows)
	{
		R_RenderLampShadowMaps();
	}

	// playing with cube maps
	// this is where dynamic cubemaps would be rendered
	// Q3Rally: the former r_dynamicReflections path was removed here. It
	// rendered six nested scenes per frame (dropping the main view's entities)
	// and its result never reached the $dynamicreflection stages. A
	// time-sliced subscene cubemap will replace it.
	if (0) //(glRefConfig.framebufferObject && !( fd->rdflags & RDF_NOWORLDMODEL ))
	{
		int i, j;

		for (i = 0; i < tr.numCubemaps; i++)
		{
			for (j = 0; j < 6; j++)
			{
				R_RenderCubemapSide(i, j, qtrue);
			}
		}
	}

	// setup view parms for the initial view
	//
	// set up viewport
	// The refdef takes 0-at-the-top y coordinates, so
	// convert to GL's 0-at-the-bottom space
	//
	Com_Memset( &parms, 0, sizeof( parms ) );
	parms.viewportX = tr.refdef.x;
	parms.viewportY = glConfig.vidHeight - ( tr.refdef.y + tr.refdef.height );
	parms.viewportWidth = tr.refdef.width;
	parms.viewportHeight = tr.refdef.height;
	parms.isPortal = qfalse;

	parms.fovX = tr.refdef.fov_x;
	parms.fovY = tr.refdef.fov_y;
	
	parms.stereoFrame = tr.refdef.stereoFrame;

	VectorCopy( fd->vieworg, parms.or.origin );
	VectorCopy( fd->viewaxis[0], parms.or.axis[0] );
	VectorCopy( fd->viewaxis[1], parms.or.axis[1] );
	VectorCopy( fd->viewaxis[2], parms.or.axis[2] );

	VectorCopy( fd->vieworg, parms.pvsOrigin );

	if(!( fd->rdflags & RDF_NOWORLDMODEL ) && r_depthPrepass->value && ((r_forceSun->integer) || tr.sunShadows || tr.refdef.numLampShadows))
	{
		parms.flags = VPF_USESUNLIGHT;
	}

	R_RenderView( &parms );

	// Q3Rally: where the car in front of the camera is (no motion blur there)
	R_ComputeMotionMask();

	if(!( fd->rdflags & RDF_NOWORLDMODEL ))
		R_AddPostProcessCmd();

	RE_EndScene();

	tr.frontEndMsec += ri.Milliseconds() - startTime;
}
