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
// tr_shade.c

#include "tr_local.h" 

/*

  THIS ENTIRE FILE IS BACK END

  This file deals with applying shaders to surface data in the tess struct.
*/


/*
==================
R_DrawElements

==================
*/

void R_DrawElements( int numIndexes, int firstIndex )
{
	if (tess.useCacheVao)
	{
		VaoCache_DrawElements(numIndexes, firstIndex);
	}
	else
	{
		qglDrawElements(GL_TRIANGLES, numIndexes, GL_INDEX_TYPE, BUFFER_OFFSET(firstIndex * sizeof(glIndex_t)));
	}
}


/*
=============================================================

SURFACE SHADERS

=============================================================
*/

shaderCommands_t	tess;


/*
=================
R_BindAnimatedImageToTMU

=================
*/
static void R_BindAnimatedImageToTMU( textureBundle_t *bundle, int tmu ) {
	int64_t index;

	if ( bundle->isVideoMap ) {
		ri.CIN_RunCinematic(bundle->videoMapHandle);
		ri.CIN_UploadCinematic(bundle->videoMapHandle);
		GL_BindToTMU(tr.scratchImage[bundle->videoMapHandle], tmu);
		return;
	}

	if ( bundle->numImageAnimations <= 1 ) {
		GL_BindToTMU( bundle->image[0], tmu);
		return;
	}

	// it is necessary to do this messy calc to make sure animations line up
	// exactly with waveforms of the same frequency
	index = tess.shaderTime * bundle->imageAnimationSpeed * FUNCTABLE_SIZE;
	index >>= FUNCTABLE_SIZE2;

	if ( index < 0 ) {
		index = 0;	// may happen with shader time offsets
	}

	// Windows x86 doesn't load renderer DLL with 64 bit modulus
	//index %= bundle->numImageAnimations;
	while ( index >= bundle->numImageAnimations ) {
		index -= bundle->numImageAnimations;
	}

	GL_BindToTMU( bundle->image[ index ], tmu );
}


/*
================
DrawTris

Draws triangle outlines for debugging
================
*/
static void DrawTris (shaderCommands_t *input) {
	GL_BindToTMU( tr.whiteImage, TB_COLORMAP );

	GL_State( GLS_POLYMODE_LINE | GLS_DEPTHMASK_TRUE );
	qglDepthRange( 0, 0 );

	{
		shaderProgram_t *sp = &tr.textureColorShader;
		vec4_t color;

		GLSL_BindProgram(sp);
		
		GLSL_SetUniformMat4(sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection);
		VectorSet4(color, 1, 1, 1, 1);
		GLSL_SetUniformVec4(sp, UNIFORM_COLOR, color);
		GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 0);

		R_DrawElements(input->numIndexes, input->firstIndex);
	}

	qglDepthRange( 0, 1 );
}


/*
================
DrawNormals

Draws vertex normals for debugging
================
*/
static void DrawNormals (shaderCommands_t *input) {
	//FIXME: implement this
}

/*
==============
RB_BeginSurface

We must set some things up before beginning any tesselation,
because a surface may be forced to perform a RB_End due
to overflow.
==============
*/
void RB_BeginSurface( shader_t *shader, int fogNum, int cubemapIndex ) {

	shader_t *state = (shader->remappedShader) ? shader->remappedShader : shader;

	tess.numIndexes = 0;
	tess.firstIndex = 0;
	tess.numVertexes = 0;
	tess.shader = state;
	tess.fogNum = fogNum;
	tess.cubemapIndex = cubemapIndex;
	tess.dlightBits = 0;		// will be OR'd in by surface functions
	tess.pshadowBits = 0;       // will be OR'd in by surface functions
	tess.xstages = state->stages;
	tess.numPasses = state->numUnfoggedPasses;
	tess.currentStageIteratorFunc = state->optimalStageIteratorFunc;
	tess.useInternalVao = qtrue;
	tess.useCacheVao = qfalse;
	tess.softParticle = qfalse;

	tess.shaderTime = backEnd.refdef.floatTime - tess.shader->timeOffset;
	if (tess.shader->clampTime && tess.shaderTime >= tess.shader->clampTime) {
		tess.shaderTime = tess.shader->clampTime;
	}

	if (backEnd.viewParms.flags & VPF_SHADOWMAP)
	{
		tess.currentStageIteratorFunc = RB_StageIteratorGeneric;
	}
}



extern float EvalWaveForm( const waveForm_t *wf );
extern float EvalWaveFormClamped( const waveForm_t *wf );


static void ComputeTexMods( shaderStage_t *pStage, int bundleNum, vec4_t outMatrix[8])
{
	int tm;
	float matrix[6];
	float tmpmatrix[6];
	float currentmatrix[6];
	float turb[2];
	textureBundle_t *bundle = &pStage->bundle[bundleNum];
	qboolean hasTurb = qfalse;

	currentmatrix[0] = 1.0f; currentmatrix[2] = 0.0f; currentmatrix[4] = 0.0f;
	currentmatrix[1] = 0.0f; currentmatrix[3] = 1.0f; currentmatrix[5] = 0.0f;

	for ( tm = 0; tm < bundle->numTexMods ; tm++ ) {
		switch ( bundle->texMods[tm].type )
		{
			
		case TMOD_NONE:
			matrix[0] = 1.0f; matrix[2] = 0.0f; matrix[4] = 0.0f;
			matrix[1] = 0.0f; matrix[3] = 1.0f; matrix[5] = 0.0f;
			break;

		case TMOD_TURBULENT:
			RB_CalcTurbulentFactors(&bundle->texMods[tm].wave, &turb[0], &turb[1]);
			break;

		case TMOD_ENTITY_TRANSLATE:
			RB_CalcScrollTexMatrix( backEnd.currentEntity->e.shaderTexCoord, matrix );
			break;

		case TMOD_SCROLL:
			RB_CalcScrollTexMatrix( bundle->texMods[tm].scroll,
									 matrix );
			break;

		case TMOD_SCALE:
			RB_CalcScaleTexMatrix( bundle->texMods[tm].scale,
								  matrix );
			break;
		
		case TMOD_STRETCH:
			RB_CalcStretchTexMatrix( &bundle->texMods[tm].wave, 
								   matrix );
			break;

		case TMOD_TRANSFORM:
			RB_CalcTransformTexMatrix( &bundle->texMods[tm],
									 matrix );
			break;

		case TMOD_ROTATE:
			RB_CalcRotateTexMatrix( bundle->texMods[tm].rotateSpeed,
									matrix );
			break;

		default:
			ri.Error( ERR_DROP, "ERROR: unknown texmod '%d' in shader '%s'", bundle->texMods[tm].type, tess.shader->name );
			break;
		}

		switch ( bundle->texMods[tm].type )
		{	
		case TMOD_TURBULENT:
			outMatrix[tm*2+0][0] = 1; outMatrix[tm*2+0][1] = 0; outMatrix[tm*2+0][2] = 0;
			outMatrix[tm*2+1][0] = 0; outMatrix[tm*2+1][1] = 1; outMatrix[tm*2+1][2] = 0;

			outMatrix[tm*2+0][3] = turb[0];
			outMatrix[tm*2+1][3] = turb[1];

			hasTurb = qtrue;
			break;

		case TMOD_NONE:
		case TMOD_ENTITY_TRANSLATE:
		case TMOD_SCROLL:
		case TMOD_SCALE:
		case TMOD_STRETCH:
		case TMOD_TRANSFORM:
		case TMOD_ROTATE:
		default:
			outMatrix[tm*2+0][0] = matrix[0]; outMatrix[tm*2+0][1] = matrix[2]; outMatrix[tm*2+0][2] = matrix[4];
			outMatrix[tm*2+1][0] = matrix[1]; outMatrix[tm*2+1][1] = matrix[3]; outMatrix[tm*2+1][2] = matrix[5];

			outMatrix[tm*2+0][3] = 0;
			outMatrix[tm*2+1][3] = 0;

			tmpmatrix[0] = matrix[0] * currentmatrix[0] + matrix[2] * currentmatrix[1];
			tmpmatrix[1] = matrix[1] * currentmatrix[0] + matrix[3] * currentmatrix[1];

			tmpmatrix[2] = matrix[0] * currentmatrix[2] + matrix[2] * currentmatrix[3];
			tmpmatrix[3] = matrix[1] * currentmatrix[2] + matrix[3] * currentmatrix[3];

			tmpmatrix[4] = matrix[0] * currentmatrix[4] + matrix[2] * currentmatrix[5] + matrix[4];
			tmpmatrix[5] = matrix[1] * currentmatrix[4] + matrix[3] * currentmatrix[5] + matrix[5];

			currentmatrix[0] = tmpmatrix[0];
			currentmatrix[1] = tmpmatrix[1];
			currentmatrix[2] = tmpmatrix[2];
			currentmatrix[3] = tmpmatrix[3];
			currentmatrix[4] = tmpmatrix[4];
			currentmatrix[5] = tmpmatrix[5];
			break;
		}
	}

	// if turb isn't used, only one matrix is needed
	if ( !hasTurb ) {
		tm = 0;

		outMatrix[tm*2+0][0] = currentmatrix[0]; outMatrix[tm*2+0][1] = currentmatrix[2]; outMatrix[tm*2+0][2] = currentmatrix[4];
		outMatrix[tm*2+1][0] = currentmatrix[1]; outMatrix[tm*2+1][1] = currentmatrix[3]; outMatrix[tm*2+1][2] = currentmatrix[5];

		outMatrix[tm*2+0][3] = 0;
		outMatrix[tm*2+1][3] = 0;
		tm++;
	}

	for ( ; tm < TR_MAX_TEXMODS ; tm++ ) {
		outMatrix[tm*2+0][0] = 1; outMatrix[tm*2+0][1] = 0; outMatrix[tm*2+0][2] = 0;
		outMatrix[tm*2+1][0] = 0; outMatrix[tm*2+1][1] = 1; outMatrix[tm*2+1][2] = 0;

		outMatrix[tm*2+0][3] = 0;
		outMatrix[tm*2+1][3] = 0;
	}
}


static void ComputeDeformValues(int *deformGen, vec5_t deformParams)
{
	// u_DeformGen
	*deformGen = DGEN_NONE;
	if(!ShaderRequiresCPUDeforms(tess.shader))
	{
		deformStage_t  *ds;

		// only support the first one
		ds = &tess.shader->deforms[0];

		switch (ds->deformation)
		{
			case DEFORM_WAVE:
				*deformGen = ds->deformationWave.func;

				deformParams[0] = ds->deformationWave.base;
				deformParams[1] = ds->deformationWave.amplitude;
				deformParams[2] = ds->deformationWave.phase;
				deformParams[3] = ds->deformationWave.frequency;
				deformParams[4] = ds->deformationSpread;
				break;

			case DEFORM_BULGE:
				*deformGen = DGEN_BULGE;

				deformParams[0] = 0;
				deformParams[1] = ds->bulgeHeight; // amplitude
				deformParams[2] = ds->bulgeWidth;  // phase
				deformParams[3] = ds->bulgeSpeed;  // frequency
				deformParams[4] = 0;
				break;

			default:
				break;
		}
	}
}


static void ProjectDlightTexture( void ) {
	int		l;
	vec3_t	origin;
	float	scale;
	float	radius;
	int deformGen;
	vec5_t deformParams;

	if ( !backEnd.refdef.num_dlights ) {
		return;
	}

	ComputeDeformValues(&deformGen, deformParams);

	for ( l = 0 ; l < backEnd.refdef.num_dlights ; l++ ) {
		dlight_t	*dl;
		shaderProgram_t *sp;
		vec4_t vector;

		if ( !( tess.dlightBits & ( 1 << l ) ) ) {
			continue;	// this surface definitely doesn't have any of this light
		}

		dl = &backEnd.refdef.dlights[l];
		VectorCopy( dl->transformed, origin );
		radius = dl->radius;
		scale = 1.0f / radius;

		sp = &tr.dlightShader[deformGen == DGEN_NONE ? 0 : 1];

		backEnd.pc.c_dlightDraws++;

		GLSL_BindProgram(sp);

		GLSL_SetUniformMat4(sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection);

		GLSL_SetUniformFloat(sp, UNIFORM_VERTEXLERP, glState.vertexAttribsInterpolation);
		
		GLSL_SetUniformInt(sp, UNIFORM_DEFORMGEN, deformGen);
		if (deformGen != DGEN_NONE)
		{
			GLSL_SetUniformFloat5(sp, UNIFORM_DEFORMPARAMS, deformParams);
			GLSL_SetUniformFloat(sp, UNIFORM_TIME, tess.shaderTime);
		}

		vector[0] = dl->color[0];
		vector[1] = dl->color[1];
		vector[2] = dl->color[2];
		vector[3] = 1.0f;
		GLSL_SetUniformVec4(sp, UNIFORM_COLOR, vector);

		vector[0] = origin[0];
		vector[1] = origin[1];
		vector[2] = origin[2];
		vector[3] = scale;
		GLSL_SetUniformVec4(sp, UNIFORM_DLIGHTINFO, vector);
	  
		GL_BindToTMU( tr.dlightImage, TB_COLORMAP );

		// include GLS_DEPTHFUNC_EQUAL so alpha tested surfaces don't add light
		// where they aren't rendered
		if ( dl->additive ) {
			GL_State( GLS_ATEST_GT_0 | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL );
		}
		else {
			GL_State( GLS_ATEST_GT_0 | GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL );
		}

		GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 1);

		R_DrawElements(tess.numIndexes, tess.firstIndex);

		backEnd.pc.c_totalIndexes += tess.numIndexes;
		backEnd.pc.c_dlightIndexes += tess.numIndexes;
		backEnd.pc.c_dlightVertexes += tess.numVertexes;
	}
}


static void ComputeShaderColors( shaderStage_t *pStage, vec4_t baseColor, vec4_t vertColor, int blend )
{
	qboolean isBlend = ((blend & GLS_SRCBLEND_BITS) == GLS_SRCBLEND_DST_COLOR)
		|| ((blend & GLS_SRCBLEND_BITS) == GLS_SRCBLEND_ONE_MINUS_DST_COLOR)
		|| ((blend & GLS_DSTBLEND_BITS) == GLS_DSTBLEND_SRC_COLOR)
		|| ((blend & GLS_DSTBLEND_BITS) == GLS_DSTBLEND_ONE_MINUS_SRC_COLOR);

	qboolean is2DDraw = backEnd.currentEntity == &backEnd.entity2D;

	float overbright = (isBlend || is2DDraw) ? 1.0f : (float)(1 << tr.overbrightBits);

	fog_t *fog;

	baseColor[0] = 
	baseColor[1] =
	baseColor[2] =
	baseColor[3] = 1.0f;

	vertColor[0] =
	vertColor[1] =
	vertColor[2] =
	vertColor[3] = 0.0f;

	//
	// rgbGen
	//
	switch ( pStage->rgbGen )
	{
		case CGEN_EXACT_VERTEX:
		case CGEN_EXACT_VERTEX_LIT:
			baseColor[0] = 
			baseColor[1] =
			baseColor[2] = 
			baseColor[3] = 0.0f;

			vertColor[0] =
			vertColor[1] =
			vertColor[2] = overbright;
			vertColor[3] = 1.0f;
			break;
		case CGEN_CONST:
			baseColor[0] = pStage->constantColor[0] / 255.0f;
			baseColor[1] = pStage->constantColor[1] / 255.0f;
			baseColor[2] = pStage->constantColor[2] / 255.0f;
			baseColor[3] = pStage->constantColor[3] / 255.0f;
			break;
		case CGEN_VERTEX:
		case CGEN_VERTEX_LIT:
			baseColor[0] =
			baseColor[1] =
			baseColor[2] =
			baseColor[3] = 0.0f;

			vertColor[0] =
			vertColor[1] =
			vertColor[2] =
			vertColor[3] = 1.0f;
			break;
		case CGEN_ONE_MINUS_VERTEX:
			baseColor[0] = 
			baseColor[1] =
			baseColor[2] = 1.0f;

			vertColor[0] =
			vertColor[1] =
			vertColor[2] = -1.0f;
			break;
		case CGEN_FOG:
			fog = tr.world->fogs + tess.fogNum;

			baseColor[0] = ((unsigned char *)(&fog->colorInt))[0] / 255.0f;
			baseColor[1] = ((unsigned char *)(&fog->colorInt))[1] / 255.0f;
			baseColor[2] = ((unsigned char *)(&fog->colorInt))[2] / 255.0f;
			baseColor[3] = ((unsigned char *)(&fog->colorInt))[3] / 255.0f;
			break;
		case CGEN_WAVEFORM:
			baseColor[0] = 
			baseColor[1] = 
			baseColor[2] = RB_CalcWaveColorSingle( &pStage->rgbWave );
			break;
		case CGEN_ENTITY:
			if (backEnd.currentEntity)
			{
				baseColor[0] = ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[0] / 255.0f;
				baseColor[1] = ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[1] / 255.0f;
				baseColor[2] = ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[2] / 255.0f;
				baseColor[3] = ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[3] / 255.0f;
			}
			break;
		case CGEN_ONE_MINUS_ENTITY:
			if (backEnd.currentEntity)
			{
				baseColor[0] = 1.0f - ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[0] / 255.0f;
				baseColor[1] = 1.0f - ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[1] / 255.0f;
				baseColor[2] = 1.0f - ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[2] / 255.0f;
				baseColor[3] = 1.0f - ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[3] / 255.0f;
			}
			break;
		case CGEN_IDENTITY:
		case CGEN_LIGHTING_DIFFUSE:
			baseColor[0] =
			baseColor[1] =
			baseColor[2] = overbright;
			break;
		case CGEN_IDENTITY_LIGHTING:
		case CGEN_BAD:
			break;
	}

	//
	// alphaGen
	//
	switch ( pStage->alphaGen )
	{
		case AGEN_SKIP:
			break;
		case AGEN_CONST:
			baseColor[3] = pStage->constantColor[3] / 255.0f;
			vertColor[3] = 0.0f;
			break;
		case AGEN_WAVEFORM:
			baseColor[3] = RB_CalcWaveAlphaSingle( &pStage->alphaWave );
			vertColor[3] = 0.0f;
			break;
		case AGEN_ENTITY:
			if (backEnd.currentEntity)
			{
				baseColor[3] = ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[3] / 255.0f;
			}
			vertColor[3] = 0.0f;
			break;
		case AGEN_ONE_MINUS_ENTITY:
			if (backEnd.currentEntity)
			{
				baseColor[3] = 1.0f - ((unsigned char *)backEnd.currentEntity->e.shaderRGBA)[3] / 255.0f;
			}
			vertColor[3] = 0.0f;
			break;
		case AGEN_VERTEX:
			baseColor[3] = 0.0f;
			vertColor[3] = 1.0f;
			break;
		case AGEN_ONE_MINUS_VERTEX:
			baseColor[3] = 1.0f;
			vertColor[3] = -1.0f;
			break;
		case AGEN_IDENTITY:
		case AGEN_LIGHTING_SPECULAR:
		case AGEN_PORTAL:
			// Done entirely in vertex program
			baseColor[3] = 1.0f;
			vertColor[3] = 0.0f;
			break;
	}

	// FIXME: find some way to implement this.
#if 0
	// if in greyscale rendering mode turn all color values into greyscale.
	if(r_greyscale->integer)
	{
		int scale;
		
		for(i = 0; i < tess.numVertexes; i++)
		{
			scale = (tess.svars.colors[i][0] + tess.svars.colors[i][1] + tess.svars.colors[i][2]) / 3;
			tess.svars.colors[i][0] = tess.svars.colors[i][1] = tess.svars.colors[i][2] = scale;
		}
	}
#endif
}


static void ComputeFogValues(vec4_t fogDistanceVector, vec4_t fogDepthVector, float *eyeT)
{
	// from RB_CalcFogTexCoords()
	fog_t  *fog;
	vec3_t  local;

	if (!tess.fogNum)
		return;

	fog = tr.world->fogs + tess.fogNum;

	VectorSubtract( backEnd.or.origin, backEnd.viewParms.or.origin, local );
	fogDistanceVector[0] = -backEnd.or.modelMatrix[2];
	fogDistanceVector[1] = -backEnd.or.modelMatrix[6];
	fogDistanceVector[2] = -backEnd.or.modelMatrix[10];
	fogDistanceVector[3] = DotProduct( local, backEnd.viewParms.or.axis[0] );

	// scale the fog vectors based on the fog's thickness
	VectorScale4(fogDistanceVector, fog->tcScale, fogDistanceVector);

	// rotate the gradient vector for this orientation
	if ( fog->hasSurface ) {
		fogDepthVector[0] = fog->surface[0] * backEnd.or.axis[0][0] + 
			fog->surface[1] * backEnd.or.axis[0][1] + fog->surface[2] * backEnd.or.axis[0][2];
		fogDepthVector[1] = fog->surface[0] * backEnd.or.axis[1][0] + 
			fog->surface[1] * backEnd.or.axis[1][1] + fog->surface[2] * backEnd.or.axis[1][2];
		fogDepthVector[2] = fog->surface[0] * backEnd.or.axis[2][0] + 
			fog->surface[1] * backEnd.or.axis[2][1] + fog->surface[2] * backEnd.or.axis[2][2];
		fogDepthVector[3] = -fog->surface[3] + DotProduct( backEnd.or.origin, fog->surface );

		*eyeT = DotProduct( backEnd.or.viewOrigin, fogDepthVector ) + fogDepthVector[3];
	} else {
		*eyeT = 1;	// non-surface fog always has eye inside
	}
}


static void ComputeFogColorMask( shaderStage_t *pStage, vec4_t fogColorMask )
{
	switch(pStage->adjustColorsForFog)
	{
		case ACFF_MODULATE_RGB:
			fogColorMask[0] =
			fogColorMask[1] =
			fogColorMask[2] = 1.0f;
			fogColorMask[3] = 0.0f;
			break;
		case ACFF_MODULATE_ALPHA:
			fogColorMask[0] =
			fogColorMask[1] =
			fogColorMask[2] = 0.0f;
			fogColorMask[3] = 1.0f;
			break;
		case ACFF_MODULATE_RGBA:
			fogColorMask[0] =
			fogColorMask[1] =
			fogColorMask[2] =
			fogColorMask[3] = 1.0f;
			break;
		default:
			fogColorMask[0] =
			fogColorMask[1] =
			fogColorMask[2] =
			fogColorMask[3] = 0.0f;
			break;
	}
}


static void ForwardDlight( void ) {
	int		l;
	//vec3_t	origin;
	//float	scale;
	float	radius;

	int deformGen;
	vec5_t deformParams;
	
	vec4_t fogDistanceVector, fogDepthVector = {0, 0, 0, 0};
	float eyeT = 0;

	shaderCommands_t *input = &tess;
	shaderStage_t *pStage = tess.xstages[0];

	if ( !backEnd.refdef.num_dlights ) {
		return;
	}
	
	ComputeDeformValues(&deformGen, deformParams);

	ComputeFogValues(fogDistanceVector, fogDepthVector, &eyeT);

	for ( l = 0 ; l < backEnd.refdef.num_dlights ; l++ ) {
		dlight_t	*dl;
		shaderProgram_t *sp;
		vec4_t vector;
		vec4_t texMatrix[8];

		if ( !( tess.dlightBits & ( 1 << l ) ) ) {
			continue;	// this surface definitely doesn't have any of this light
		}

		dl = &backEnd.refdef.dlights[l];
		//VectorCopy( dl->transformed, origin );
		radius = dl->radius;
		//scale = 1.0f / radius;

		//if (pStage->glslShaderGroup == tr.lightallShader)
		{
			int index = pStage->glslShaderIndex;

			index &= ~LIGHTDEF_LIGHTTYPE_MASK;
			index |= LIGHTDEF_USE_LIGHT_VECTOR;

			sp = &tr.lightallShader[index];
		}

		backEnd.pc.c_lightallDraws++;

		GLSL_BindProgram(sp);

		GLSL_SetUniformMat4(sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection);
		GLSL_SetUniformVec3(sp, UNIFORM_VIEWORIGIN, backEnd.viewParms.or.origin);
		GLSL_SetUniformVec3(sp, UNIFORM_LOCALVIEWORIGIN, backEnd.or.viewOrigin);

		GLSL_SetUniformFloat(sp, UNIFORM_VERTEXLERP, glState.vertexAttribsInterpolation);

		GLSL_SetUniformInt(sp, UNIFORM_DEFORMGEN, deformGen);
		if (deformGen != DGEN_NONE)
		{
			GLSL_SetUniformFloat5(sp, UNIFORM_DEFORMPARAMS, deformParams);
			GLSL_SetUniformFloat(sp, UNIFORM_TIME, tess.shaderTime);
		}

		if ( input->fogNum ) {
			vec4_t fogColorMask;

			GLSL_SetUniformVec4(sp, UNIFORM_FOGDISTANCE, fogDistanceVector);
			GLSL_SetUniformVec4(sp, UNIFORM_FOGDEPTH, fogDepthVector);
			GLSL_SetUniformFloat(sp, UNIFORM_FOGEYET, eyeT);

			ComputeFogColorMask(pStage, fogColorMask);

			GLSL_SetUniformVec4(sp, UNIFORM_FOGCOLORMASK, fogColorMask);
		}

		{
			vec4_t baseColor;
			vec4_t vertColor;

			ComputeShaderColors(pStage, baseColor, vertColor, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);

			GLSL_SetUniformVec4(sp, UNIFORM_BASECOLOR, baseColor);
			GLSL_SetUniformVec4(sp, UNIFORM_VERTCOLOR, vertColor);
		}

		if (pStage->alphaGen == AGEN_PORTAL)
		{
			GLSL_SetUniformFloat(sp, UNIFORM_PORTALRANGE, tess.shader->portalRange);
		}

		GLSL_SetUniformInt(sp, UNIFORM_COLORGEN, pStage->rgbGen);
		GLSL_SetUniformInt(sp, UNIFORM_ALPHAGEN, pStage->alphaGen);

		GLSL_SetUniformVec3(sp, UNIFORM_DIRECTEDLIGHT, dl->color);

		VectorSet(vector, 0, 0, 0);
		GLSL_SetUniformVec3(sp, UNIFORM_AMBIENTLIGHT, vector);

		VectorCopy(dl->origin, vector);
		vector[3] = 1.0f;
		GLSL_SetUniformVec4(sp, UNIFORM_LIGHTORIGIN, vector);

		GLSL_SetUniformFloat(sp, UNIFORM_LIGHTRADIUS, radius);

		GLSL_SetUniformVec4(sp, UNIFORM_NORMALSCALE, pStage->normalScale);
		GLSL_SetUniformVec4(sp, UNIFORM_SPECULARSCALE, pStage->specularScale);
		
		// include GLS_DEPTHFUNC_EQUAL so alpha tested surfaces don't add light
		// where they aren't rendered
		GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL );
		GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 0);

		GLSL_SetUniformMat4(sp, UNIFORM_MODELMATRIX, backEnd.or.transformMatrix);

		if (pStage->bundle[TB_DIFFUSEMAP].image[0])
			R_BindAnimatedImageToTMU( &pStage->bundle[TB_DIFFUSEMAP], TB_DIFFUSEMAP);

		// bind textures that are sampled and used in the glsl shader, and
		// bind whiteImage to textures that are sampled but zeroed in the glsl shader
		//
		// alternatives:
		//  - use the last bound texture
		//     -> costs more to sample a higher res texture then throw out the result
		//  - disable texture sampling in glsl shader with #ifdefs, as before
		//     -> increases the number of shaders that must be compiled
		//

		if (pStage->bundle[TB_NORMALMAP].image[0])
		{
			R_BindAnimatedImageToTMU( &pStage->bundle[TB_NORMALMAP], TB_NORMALMAP);
		}
		else if (r_normalMapping->integer)
			GL_BindToTMU( tr.whiteImage, TB_NORMALMAP );

		if (pStage->bundle[TB_SPECULARMAP].image[0])
		{
			R_BindAnimatedImageToTMU( &pStage->bundle[TB_SPECULARMAP], TB_SPECULARMAP);
		}
		else if (r_specularMapping->integer)
			GL_BindToTMU( tr.whiteImage, TB_SPECULARMAP );

		{
			vec4_t enableTextures;

			VectorSet4(enableTextures, 0.0f, 0.0f, 0.0f, 0.0f);
			GLSL_SetUniformVec4(sp, UNIFORM_ENABLETEXTURES, enableTextures);
		}

		if (r_dlightMode->integer >= 2)
			GL_BindToTMU(tr.shadowCubemaps[l], TB_SHADOWMAP);

		ComputeTexMods( pStage, TB_DIFFUSEMAP, texMatrix );
		GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX0, texMatrix[0]);
		GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX1, texMatrix[1]);
		GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX2, texMatrix[2]);
		GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX3, texMatrix[3]);
		GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX4, texMatrix[4]);
		GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX5, texMatrix[5]);
		GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX6, texMatrix[6]);
		GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX7, texMatrix[7]);

		GLSL_SetUniformInt(sp, UNIFORM_TCGEN0, pStage->bundle[0].tcGen);

		//
		// draw
		//

		R_DrawElements(input->numIndexes, input->firstIndex);

		backEnd.pc.c_totalIndexes += tess.numIndexes;
		backEnd.pc.c_dlightIndexes += tess.numIndexes;
		backEnd.pc.c_dlightVertexes += tess.numVertexes;
	}
}


/*
===================
ForwardSpotLights

Q3Rally: headlights (spot lights). One additive pass per light on the
surfaces it reaches: surface colour (the first texture stage that is not
the lightmap) * light colour * cone * falloff * N.L * shadow.
===================
*/
static void ForwardSpotLights( int spotBits ) {
	shaderProgram_t	*sp = &tr.spotlightShader;
	shaderStage_t	*pStage = NULL;
	vec4_t			texMatrix[8];
	vec4_t			vector;
	int				i, l;

	if ( !sp->program ) {
		return;
	}

	for ( i = 0; i < MAX_SHADER_STAGES; i++ ) {
		shaderStage_t *st = tess.xstages[i];

		if ( !st ) {
			break;
		}
		if ( st->bundle[0].image[0] && !st->bundle[0].isLightmap && st->bundle[0].tcGen == TCGEN_TEXTURE ) {
			pStage = st;
			break;
		}
	}

	GLSL_BindProgram( sp );

	GLSL_SetUniformMat4( sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection );
	// world surfaces are in world space (their transformMatrix is not set)
	if ( backEnd.currentEntity && backEnd.currentEntity != &tr.worldEntity ) {
		GLSL_SetUniformMat4( sp, UNIFORM_MODELMATRIX, backEnd.or.transformMatrix );
	} else {
		mat4_t identity;
		Mat4Identity( identity );
		GLSL_SetUniformMat4( sp, UNIFORM_MODELMATRIX, identity );
	}

	if ( pStage ) {
		R_BindAnimatedImageToTMU( &pStage->bundle[0], TB_DIFFUSEMAP );
		ComputeTexMods( pStage, TB_DIFFUSEMAP, texMatrix );
	} else {
		GL_BindToTMU( tr.whiteImage, TB_DIFFUSEMAP );
		for ( i = 0; i < 8; i++ ) {
			VectorSet4( texMatrix[i], ( i & 1 ) ? 0.0f : 1.0f, ( i & 1 ) ? 1.0f : 0.0f, 0.0f, 0.0f );
		}
	}

	GLSL_SetUniformVec4( sp, UNIFORM_DIFFUSETEXMATRIX0, texMatrix[0] );
	GLSL_SetUniformVec4( sp, UNIFORM_DIFFUSETEXMATRIX1, texMatrix[1] );
	GLSL_SetUniformVec4( sp, UNIFORM_DIFFUSETEXMATRIX2, texMatrix[2] );
	GLSL_SetUniformVec4( sp, UNIFORM_DIFFUSETEXMATRIX3, texMatrix[3] );
	GLSL_SetUniformVec4( sp, UNIFORM_DIFFUSETEXMATRIX4, texMatrix[4] );
	GLSL_SetUniformVec4( sp, UNIFORM_DIFFUSETEXMATRIX5, texMatrix[5] );
	GLSL_SetUniformVec4( sp, UNIFORM_DIFFUSETEXMATRIX6, texMatrix[6] );
	GLSL_SetUniformVec4( sp, UNIFORM_DIFFUSETEXMATRIX7, texMatrix[7] );

	// include GLS_DEPTHFUNC_EQUAL so alpha tested surfaces don't add light
	// where they aren't rendered
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL );

	for ( l = 0; l < backEnd.refdef.num_dlights; l++ ) {
		dlight_t *dl;

		if ( !( spotBits & ( 1 << l ) ) ) {
			continue;
		}

		dl = &backEnd.refdef.dlights[l];

		VectorScale( dl->color, r_headlightIntensity->value, vector );
		vector[3] = 1.0f;
		GLSL_SetUniformVec4( sp, UNIFORM_COLOR, vector );

		VectorCopy( dl->origin, vector );
		vector[3] = dl->radius;
		GLSL_SetUniformVec4( sp, UNIFORM_LIGHTORIGIN, vector );

		VectorCopy( dl->spotDir, vector );
		vector[3] = dl->spotCosOuter;
		GLSL_SetUniformVec4( sp, UNIFORM_SPOTDIR, vector );

		VectorSet4( vector, dl->spotCosInner, dl->spotShadow >= 0 ? 1.0f : 0.0f, 0.0f, 0.0f );
		GLSL_SetUniformVec4( sp, UNIFORM_SHADOWPARAMS, vector );

		if ( dl->spotShadow >= 0 ) {
			GL_BindToTMU( tr.spotShadowImage[dl->spotShadow], TB_SHADOWMAP );
			GLSL_SetUniformMat4( sp, UNIFORM_SHADOWMVP, dl->spotShadowMvp );
		}

		R_DrawElements( tess.numIndexes, tess.firstIndex );

		backEnd.pc.c_totalIndexes += tess.numIndexes;
		backEnd.pc.c_dlightIndexes += tess.numIndexes;
		backEnd.pc.c_dlightVertexes += tess.numVertexes;
	}
}


static void ProjectPshadowVBOGLSL( void ) {
	int		l;
	vec3_t	origin;
	float	radius;

	int deformGen;
	vec5_t deformParams;

	shaderCommands_t *input = &tess;

	if ( !backEnd.refdef.num_pshadows ) {
		return;
	}
	
	ComputeDeformValues(&deformGen, deformParams);

	for ( l = 0 ; l < backEnd.refdef.num_pshadows ; l++ ) {
		pshadow_t	*ps;
		shaderProgram_t *sp;
		vec4_t vector;

		if ( !( tess.pshadowBits & ( 1 << l ) ) ) {
			continue;	// this surface definitely doesn't have any of this shadow
		}

		ps = &backEnd.refdef.pshadows[l];
		VectorCopy( ps->lightOrigin, origin );
		radius = ps->lightRadius;

		sp = &tr.pshadowShader;

		GLSL_BindProgram(sp);

		GLSL_SetUniformMat4(sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection);

		VectorCopy(origin, vector);
		vector[3] = 1.0f;
		GLSL_SetUniformVec4(sp, UNIFORM_LIGHTORIGIN, vector);

		VectorScale(ps->lightViewAxis[0], 1.0f / ps->viewRadius, vector);
		GLSL_SetUniformVec3(sp, UNIFORM_LIGHTFORWARD, vector);

		VectorScale(ps->lightViewAxis[1], 1.0f / ps->viewRadius, vector);
		GLSL_SetUniformVec3(sp, UNIFORM_LIGHTRIGHT, vector);

		VectorScale(ps->lightViewAxis[2], 1.0f / ps->viewRadius, vector);
		GLSL_SetUniformVec3(sp, UNIFORM_LIGHTUP, vector);

		GLSL_SetUniformFloat(sp, UNIFORM_LIGHTRADIUS, radius);
	  
		// include GLS_DEPTHFUNC_EQUAL so alpha tested surfaces don't add light
		// where they aren't rendered
		GL_State( GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHFUNC_EQUAL );
		GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 0);

		GL_BindToTMU( tr.pshadowMaps[l], TB_DIFFUSEMAP );

		//
		// draw
		//

		R_DrawElements(input->numIndexes, input->firstIndex);

		backEnd.pc.c_totalIndexes += tess.numIndexes;
		//backEnd.pc.c_dlightIndexes += tess.numIndexes;
	}
}



/*
===================
RB_FogPass

Blends a fog texture on top of everything else
===================
*/
static void RB_FogPass( void ) {
	fog_t		*fog;
	vec4_t  color;
	vec4_t	fogDistanceVector, fogDepthVector = {0, 0, 0, 0};
	float	eyeT = 0;
	shaderProgram_t *sp;

	int deformGen;
	vec5_t deformParams;

	ComputeDeformValues(&deformGen, deformParams);

	{
		int index = 0;

		if (deformGen != DGEN_NONE)
			index |= FOGDEF_USE_DEFORM_VERTEXES;

		if (glState.vertexAnimation)
			index |= FOGDEF_USE_VERTEX_ANIMATION;
		else if (glState.boneAnimation)
			index |= FOGDEF_USE_BONE_ANIMATION;
		
		sp = &tr.fogShader[index];
	}

	backEnd.pc.c_fogDraws++;

	GLSL_BindProgram(sp);

	fog = tr.world->fogs + tess.fogNum;

	GLSL_SetUniformMat4(sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection);

	GLSL_SetUniformFloat(sp, UNIFORM_VERTEXLERP, glState.vertexAttribsInterpolation);

	if (glState.boneAnimation)
	{
		GLSL_SetUniformMat4BoneMatrix(sp, UNIFORM_BONEMATRIX, glState.boneMatrix, glState.boneAnimation);
	}
	
	GLSL_SetUniformInt(sp, UNIFORM_DEFORMGEN, deformGen);
	if (deformGen != DGEN_NONE)
	{
		GLSL_SetUniformFloat5(sp, UNIFORM_DEFORMPARAMS, deformParams);
		GLSL_SetUniformFloat(sp, UNIFORM_TIME, tess.shaderTime);
	}

	color[0] = ((unsigned char *)(&fog->colorInt))[0] / 255.0f;
	color[1] = ((unsigned char *)(&fog->colorInt))[1] / 255.0f;
	color[2] = ((unsigned char *)(&fog->colorInt))[2] / 255.0f;
	color[3] = ((unsigned char *)(&fog->colorInt))[3] / 255.0f;
	GLSL_SetUniformVec4(sp, UNIFORM_COLOR, color);

	ComputeFogValues(fogDistanceVector, fogDepthVector, &eyeT);

	GLSL_SetUniformVec4(sp, UNIFORM_FOGDISTANCE, fogDistanceVector);
	GLSL_SetUniformVec4(sp, UNIFORM_FOGDEPTH, fogDepthVector);
	GLSL_SetUniformFloat(sp, UNIFORM_FOGEYET, eyeT);

	if ( tess.shader->fogPass == FP_EQUAL ) {
		GL_State( GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHFUNC_EQUAL );
	} else {
		GL_State( GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA );
	}
	GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 0);

	R_DrawElements(tess.numIndexes, tess.firstIndex);
}


static unsigned int RB_CalcShaderVertexAttribs( shaderCommands_t *input )
{
	unsigned int vertexAttribs = input->shader->vertexAttribs;

	if(glState.vertexAnimation)
	{
		vertexAttribs |= ATTR_POSITION2;
		if (vertexAttribs & ATTR_NORMAL)
		{
			vertexAttribs |= ATTR_NORMAL2;
			vertexAttribs |= ATTR_TANGENT2;
		}
	}

	return vertexAttribs;
}

/*
===================
RB_CarReflectionStage

Q3Rally: a $dynamicreflection stage (car paint and glass, tcGen environment)
reflects the cube map around the camera (R_RenderCarReflection) instead of
the fixed sphere map. The stage keeps its blend, so the paint layer on top
still decides how much of the reflection shows. Shaders with an opaque such
stage (paint) get a clear coat pass on top as well, see the end of
RB_IterateStagesGeneric. qfalse = draw the stage the old way.
===================
*/
static qboolean RB_CarReflectionUsable( shaderCommands_t *input )
{
	if ( !r_carReflections->integer || !tr.carReflectImage || !tr.carReflectShader.program ) {
		return qfalse;
	}
	// cars far away inside the cube map itself, bone animated or deformed models
	if ( glState.currentFBO == tr.carReflectFbo || glState.boneAnimation || input->shader->numDeforms ) {
		return qfalse;
	}
	return qtrue;
}

static void RB_DrawCarReflection( shaderCommands_t *input, float strength, float facing, float mip, int stateBits )
{
	shaderProgram_t *sp = &tr.carReflectShader;
	vec4_t vec;

	GLSL_BindProgram( sp );

	GLSL_SetUniformMat4( sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection );
	if ( backEnd.currentEntity && backEnd.currentEntity != &tr.worldEntity ) {
		GLSL_SetUniformMat4( sp, UNIFORM_MODELMATRIX, backEnd.or.transformMatrix );
	} else {
		mat4_t identity;
		Mat4Identity( identity );
		GLSL_SetUniformMat4( sp, UNIFORM_MODELMATRIX, identity );
	}
	GLSL_SetUniformVec3( sp, UNIFORM_VIEWORIGIN, backEnd.viewParms.or.origin );
	GLSL_SetUniformFloat( sp, UNIFORM_VERTEXLERP, glState.vertexAnimation ? glState.vertexAttribsInterpolation : 0.0f );

	// x = strength, y = reflection seen straight on (more towards the
	// edges, Fresnel), z = mip level (rougher)
	VectorSet4( vec, strength, facing, mip, 0.0f );
	GLSL_SetUniformVec4( sp, UNIFORM_COLOR, vec );

	GL_BindToTMU( tr.carReflectImage, TB_CUBEMAP );
	GL_State( stateBits );

	R_DrawElements( input->numIndexes, input->firstIndex );
}

static qboolean RB_CarReflectionStage( shaderCommands_t *input, shaderStage_t *pStage )
{
	if ( pStage->bundle[0].image[0] != tr.dynamicReflectionImage || !RB_CarReflectionUsable( input ) ) {
		return qfalse;
	}
	RB_DrawCarReflection( input, r_carReflectionStrength->value, CAR_REFLECT_FACING, CAR_REFLECT_BLUR, pStage->stateBits );
	return qtrue;
}

/*
==================
RB_WetGroundPass

Q3Rally: wet ground. Drawn over an opaque world surface after its stages:
darker, and a screen space reflection of the scene (glsl/wetground_fp.glsl).
How strong depends on the surface type: asphalt and other hard ground
mirrors with puddles, loose ground mostly gets darker, grass only a bit.
==================
*/
#define WET_SKIP_FLAGS	( SURF_SKY | SURF_NODRAW | SURF_SNOW | SURF_ICE )

static void RB_WetGroundPass( shaderCommands_t *input )
{
	shader_t *shader = input->shader;
	shaderProgram_t *sp = &tr.wetGroundShader;
	float wetness, reflection, darken, puddles, buildup;
	vec3_t tint;
	vec4_t vec;
	qboolean ssr;
	image_t *lightmap = NULL;
	int i;

	if ( !sp->program || backEnd.depthFill || backEnd.currentEntity != &tr.worldEntity
		|| ( backEnd.refdef.rdflags & RDF_NOWORLDMODEL ) || backEnd.viewParms.targetFbo || backEnd.viewParms.isPortal
		|| ( backEnd.viewParms.flags & ( VPF_SHADOWMAP | VPF_DEPTHSHADOW | VPF_WATERREFLECTION | VPF_CARREFLECT ) )
		|| backEnd.viewParms.viewportWidth < glConfig.vidWidth / 2 )
	{
		return;
	}
	if ( shader->sort != SS_OPAQUE || shader->polygonOffset || shader->numDeforms || shader->isWater
		|| ( shader->surfaceFlags & WET_SKIP_FLAGS ) )
	{
		return;
	}

	wetness = backEnd.refdef.wetness;
	if ( shader->surfaceFlags & SURF_WET ) {
		wetness = 1.0f;		// always wet (puddles, wet spots in the map)
	}
	if ( wetness <= 0.0f && backEnd.refdef.weather[0] <= 0.0f && backEnd.refdef.weather[1] <= 0.0f && !r_weatherDebug->integer ) {
		return;		// dry, no snow, and no weather
	}

	// reflection of the surface itself, darkening, how much water it holds
	// (puddles), colour of the puddle water, how fast the snow settles
	if ( shader->surfaceFlags & SURF_GRASS ) {
		reflection = 0.10f; darken = 0.22f; puddles = 0.25f; buildup = 1.2f;
		VectorSet( tint, 0.75f, 0.72f, 0.55f );
	} else if ( shader->surfaceFlags & ( SURF_DIRT | SURF_DUST | SURF_SAND ) ) {
		reflection = 0.30f; darken = 0.40f; puddles = 1.0f; buildup = 1.1f;
		VectorSet( tint, 0.78f, 0.66f, 0.50f );		// muddy
	} else if ( shader->surfaceFlags & SURF_GRAVEL ) {
		reflection = 0.25f; darken = 0.35f; puddles = 0.6f; buildup = 1.0f;
		VectorSet( tint, 0.80f, 0.74f, 0.64f );
	} else {
		reflection = 1.0f; darken = 0.42f; puddles = 0.9f; buildup = 0.85f;	// asphalt, concrete, metal
		VectorSet( tint, 1.0f, 1.0f, 1.0f );
	}

	// the reflection needs this frame's depth and last frame's picture,
	// taken from almost the same place
	ssr = backEnd.wetDepthReady && tr.wetSceneValid && tr.hdrDepthImage && tr.wetSceneImage
		&& backEnd.refdef.time - tr.wetSceneTime > 0 && backEnd.refdef.time - tr.wetSceneTime < 250
		&& Distance( tr.wetScenePrevOrigin, backEnd.viewParms.or.origin ) < 400.0f;
	if ( !ssr ) {
		reflection = 0.0f;
	}

	GLSL_BindProgram( sp );
	GLSL_SetUniformMat4( sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection );
	GLSL_SetUniformMat4( sp, UNIFORM_PREVVIEWPROJ, tr.wetScenePrevViewProj );
	GLSL_SetUniformVec3( sp, UNIFORM_VIEWORIGIN, backEnd.viewParms.or.origin );

	// x, y = near and far plane, z = ray steps, w = mip level of the rough
	// reflection
	VectorSet4( vec, backEnd.viewParms.zNear, backEnd.viewParms.zFar, r_wetGround->integer >= 2 ? 40.0f : 24.0f, 4.0f );
	GLSL_SetUniformVec4( sp, UNIFORM_VIEWINFO, vec );

	VectorSet4( vec, wetness, reflection, darken, puddles );
	GLSL_SetUniformVec4( sp, UNIFORM_WETPARAMS, vec );

	VectorSet4( vec, tint[0], tint[1], tint[2], r_weatherDebug->integer ? 1.0f : 0.0f );
	GLSL_SetUniformVec4( sp, UNIFORM_COLOR, vec );

	// the weather: wet (and with drop rings while it rains) where the sky
	// grid says the weather reaches
	GLSL_SetUniformVec4( sp, UNIFORM_WEATHER, backEnd.refdef.weather );
	GLSL_SetUniformVec4( sp, UNIFORM_WEATHERGRID, tr.weatherGrid );
	GLSL_SetUniformVec4( sp, UNIFORM_WEATHERGRIDSIZE, tr.weatherGridSize );
	GL_BindToTMU( tr.weatherGridImage ? tr.weatherGridImage : tr.whiteImage, TB_NORMALMAP );
	GLSL_SetUniformFloat( sp, UNIFORM_TIME, backEnd.refdef.floatTime );

	// the snow is lit like the surface: by its lightmap, or by its vertex
	// light, and darkened by the sun shadow
	for ( i = 0; i < MAX_SHADER_STAGES && shader->stages[i] && shader->stages[i]->active; i++ ) {
		if ( shader->lightmapIndex < 0 ) {
			break;
		}
		if ( shader->stages[i]->bundle[TB_LIGHTMAP].image[0] ) {
			lightmap = shader->stages[i]->bundle[TB_LIGHTMAP].image[0];
			break;
		}
		if ( shader->stages[i]->bundle[0].isLightmap && shader->stages[i]->bundle[0].image[0] ) {
			lightmap = shader->stages[i]->bundle[0].image[0];	// a stage of its own
			break;
		}
	}
	GL_BindToTMU( lightmap ? lightmap : tr.whiteImage, TB_DELUXEMAP );
	vec[0] = lightmap ? 1.0f : 0.0f;
	vec[1] = buildup;
	vec[2] = -1.0f;
	vec[3] = 1.0f;
	if ( backEnd.refdef.weather[1] > 0.0f && r_sunlightMode->integer == 1 && ( backEnd.viewParms.flags & VPF_USESUNLIGHT )
		&& tr.screenShadowImage ) {
		GL_BindToTMU( tr.screenShadowImage, TB_SHADOWMAP );
		vec[2] = backEnd.refdef.sunAmbCol[0];
	}
	GLSL_SetUniformVec4( sp, UNIFORM_SNOWPARAMS, vec );

	if ( ssr ) {
		GL_BindToTMU( tr.wetSceneImage, TB_COLORMAP );
		GL_BindToTMU( tr.hdrDepthImage, TB_LIGHTMAP );
	} else {
		GL_BindToTMU( tr.whiteImage, TB_COLORMAP );	// not read without a reflection
		GL_BindToTMU( tr.whiteImage, TB_LIGHTMAP );
	}

	// colour adds the reflection, alpha darkens what is there
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_SRC_ALPHA | GLS_DEPTHFUNC_EQUAL );

	R_DrawElements( input->numIndexes, input->firstIndex );
}

static void RB_IterateStagesGeneric( shaderCommands_t *input )
{
	int stage;
	
	vec4_t fogDistanceVector, fogDepthVector = {0, 0, 0, 0};
	float eyeT = 0;

	int deformGen;
	vec5_t deformParams;

	qboolean renderToCubemap = tr.renderCubeFbo && glState.currentFBO == tr.renderCubeFbo;
	qboolean clearCoat = qfalse;	// Q3Rally: car paint gets a clear coat pass

	ComputeDeformValues(&deformGen, deformParams);

	ComputeFogValues(fogDistanceVector, fogDepthVector, &eyeT);

	for ( stage = 0; stage < MAX_SHADER_STAGES; stage++ )
	{
		shaderStage_t *pStage = input->xstages[stage];
		shaderProgram_t *sp;
		vec4_t texMatrix[8];

		if ( !pStage )
		{
			break;
		}

		// Q3Rally: car paint reflection
		if ( !backEnd.depthFill && RB_CarReflectionStage( input, pStage ) )
		{
			if ( !( pStage->stateBits & ( GLS_SRCBLEND_BITS | GLS_DSTBLEND_BITS ) ) )
			{
				clearCoat = qtrue;
			}
			continue;
		}

		if (backEnd.depthFill)
		{
			if (pStage->glslShaderGroup == tr.lightallShader)
			{
				int index = 0;

				if (backEnd.currentEntity && backEnd.currentEntity != &tr.worldEntity)
				{
					if (glState.boneAnimation)
					{
						index |= LIGHTDEF_ENTITY_BONE_ANIMATION;
					}
					else
					{
						index |= LIGHTDEF_ENTITY_VERTEX_ANIMATION;
					}
				}

				if (pStage->stateBits & GLS_ATEST_BITS)
				{
					index |= LIGHTDEF_USE_TCGEN_AND_TCMOD;
				}

				sp = &pStage->glslShaderGroup[index];
			}
			else
			{
				int shaderAttribs = 0;

				if (tess.shader->numDeforms && !ShaderRequiresCPUDeforms(tess.shader))
				{
					shaderAttribs |= GENERICDEF_USE_DEFORM_VERTEXES;
				}

				if (glState.vertexAnimation)
				{
					shaderAttribs |= GENERICDEF_USE_VERTEX_ANIMATION;
				}
				else if (glState.boneAnimation)
				{
					shaderAttribs |= GENERICDEF_USE_BONE_ANIMATION;
				}

				if (pStage->stateBits & GLS_ATEST_BITS)
				{
					shaderAttribs |= GENERICDEF_USE_TCGEN_AND_TCMOD;
				}

				sp = &tr.genericShader[shaderAttribs];
			}
		}
		else if (pStage->glslShaderGroup == tr.lightallShader)
		{
			int index = pStage->glslShaderIndex;

			if (backEnd.currentEntity && backEnd.currentEntity != &tr.worldEntity)
			{
				if (glState.boneAnimation)
				{
					index |= LIGHTDEF_ENTITY_BONE_ANIMATION;
				}
				else
				{
					index |= LIGHTDEF_ENTITY_VERTEX_ANIMATION;
				}
			}

			if (r_sunlightMode->integer && (backEnd.viewParms.flags & VPF_USESUNLIGHT) && (index & LIGHTDEF_LIGHTTYPE_MASK))
			{
				index |= LIGHTDEF_USE_SHADOWMAP;
			}

			if (r_lightmap->integer && ((index & LIGHTDEF_LIGHTTYPE_MASK) == LIGHTDEF_USE_LIGHTMAP))
			{
				index = LIGHTDEF_USE_TCGEN_AND_TCMOD;
			}

			sp = &pStage->glslShaderGroup[index];

			backEnd.pc.c_lightallDraws++;
		}
		else
		{
			sp = GLSL_GetGenericShaderProgram(stage);

			backEnd.pc.c_genericDraws++;
		}

		GLSL_BindProgram(sp);

		GLSL_SetUniformMat4(sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection);
		GLSL_SetUniformVec3(sp, UNIFORM_VIEWORIGIN, backEnd.viewParms.or.origin);
		GLSL_SetUniformVec3(sp, UNIFORM_LOCALVIEWORIGIN, backEnd.or.viewOrigin);

		GLSL_SetUniformFloat(sp, UNIFORM_VERTEXLERP, glState.vertexAttribsInterpolation);

		if (glState.boneAnimation)
		{
			GLSL_SetUniformMat4BoneMatrix(sp, UNIFORM_BONEMATRIX, glState.boneMatrix, glState.boneAnimation);
		}
		
		GLSL_SetUniformInt(sp, UNIFORM_DEFORMGEN, deformGen);
		if (deformGen != DGEN_NONE)
		{
			GLSL_SetUniformFloat5(sp, UNIFORM_DEFORMPARAMS, deformParams);
			GLSL_SetUniformFloat(sp, UNIFORM_TIME, tess.shaderTime);
		}

		if ( input->fogNum ) {
			GLSL_SetUniformVec4(sp, UNIFORM_FOGDISTANCE, fogDistanceVector);
			GLSL_SetUniformVec4(sp, UNIFORM_FOGDEPTH, fogDepthVector);
			GLSL_SetUniformFloat(sp, UNIFORM_FOGEYET, eyeT);
		}

		// Q3Rally: soft particles, sprites and polys fade out where they
		// cut into the opaque scene (tr.hdrDepthImage, copied after the
		// depth prepass)
		if (pStage->glslShaderGroup != tr.lightallShader)
		{
			vec4_t soft;
			int blend = pStage->stateBits & ( GLS_SRCBLEND_BITS | GLS_DSTBLEND_BITS );

			VectorSet4(soft, 0.0f, 0.0f, 0.0f, 0.0f);
			if (tess.softParticle && backEnd.softDepthReady && blend && !(pStage->stateBits & GLS_DEPTHMASK_TRUE)
				&& !input->shader->polygonOffset && input->shader->sort > SS_DECAL)
			{
				int src = pStage->stateBits & GLS_SRCBLEND_BITS;
				int dst = pStage->stateBits & GLS_DSTBLEND_BITS;
				vec4_t viewInfo;

				// Fade towards the colour that leaves the scene unchanged for
				// this blend; a wrong target shows the whole quad (e.g. white
				// for "gl_one gl_src_alpha" rocket explosions).
				soft[0] = r_softParticles->value;
				if (dst == GLS_DSTBLEND_ONE)
					soft[1] = (src == GLS_SRCBLEND_SRC_ALPHA) ? 0.0f : 1.0f;	// additive
				else if (dst == GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA)
					soft[1] = (src == GLS_SRCBLEND_SRC_ALPHA) ? 0.0f : 5.0f;	// alpha / premultiplied
				else if (dst == GLS_DSTBLEND_SRC_ALPHA)
					soft[1] = 4.0f;		// colour to black, alpha to one
				else if ((dst == GLS_DSTBLEND_ZERO && src == GLS_SRCBLEND_DST_COLOR)
					|| (dst == GLS_DSTBLEND_SRC_COLOR && src == GLS_SRCBLEND_ZERO))
					soft[1] = 2.0f;		// filter: towards white
				else if (dst == GLS_DSTBLEND_SRC_COLOR && src == GLS_SRCBLEND_DST_COLOR)
					soft[1] = 3.0f;		// 2x modulate: towards grey
				else if (dst == GLS_DSTBLEND_ONE_MINUS_SRC_COLOR
					&& (src == GLS_SRCBLEND_ZERO || src == GLS_SRCBLEND_ONE))
					soft[1] = 1.0f;		// subtractive: towards black
				else
					soft[0] = 0.0f;		// no neutral colour: leave it hard
				soft[2] = 1.0f / tr.hdrDepthImage->width;
				soft[3] = 1.0f / tr.hdrDepthImage->height;

				VectorSet4(viewInfo, backEnd.viewParms.zNear, backEnd.viewParms.zFar, 0.0f, 0.0f);
				GLSL_SetUniformVec4(sp, UNIFORM_VIEWINFO, viewInfo);
				GL_BindToTMU(tr.hdrDepthImage, TB_SPECULARMAP);
			}
			GLSL_SetUniformVec4(sp, UNIFORM_SOFTPARTICLE, soft);
		}

		GL_State( pStage->stateBits );
		if ((pStage->stateBits & GLS_ATEST_BITS) == GLS_ATEST_GT_0)
		{
			GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 1);
		}
		else if ((pStage->stateBits & GLS_ATEST_BITS) == GLS_ATEST_LT_80)
		{
			GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 2);
		}
		else if ((pStage->stateBits & GLS_ATEST_BITS) == GLS_ATEST_GE_80)
		{
			GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 3);
		}
		else
		{
			GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 0);
		}


		{
			vec4_t baseColor;
			vec4_t vertColor;

			ComputeShaderColors(pStage, baseColor, vertColor, pStage->stateBits);

			GLSL_SetUniformVec4(sp, UNIFORM_BASECOLOR, baseColor);
			GLSL_SetUniformVec4(sp, UNIFORM_VERTCOLOR, vertColor);
		}

		if (pStage->rgbGen == CGEN_LIGHTING_DIFFUSE)
		{
			vec4_t vec;

			VectorScale(backEnd.currentEntity->ambientLight, 1.0f / 255.0f, vec);
			GLSL_SetUniformVec3(sp, UNIFORM_AMBIENTLIGHT, vec);

			VectorScale(backEnd.currentEntity->directedLight, 1.0f / 255.0f, vec);
			GLSL_SetUniformVec3(sp, UNIFORM_DIRECTEDLIGHT, vec);
			
			VectorCopy(backEnd.currentEntity->lightDir, vec);
			vec[3] = 0.0f;
			GLSL_SetUniformVec4(sp, UNIFORM_LIGHTORIGIN, vec);
			GLSL_SetUniformVec3(sp, UNIFORM_MODELLIGHTDIR, backEnd.currentEntity->modelLightDir);

			GLSL_SetUniformFloat(sp, UNIFORM_LIGHTRADIUS, 0.0f);
		}

		if (pStage->alphaGen == AGEN_PORTAL)
		{
			GLSL_SetUniformFloat(sp, UNIFORM_PORTALRANGE, tess.shader->portalRange);
		}

		GLSL_SetUniformInt(sp, UNIFORM_COLORGEN, pStage->rgbGen);
		GLSL_SetUniformInt(sp, UNIFORM_ALPHAGEN, pStage->alphaGen);

		if ( input->fogNum )
		{
			vec4_t fogColorMask;

			ComputeFogColorMask(pStage, fogColorMask);

			GLSL_SetUniformVec4(sp, UNIFORM_FOGCOLORMASK, fogColorMask);
		}

		if (r_lightmap->integer)
		{
			vec4_t st[2];
			VectorSet4(st[0], 1.0f, 0.0f, 0.0f, 0.0f);
			VectorSet4(st[1], 0.0f, 1.0f, 0.0f, 0.0f);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX0, st[0]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX1, st[1]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX2, st[0]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX3, st[1]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX4, st[0]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX5, st[1]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX6, st[0]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX7, st[1]);

			GLSL_SetUniformInt(sp, UNIFORM_TCGEN0, TCGEN_LIGHTMAP);
		}
		else
		{
			ComputeTexMods(pStage, TB_DIFFUSEMAP, texMatrix);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX0, texMatrix[0]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX1, texMatrix[1]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX2, texMatrix[2]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX3, texMatrix[3]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX4, texMatrix[4]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX5, texMatrix[5]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX6, texMatrix[6]);
			GLSL_SetUniformVec4(sp, UNIFORM_DIFFUSETEXMATRIX7, texMatrix[7]);

			GLSL_SetUniformInt(sp, UNIFORM_TCGEN0, pStage->bundle[0].tcGen);
			if (pStage->bundle[0].tcGen == TCGEN_VECTOR)
			{
				vec3_t vec;

				VectorCopy(pStage->bundle[0].tcGenVectors[0], vec);
				GLSL_SetUniformVec3(sp, UNIFORM_TCGEN0VECTOR0, vec);
				VectorCopy(pStage->bundle[0].tcGenVectors[1], vec);
				GLSL_SetUniformVec3(sp, UNIFORM_TCGEN0VECTOR1, vec);
			}
		}

		GLSL_SetUniformMat4(sp, UNIFORM_MODELMATRIX, backEnd.or.transformMatrix);

		GLSL_SetUniformVec4(sp, UNIFORM_NORMALSCALE, pStage->normalScale);

		{
			vec4_t specularScale;
			Vector4Copy(pStage->specularScale, specularScale);

			if (renderToCubemap)
			{
				// force specular to nonmetal if rendering cubemaps
				if (r_pbr->integer)
					specularScale[1] = 0.0f;
			}

			GLSL_SetUniformVec4(sp, UNIFORM_SPECULARSCALE, specularScale);
		}

		//GLSL_SetUniformFloat(sp, UNIFORM_MAPLIGHTSCALE, backEnd.refdef.mapLightScale);

		//
		// do multitexture
		//
		if ( backEnd.depthFill )
		{
			if (!(pStage->stateBits & GLS_ATEST_BITS))
				GL_BindToTMU( tr.whiteImage, TB_COLORMAP );
			else if ( pStage->bundle[TB_COLORMAP].image[0] != 0 )
				R_BindAnimatedImageToTMU( &pStage->bundle[TB_COLORMAP], TB_COLORMAP );
		}
		else if ( pStage->glslShaderGroup == tr.lightallShader )
		{
			int i;
			vec4_t enableTextures;

			if (r_sunlightMode->integer && (backEnd.viewParms.flags & VPF_USESUNLIGHT) && (pStage->glslShaderIndex & LIGHTDEF_LIGHTTYPE_MASK))
			{
				// FIXME: screenShadowImage is NULL if no framebuffers
				if (tr.screenShadowImage)
					GL_BindToTMU(tr.screenShadowImage, TB_SHADOWMAP);
				GLSL_SetUniformVec3(sp, UNIFORM_PRIMARYLIGHTAMBIENT, backEnd.refdef.sunAmbCol);
				if (r_pbr->integer)
				{
					vec3_t color;

					color[0] = backEnd.refdef.sunCol[0] * backEnd.refdef.sunCol[0];
					color[1] = backEnd.refdef.sunCol[1] * backEnd.refdef.sunCol[1];
					color[2] = backEnd.refdef.sunCol[2] * backEnd.refdef.sunCol[2];
					GLSL_SetUniformVec3(sp, UNIFORM_PRIMARYLIGHTCOLOR, color);
				}
				else
				{
					GLSL_SetUniformVec3(sp, UNIFORM_PRIMARYLIGHTCOLOR, backEnd.refdef.sunCol);
				}
				GLSL_SetUniformVec4(sp, UNIFORM_PRIMARYLIGHTORIGIN,  backEnd.refdef.sunDir);
			}

			VectorSet4(enableTextures, 0, 0, 0, 0);
			if ((r_lightmap->integer == 1 || r_lightmap->integer == 2) && pStage->bundle[TB_LIGHTMAP].image[0])
			{
				for (i = 0; i < NUM_TEXTURE_BUNDLES; i++)
				{
					if (i == TB_COLORMAP)
						R_BindAnimatedImageToTMU( &pStage->bundle[TB_LIGHTMAP], i);
					else
						GL_BindToTMU( tr.whiteImage, i );
				}
			}
			else if (r_lightmap->integer == 3 && pStage->bundle[TB_DELUXEMAP].image[0])
			{
				for (i = 0; i < NUM_TEXTURE_BUNDLES; i++)
				{
					if (i == TB_COLORMAP)
						R_BindAnimatedImageToTMU( &pStage->bundle[TB_DELUXEMAP], i);
					else
						GL_BindToTMU( tr.whiteImage, i );
				}
			}
			else
			{
				qboolean light = (pStage->glslShaderIndex & LIGHTDEF_LIGHTTYPE_MASK) != 0;
				qboolean fastLight = !(r_normalMapping->integer || r_specularMapping->integer);

				if (pStage->bundle[TB_DIFFUSEMAP].image[0])
					R_BindAnimatedImageToTMU( &pStage->bundle[TB_DIFFUSEMAP], TB_DIFFUSEMAP);

				if (pStage->bundle[TB_LIGHTMAP].image[0])
					R_BindAnimatedImageToTMU( &pStage->bundle[TB_LIGHTMAP], TB_LIGHTMAP);

				// bind textures that are sampled and used in the glsl shader, and
				// bind whiteImage to textures that are sampled but zeroed in the glsl shader
				//
				// alternatives:
				//  - use the last bound texture
				//     -> costs more to sample a higher res texture then throw out the result
				//  - disable texture sampling in glsl shader with #ifdefs, as before
				//     -> increases the number of shaders that must be compiled
				//
				if (light && !fastLight)
				{
					if (pStage->bundle[TB_NORMALMAP].image[0])
					{
						R_BindAnimatedImageToTMU( &pStage->bundle[TB_NORMALMAP], TB_NORMALMAP);
						enableTextures[0] = 1.0f;
					}
					else if (r_normalMapping->integer)
						GL_BindToTMU( tr.whiteImage, TB_NORMALMAP );

					if (pStage->bundle[TB_DELUXEMAP].image[0])
					{
						R_BindAnimatedImageToTMU( &pStage->bundle[TB_DELUXEMAP], TB_DELUXEMAP);
						enableTextures[1] = 1.0f;
					}
					else if (r_deluxeMapping->integer)
						GL_BindToTMU( tr.whiteImage, TB_DELUXEMAP );

					if (pStage->bundle[TB_SPECULARMAP].image[0])
					{
						R_BindAnimatedImageToTMU( &pStage->bundle[TB_SPECULARMAP], TB_SPECULARMAP);
						enableTextures[2] = 1.0f;
					}
					else if (r_specularMapping->integer)
						GL_BindToTMU( tr.whiteImage, TB_SPECULARMAP );
				}

				enableTextures[3] = (r_cubeMapping->integer && !(tr.viewParms.flags & VPF_NOCUBEMAPS) && input->cubemapIndex) ? 1.0f : 0.0f;
			}

			GLSL_SetUniformVec4(sp, UNIFORM_ENABLETEXTURES, enableTextures);

			// Q3Rally: under a clouded sky the map's baked sunlight is dimmed
			GLSL_SetUniformFloat(sp, UNIFORM_OVERCAST, backEnd.currentEntity == &tr.worldEntity ? backEnd.refdef.overcast : 0.0f);
		}
		else if ( pStage->bundle[1].image[0] != 0 )
		{
			R_BindAnimatedImageToTMU( &pStage->bundle[0], 0 );
			R_BindAnimatedImageToTMU( &pStage->bundle[1], 1 );
		}
		else 
		{
			//
			// set state
			//
			R_BindAnimatedImageToTMU( &pStage->bundle[0], 0 );
		}

		//
		// testing cube map
		//
		if (!(tr.viewParms.flags & VPF_NOCUBEMAPS) && input->cubemapIndex && r_cubeMapping->integer)
		{
			vec4_t vec;
			cubemap_t *cubemap = &tr.cubemaps[input->cubemapIndex - 1];

			// FIXME: cubemap image could be NULL if cubemap isn't renderer or loaded
			if (cubemap->image)
				GL_BindToTMU( cubemap->image, TB_CUBEMAP);

			VectorSubtract(cubemap->origin, backEnd.viewParms.or.origin, vec);
			vec[3] = 1.0f;

			VectorScale4(vec, 1.0f / cubemap->parallaxRadius, vec);

			GLSL_SetUniformVec4(sp, UNIFORM_CUBEMAPINFO, vec);
		}

		//
		// draw
		//
		R_DrawElements(input->numIndexes, input->firstIndex);

		// allow skipping out to show just lightmaps during development
		if ( r_lightmap->integer && ( pStage->bundle[0].isLightmap || pStage->bundle[1].isLightmap ) )
		{
			break;
		}

		if (backEnd.depthFill)
			break;
	}

	// Q3Rally: clear coat of the car paint, the sharp reflection on top of
	// the paint layer (weak seen straight on, strong at the edges)
	if ( clearCoat && !backEnd.depthFill )
	{
		RB_DrawCarReflection( input, r_carReflectionStrength->value * CAR_REFLECT_COAT, CAR_REFLECT_COAT_FACING, 0.0f,
			GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_EQUAL );
	}

	// Q3Rally: wet ground
	RB_WetGroundPass( input );
}


static void RB_RenderShadowmap( shaderCommands_t *input )
{
	int deformGen;
	vec5_t deformParams;

	ComputeDeformValues(&deformGen, deformParams);

	{
		shaderProgram_t *sp = &tr.shadowmapShader[0];

		if (glState.vertexAnimation)
		{
			sp = &tr.shadowmapShader[SHADOWMAPDEF_USE_VERTEX_ANIMATION];
		}
		else if (glState.boneAnimation)
		{
			sp = &tr.shadowmapShader[SHADOWMAPDEF_USE_BONE_ANIMATION];
		}

		vec4_t vector;

		GLSL_BindProgram(sp);

		GLSL_SetUniformMat4(sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection);

		GLSL_SetUniformMat4(sp, UNIFORM_MODELMATRIX, backEnd.or.transformMatrix);

		GLSL_SetUniformFloat(sp, UNIFORM_VERTEXLERP, glState.vertexAttribsInterpolation);

		if (glState.boneAnimation)
		{
			GLSL_SetUniformMat4BoneMatrix(sp, UNIFORM_BONEMATRIX, glState.boneMatrix, glState.boneAnimation);
		}

		GLSL_SetUniformInt(sp, UNIFORM_DEFORMGEN, deformGen);
		if (deformGen != DGEN_NONE)
		{
			GLSL_SetUniformFloat5(sp, UNIFORM_DEFORMPARAMS, deformParams);
			GLSL_SetUniformFloat(sp, UNIFORM_TIME, tess.shaderTime);
		}

		VectorCopy(backEnd.viewParms.or.origin, vector);
		vector[3] = 1.0f;
		GLSL_SetUniformVec4(sp, UNIFORM_LIGHTORIGIN, vector);
		GLSL_SetUniformFloat(sp, UNIFORM_LIGHTRADIUS, backEnd.viewParms.zFar);

		GL_State( 0 );
		GLSL_SetUniformInt(sp, UNIFORM_ALPHATEST, 0);

		//
		// do multitexture
		//
		//if ( pStage->glslShaderGroup )
		{
			//
			// draw
			//

			R_DrawElements(input->numIndexes, input->firstIndex);
		}
	}
}



/*
** RB_StageIteratorGeneric
*/
/*
===================
RB_WaterPass

Q3Rally: draws a q3r_water surface with the water GLSL program. The
reflection texture holds the mirror view rendered for this frame; without
it the water is drawn with its colour only.
===================
*/
static void RB_WaterPass( shaderCommands_t *input )
{
	shader_t *shader = input->shader;
	shaderProgram_t *sp = &tr.waterShader;
	vec4_t vec;
	vec3_t sunColor;
	vec2_t invRes;
	float maxSun;

	// nothing to show inside mirror and portal views (the water plane is
	// the mirror plane there)
	if ( backEnd.viewParms.isPortal )
		return;

	GLSL_BindProgram(sp);

	GLSL_SetUniformMat4(sp, UNIFORM_MODELVIEWPROJECTIONMATRIX, glState.modelviewProjection);
	// world surfaces are already in world space (transformMatrix is only
	// set up for entities)
	if ( backEnd.currentEntity && backEnd.currentEntity != &tr.worldEntity )
	{
		GLSL_SetUniformMat4(sp, UNIFORM_MODELMATRIX, backEnd.or.transformMatrix);
	}
	else
	{
		mat4_t identity;

		Mat4Identity(identity);
		GLSL_SetUniformMat4(sp, UNIFORM_MODELMATRIX, identity);
	}
	GLSL_SetUniformVec3(sp, UNIFORM_VIEWORIGIN, backEnd.viewParms.or.origin);
	GLSL_SetUniformFloat(sp, UNIFORM_TIME, input->shaderTime);

	invRes[0] = 1.0f / tr.waterReflectionImage->width;
	invRes[1] = 1.0f / tr.waterReflectionImage->height;
	GLSL_SetUniformVec2(sp, UNIFORM_INVTEXRES, invRes);

	GLSL_SetUniformVec4(sp, UNIFORM_COLOR, shader->waterColor);

	VectorSet4(vec, 1.0f / shader->waterTile, shader->waterSpeed, shader->waterDistortion,
		backEnd.waterReflectionReady ? 1.0f : 0.0f);
	GLSL_SetUniformVec4(sp, UNIFORM_NORMALSCALE, vec);

	VectorSet4(vec, shader->waterFresnelBias, shader->waterFresnelPower, shader->waterSpecular, 0.0f);
	GLSL_SetUniformVec4(sp, UNIFORM_SPECULARSCALE, vec);

	// sun glint from the map's q3map_sun, brightest channel scaled to 1
	maxSun = MAX(tr.sunLight[0], MAX(tr.sunLight[1], tr.sunLight[2]));
	if (maxSun > 0.0f)
		VectorScale(tr.sunLight, 1.0f / maxSun, sunColor);
	else
		VectorClear(sunColor);
	GLSL_SetUniformVec3(sp, UNIFORM_PRIMARYLIGHTCOLOR, sunColor);

	VectorCopy(tr.sunDirection, vec);
	vec[3] = 0.0f;
	GLSL_SetUniformVec4(sp, UNIFORM_PRIMARYLIGHTORIGIN, vec);

	GL_BindToTMU(tr.waterReflectionImage, TB_COLORMAP);
	GL_BindToTMU(shader->waterNormalImage, TB_NORMALMAP);

	GL_State(GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA);

	R_DrawElements(input->numIndexes, input->firstIndex);
}

void RB_StageIteratorGeneric( void )
{
	shaderCommands_t *input;
	unsigned int vertexAttribs = 0;

	input = &tess;
	
	if (!input->numVertexes || !input->numIndexes)
	{
		return;
	}

	if (tess.useInternalVao)
	{
		RB_DeformTessGeometry();
	}

	vertexAttribs = RB_CalcShaderVertexAttribs( input );

	if (tess.useInternalVao)
	{
		RB_UpdateTessVao(vertexAttribs);
	}
	else
	{
		backEnd.pc.c_staticVaoDraws++;
	}

	//
	// log this call
	//
	if ( r_logFile->integer ) 
	{
		// don't just call LogComment, or we will get
		// a call to va() every frame!
		GLimp_LogComment( va("--- RB_StageIteratorGeneric( %s ) ---\n", tess.shader->name) );
	}

	//
	// set face culling appropriately
	//
	if (input->shader->cullType == CT_TWO_SIDED)
	{
		GL_Cull( CT_TWO_SIDED );
	}
	else
	{
		qboolean cullFront = (input->shader->cullType == CT_FRONT_SIDED);

		if ( backEnd.viewParms.flags & VPF_DEPTHSHADOW )
			cullFront = !cullFront;

		if ( backEnd.viewParms.isMirror )
			cullFront = !cullFront;

		if ( backEnd.currentEntity && backEnd.currentEntity->mirrored )
			cullFront = !cullFront;

		if (cullFront)
			GL_Cull( CT_FRONT_SIDED );
		else
			GL_Cull( CT_BACK_SIDED );
	}

	// set polygon offset if necessary
	if ( input->shader->polygonOffset )
	{
		qglEnable( GL_POLYGON_OFFSET_FILL );
	}

	//
	// render depth if in depthfill mode
	//
	if (backEnd.depthFill)
	{
		RB_IterateStagesGeneric( input );

		//
		// reset polygon offset
		//
		if ( input->shader->polygonOffset )
		{
			qglDisable( GL_POLYGON_OFFSET_FILL );
		}

		return;
	}

	//
	// render shadowmap if in shadowmap mode
	//
	if (backEnd.viewParms.flags & VPF_SHADOWMAP)
	{
		if ( input->shader->sort == SS_OPAQUE )
		{
			RB_RenderShadowmap( input );
		}
		//
		// reset polygon offset
		//
		if ( input->shader->polygonOffset )
		{
			qglDisable( GL_POLYGON_OFFSET_FILL );
		}

		return;
	}

	//
	//
	// call shader function
	//
	if ( input->shader->isWater )
	{
		RB_WaterPass( input );

		if ( input->shader->polygonOffset )
		{
			qglDisable( GL_POLYGON_OFFSET_FILL );
		}

		return;
	}

	RB_IterateStagesGeneric( input );

	//
	// pshadows!
	//
	if (glRefConfig.framebufferObject && r_shadows->integer == 4 && tess.pshadowBits
		&& tess.shader->sort <= SS_OPAQUE && !(tess.shader->surfaceFlags & (SURF_NODLIGHT | SURF_SKY) ) ) {
		ProjectPshadowVBOGLSL();
	}


	// 
	// now do any dynamic lighting needed
	//
	if ( tess.dlightBits && tess.shader->sort <= SS_OPAQUE && r_lightmap->integer == 0
		&& !(tess.shader->surfaceFlags & (SURF_NODLIGHT | SURF_SKY) ) ) {
		// Q3Rally: spot lights (headlights) have their own pass
		int allBits = tess.dlightBits;
		int spotBits = allBits & backEnd.refdef.spotMask;

		tess.dlightBits = allBits & ~spotBits;
		if (tess.dlightBits)
		{
			if (tess.shader->numUnfoggedPasses == 1 && tess.xstages[0]->glslShaderGroup == tr.lightallShader
				&& (tess.xstages[0]->glslShaderIndex & LIGHTDEF_LIGHTTYPE_MASK) && r_dlightMode->integer)
			{
				ForwardDlight();
			}
			else
			{
				ProjectDlightTexture();
			}
		}
		tess.dlightBits = allBits;

		if (spotBits)
			ForwardSpotLights(spotBits);
	}

	//
	// now do fog
	//
	if ( tess.fogNum && tess.shader->fogPass ) {
		RB_FogPass();
	}

	//
	// reset polygon offset
	//
	if ( input->shader->polygonOffset )
	{
		qglDisable( GL_POLYGON_OFFSET_FILL );
	}
}

/*
** RB_EndSurface
*/
void RB_EndSurface( void ) {
	shaderCommands_t *input;

	input = &tess;

	if (input->numIndexes == 0 || input->numVertexes == 0) {
		return;
	}

	if (input->indexes[SHADER_MAX_INDEXES-1] != 0) {
		ri.Error (ERR_DROP, "RB_EndSurface() - SHADER_MAX_INDEXES hit");
	}	
	if (input->xyz[SHADER_MAX_VERTEXES-1][0] != 0) {
		ri.Error (ERR_DROP, "RB_EndSurface() - SHADER_MAX_VERTEXES hit");
	}

	if ( tess.shader == tr.shadowShader ) {
		RB_ShadowTessEnd();
		return;
	}

	// for debugging of sort order issues, stop rendering after a given sort value
	if ( r_debugSort->integer && r_debugSort->integer < tess.shader->sort ) {
		return;
	}

	if (tess.useCacheVao)
	{
		// upload indexes now
		VaoCache_Commit();
	}

	//
	// update performance counters
	//
	backEnd.pc.c_shaders++;
	backEnd.pc.c_vertexes += tess.numVertexes;
	backEnd.pc.c_indexes += tess.numIndexes;
	backEnd.pc.c_totalIndexes += tess.numIndexes * tess.numPasses;

	//
	// call off to shader specific tess end function
	//
	tess.currentStageIteratorFunc();

	//
	// draw debugging stuff
	//
	if ( r_showtris->integer ) {
		DrawTris (input);
	}
	if ( r_shownormals->integer ) {
		DrawNormals (input);
	}
	// clear shader so we can tell we don't have any unclosed surfaces
	tess.numIndexes = 0;
	tess.numVertexes = 0;
	tess.firstIndex = 0;
	tess.useCacheVao = qfalse;
	tess.useInternalVao = qfalse;

	GLimp_LogComment( "----------\n" );
}
