/*
===========================================================================
Copyright (C) 2011 Andrei Drexler, Richard Allen, James Canete

This file is part of Reaction source code.

Reaction source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Reaction source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Reaction source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "tr_local.h"

void RB_ToneMap(FBO_t *hdrFbo, ivec4_t hdrBox, FBO_t *ldrFbo, ivec4_t ldrBox, int autoExposure)
{
	ivec4_t srcBox, dstBox;
	vec4_t color;
	static int lastFrameCount = 0;

	if (autoExposure)
	{
		if (lastFrameCount == 0 || tr.frameCount < lastFrameCount || tr.frameCount - lastFrameCount > 5)
		{
			// determine average log luminance
			FBO_t *srcFbo, *dstFbo, *tmp;
			int size = 256;

			lastFrameCount = tr.frameCount;

			VectorSet4(dstBox, 0, 0, size, size);

			FBO_Blit(hdrFbo, hdrBox, NULL, tr.textureScratchFbo[0], dstBox, &tr.calclevels4xShader[0], NULL, 0);

			srcFbo = tr.textureScratchFbo[0];
			dstFbo = tr.textureScratchFbo[1];

			// downscale to 1x1 texture
			while (size > 1)
			{
				VectorSet4(srcBox, 0, 0, size, size);
				//size >>= 2;
				size >>= 1;
				VectorSet4(dstBox, 0, 0, size, size);

				if (size == 1)
					dstFbo = tr.targetLevelsFbo;

				//FBO_Blit(targetFbo, srcBox, NULL, tr.textureScratchFbo[nextScratch], dstBox, &tr.calclevels4xShader[1], NULL, 0);
				FBO_FastBlit(srcFbo, srcBox, dstFbo, dstBox, GL_COLOR_BUFFER_BIT, GL_LINEAR);

				tmp = srcFbo;
				srcFbo = dstFbo;
				dstFbo = tmp;
			}
		}

		// blend with old log luminance for gradual change
		VectorSet4(srcBox, 0, 0, 0, 0);

		color[0] = 
		color[1] =
		color[2] = 1.0f;
		if (glRefConfig.textureFloat)
			color[3] = 0.03f;
		else
			color[3] = 0.1f;

		FBO_Blit(tr.targetLevelsFbo, srcBox, NULL, tr.calcLevelsFbo, NULL,  NULL, color, GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA);
	}

	// tonemap
	color[0] =
	color[1] =
	color[2] = pow(2, r_cameraExposure->value - autoExposure); //exp2(r_cameraExposure->value);
	color[3] = 1.0f;

	if (autoExposure)
		GL_BindToTMU(tr.calcLevelsImage,  TB_LEVELSMAP);
	else
		GL_BindToTMU(tr.fixedLevelsImage, TB_LEVELSMAP);

	// Q3Rally: tone curve and colour grading
	{
		vec4_t params;

		VectorSet4(params, r_toneMapCurve->integer ? 1.0f : 0.0f, r_filmicExposure->value, r_filmicWhite->value, 0.0f);
		GLSL_SetUniformVec4(&tr.tonemapShader, UNIFORM_TONEPARAMS, params);

		if (tr.colorLutImage && r_colorGrade->value > 0.0f)
		{
			VectorSet4(params, MIN(r_colorGrade->value, 1.0f), tr.colorLutImage->height, 0.0f, 0.0f);
			GL_BindToTMU(tr.colorLutImage, TB_NORMALMAP);
		}
		else
		{
			VectorSet4(params, 0.0f, 0.0f, 0.0f, 0.0f);
			GL_BindToTMU(tr.whiteImage, TB_NORMALMAP);
		}
		GLSL_SetUniformVec4(&tr.tonemapShader, UNIFORM_LUTPARAMS, params);
	}

	FBO_Blit(hdrFbo, hdrBox, NULL, ldrFbo, ldrBox, &tr.tonemapShader, color, 0);
}

/*
=============
RB_BokehBlur


Blurs a part of one framebuffer to another.

Framebuffers can be identical. 
=============
*/
void RB_BokehBlur(FBO_t *src, ivec4_t srcBox, FBO_t *dst, ivec4_t dstBox, float blur)
{
//	ivec4_t srcBox, dstBox;
	vec4_t color;
	
	blur *= 10.0f;

	if (blur < 0.004f)
		return;

	if (glRefConfig.framebufferObject)
	{
		// bokeh blur
		if (blur > 0.0f)
		{
			ivec4_t quarterBox;

			quarterBox[0] = 0;
			quarterBox[1] = tr.quarterFbo[0]->height;
			quarterBox[2] = tr.quarterFbo[0]->width;
			quarterBox[3] = -tr.quarterFbo[0]->height;

			// create a quarter texture
			//FBO_Blit(NULL, NULL, NULL, tr.quarterFbo[0], NULL, NULL, NULL, 0);
			FBO_FastBlit(src, srcBox, tr.quarterFbo[0], quarterBox, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}

#ifndef HQ_BLUR
		if (blur > 1.0f)
		{
			// create a 1/16th texture
			//FBO_Blit(tr.quarterFbo[0], NULL, NULL, tr.textureScratchFbo[0], NULL, NULL, NULL, 0);
			FBO_FastBlit(tr.quarterFbo[0], NULL, tr.textureScratchFbo[0], NULL, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
#endif

		if (blur > 0.0f && blur <= 1.0f)
		{
			// Crossfade original with quarter texture
			VectorSet4(color, 1, 1, 1, blur);

			FBO_Blit(tr.quarterFbo[0], NULL, NULL, dst, dstBox, NULL, color, GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA);
		}
#ifndef HQ_BLUR
		// ok blur, but can see some pixelization
		else if (blur > 1.0f && blur <= 2.0f)
		{
			// crossfade quarter texture with 1/16th texture
			FBO_Blit(tr.quarterFbo[0], NULL, NULL, dst, dstBox, NULL, NULL, 0);

			VectorSet4(color, 1, 1, 1, blur - 1.0f);

			FBO_Blit(tr.textureScratchFbo[0], NULL, NULL, dst, dstBox, NULL, color, GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA);
		}
		else if (blur > 2.0f)
		{
			// blur 1/16th texture then replace
			int i;

			for (i = 0; i < 2; i++)
			{
				vec2_t blurTexScale;
				float subblur;

				subblur = ((blur - 2.0f) / 2.0f) / 3.0f * (float)(i + 1);

				blurTexScale[0] =
				blurTexScale[1] = subblur;

				color[0] =
				color[1] =
				color[2] = 0.5f;
				color[3] = 1.0f;

				if (i != 0)
					FBO_Blit(tr.textureScratchFbo[0], NULL, blurTexScale, tr.textureScratchFbo[1], NULL, &tr.bokehShader, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);
				else
					FBO_Blit(tr.textureScratchFbo[0], NULL, blurTexScale, tr.textureScratchFbo[1], NULL, &tr.bokehShader, color, 0);
			}

			FBO_Blit(tr.textureScratchFbo[1], NULL, NULL, dst, dstBox, NULL, NULL, 0);
		}
#else // higher quality blur, but slower
		else if (blur > 1.0f)
		{
			// blur quarter texture then replace
			int i;

			src = tr.quarterFbo[0];
			dst = tr.quarterFbo[1];

			VectorSet4(color, 0.5f, 0.5f, 0.5f, 1);

			for (i = 0; i < 2; i++)
			{
				vec2_t blurTexScale;
				float subblur;

				subblur = (blur - 1.0f) / 2.0f * (float)(i + 1);

				blurTexScale[0] =
				blurTexScale[1] = subblur;

				color[0] =
				color[1] =
				color[2] = 1.0f;
				if (i != 0)
					color[3] = 1.0f;
				else
					color[3] = 0.5f;

				FBO_Blit(tr.quarterFbo[0], NULL, blurTexScale, tr.quarterFbo[1], NULL, &tr.bokehShader, color, GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA);
			}

			FBO_Blit(tr.quarterFbo[1], NULL, NULL, dst, dstBox, NULL, NULL, 0);
		}
#endif
	}
}


static void RB_RadialBlur(FBO_t *srcFbo, FBO_t *dstFbo, int passes, float stretch, float x, float y, float w, float h, float xcenter, float ycenter, float alpha)
{
	ivec4_t srcBox, dstBox;
	int srcWidth, srcHeight;
	vec4_t color;
	const float inc = 1.f / passes;
	const float mul = powf(stretch, inc);
	float scale;

	alpha *= inc;
	VectorSet4(color, alpha, alpha, alpha, 1.0f);

	srcWidth  = srcFbo ? srcFbo->width  : glConfig.vidWidth;
	srcHeight = srcFbo ? srcFbo->height : glConfig.vidHeight;

	VectorSet4(srcBox, 0, 0, srcWidth, srcHeight);

	VectorSet4(dstBox, x, y, w, h);
	FBO_Blit(srcFbo, srcBox, NULL, dstFbo, dstBox, NULL, color, 0);

	--passes;
	scale = mul;
	while (passes > 0)
	{
		float iscale = 1.f / scale;
		float s0 = xcenter * (1.f - iscale);
		float t0 = (1.0f - ycenter) * (1.f - iscale);

		srcBox[0] = s0 * srcWidth;
		srcBox[1] = t0 * srcHeight;
		srcBox[2] = iscale * srcWidth;
		srcBox[3] = iscale * srcHeight;
			
		FBO_Blit(srcFbo, srcBox, NULL, dstFbo, dstBox, NULL, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE );

		scale *= mul;
		--passes;
	}
}


static qboolean RB_UpdateSunFlareVis(void)
{
	GLuint sampleCount = 0;
	if (!glRefConfig.occlusionQuery)
		return qtrue;

	tr.sunFlareQueryIndex ^= 1;
	if (!tr.sunFlareQueryActive[tr.sunFlareQueryIndex])
		return qtrue;

	/* debug code */
	if (0)
	{
		int iter;
		for (iter=0 ; ; ++iter)
		{
			GLint available = 0;
			qglGetQueryObjectiv(tr.sunFlareQuery[tr.sunFlareQueryIndex], GL_QUERY_RESULT_AVAILABLE, &available);
			if (available)
				break;
		}

		ri.Printf(PRINT_DEVELOPER, "Waited %d iterations\n", iter);
	}
	
	// Note: On desktop OpenGL this is a sample count (glRefConfig.occlusionQueryTarget == GL_SAMPLES_PASSED)
	// but on OpenGL ES this is a boolean (glRefConfig.occlusionQueryTarget == GL_ANY_SAMPLES_PASSED)
	qglGetQueryObjectuiv(tr.sunFlareQuery[tr.sunFlareQueryIndex], GL_QUERY_RESULT, &sampleCount);
	return sampleCount > 0;
}

void RB_SunRays(FBO_t *srcFbo, ivec4_t srcBox, FBO_t *dstFbo, ivec4_t dstBox)
{
	vec4_t color;
	float dot;
	const float cutoff = 0.25f;
	qboolean colorize = qtrue;

//	float w, h, w2, h2;
	mat4_t mvp;
	vec4_t pos, hpos;

	dot = DotProduct(tr.sunDirection, backEnd.viewParms.or.axis[0]);
	if (dot < cutoff)
		return;

	if (!RB_UpdateSunFlareVis())
		return;

	// From RB_DrawSun()
	{
		float dist;
		mat4_t trans, model;

		Mat4Translation( backEnd.viewParms.or.origin, trans );
		Mat4Multiply( backEnd.viewParms.world.modelMatrix, trans, model );
		Mat4Multiply(backEnd.viewParms.projectionMatrix, model, mvp);

		dist = backEnd.viewParms.zFar / 1.75;		// div sqrt(3)

		VectorScale( tr.sunDirection, dist, pos );
	}

	// project sun point
	//Mat4Multiply(backEnd.viewParms.projectionMatrix, backEnd.viewParms.world.modelMatrix, mvp);
	Mat4Transform(mvp, pos, hpos);

	// transform to UV coords
	hpos[3] = 0.5f / hpos[3];

	pos[0] = 0.5f + hpos[0] * hpos[3];
	pos[1] = 0.5f + hpos[1] * hpos[3];

	// initialize quarter buffers
	{
		float mul = 1.f;
		ivec4_t rayBox, quarterBox;
		int srcWidth  = srcFbo ? srcFbo->width  : glConfig.vidWidth;
		int srcHeight = srcFbo ? srcFbo->height : glConfig.vidHeight;

		VectorSet4(color, mul, mul, mul, 1);

		rayBox[0] = srcBox[0] * tr.sunRaysFbo->width  / srcWidth;
		rayBox[1] = srcBox[1] * tr.sunRaysFbo->height / srcHeight;
		rayBox[2] = srcBox[2] * tr.sunRaysFbo->width  / srcWidth;
		rayBox[3] = srcBox[3] * tr.sunRaysFbo->height / srcHeight;

		quarterBox[0] = 0;
		quarterBox[1] = tr.quarterFbo[0]->height;
		quarterBox[2] = tr.quarterFbo[0]->width;
		quarterBox[3] = -tr.quarterFbo[0]->height;

		// first, downsample the framebuffer
		if (colorize)
		{
			FBO_FastBlit(srcFbo, srcBox, tr.quarterFbo[0], quarterBox, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			FBO_Blit(tr.sunRaysFbo, rayBox, NULL, tr.quarterFbo[0], quarterBox, NULL, color, GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ZERO);
		}
		else
		{
			FBO_FastBlit(tr.sunRaysFbo, rayBox, tr.quarterFbo[0], quarterBox, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
	}

	// radial blur passes, ping-ponging between the two quarter-size buffers
	{
		const float stretch_add = 2.f/3.f;
		float stretch = 1.f + stretch_add;
		int i;
		for (i=0; i<2; ++i)
		{
			RB_RadialBlur(tr.quarterFbo[i&1], tr.quarterFbo[(~i) & 1], 5, stretch, 0.f, 0.f, tr.quarterFbo[0]->width, tr.quarterFbo[0]->height, pos[0], pos[1], 1.125f);
			stretch += stretch_add;
		}
	}
	
	// add result back on top of the main buffer
	{
		float mul = 1.f;

		VectorSet4(color, mul, mul, mul, 1);

		FBO_Blit(tr.quarterFbo[0], NULL, NULL, dstFbo, dstBox, NULL, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);
	}
}

static void RB_BlurAxis(FBO_t *srcFbo, FBO_t *dstFbo, float strength, qboolean horizontal)
{
	float dx, dy;
	float xmul, ymul;
	float weights[3] = {
		0.227027027f,
		0.316216216f,
		0.070270270f,
	};
	float offsets[3] = {
		0.f,
		1.3846153846f,
		3.2307692308f,
	};

	xmul = horizontal;
	ymul = 1.f - xmul;

	xmul *= strength;
	ymul *= strength;

	{
		ivec4_t srcBox, dstBox;
		vec4_t color;

		VectorSet4(color, weights[0], weights[0], weights[0], 1.0f);
		VectorSet4(srcBox, 0, 0, srcFbo->width, srcFbo->height);
		VectorSet4(dstBox, 0, 0, dstFbo->width, dstFbo->height);
		FBO_Blit(srcFbo, srcBox, NULL, dstFbo, dstBox, NULL, color, 0);

		VectorSet4(color, weights[1], weights[1], weights[1], 1.0f);
		dx = offsets[1] * xmul;
		dy = offsets[1] * ymul;
		VectorSet4(srcBox, dx, dy, srcFbo->width, srcFbo->height);
		FBO_Blit(srcFbo, srcBox, NULL, dstFbo, dstBox, NULL, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);
		VectorSet4(srcBox, -dx, -dy, srcFbo->width, srcFbo->height);
		FBO_Blit(srcFbo, srcBox, NULL, dstFbo, dstBox, NULL, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);

		VectorSet4(color, weights[2], weights[2], weights[2], 1.0f);
		dx = offsets[2] * xmul;
		dy = offsets[2] * ymul;
		VectorSet4(srcBox, dx, dy, srcFbo->width, srcFbo->height);
		FBO_Blit(srcFbo, srcBox, NULL, dstFbo, dstBox, NULL, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);
		VectorSet4(srcBox, -dx, -dy, srcFbo->width, srcFbo->height);
		FBO_Blit(srcFbo, srcBox, NULL, dstFbo, dstBox, NULL, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);
	}
}

static void RB_HBlur(FBO_t *srcFbo, FBO_t *dstFbo, float strength)
{
	RB_BlurAxis(srcFbo, dstFbo, strength, qtrue);
}

static void RB_VBlur(FBO_t *srcFbo, FBO_t *dstFbo, float strength)
{
	RB_BlurAxis(srcFbo, dstFbo, strength, qfalse);
}

void RB_GaussianBlur(FBO_t *srcFbo, FBO_t *dstFbo, float blur)
{
	//float mul = 1.f;
	float factor = Com_Clamp(0.f, 1.f, blur);

	if (factor <= 0.f)
		return;

	{
		ivec4_t srcBox, dstBox;
		vec4_t color;

		VectorSet4(color, 1, 1, 1, 1);

		// first, downsample the framebuffer
		FBO_FastBlit(srcFbo, NULL, tr.quarterFbo[0], NULL, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		FBO_FastBlit(tr.quarterFbo[0], NULL, tr.textureScratchFbo[0], NULL, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// set the alpha channel
		qglColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		FBO_BlitFromTexture(tr.whiteImage, NULL, NULL, tr.textureScratchFbo[0], NULL, NULL, color, GLS_DEPTHTEST_DISABLE);
		qglColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		// blur the tiny buffer horizontally and vertically
		RB_HBlur(tr.textureScratchFbo[0], tr.textureScratchFbo[1], factor);
		RB_VBlur(tr.textureScratchFbo[1], tr.textureScratchFbo[0], factor);

		// finally, merge back to framebuffer
		VectorSet4(srcBox, 0, 0, tr.textureScratchFbo[0]->width, tr.textureScratchFbo[0]->height);
		VectorSet4(dstBox, 0, 0, glConfig.vidWidth,              glConfig.vidHeight);
		color[3] = factor;
		FBO_Blit(tr.textureScratchFbo[0], srcBox, NULL, dstFbo, dstBox, NULL, color, GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA);
	}
}


/*
=============
RB_Bloom

Q3Rally: glow around the bright parts of the final (tone mapped) image.
The bright pass also downsamples to 1/4 of the screen; that level is
blurred, downsampled again to 1/16 and blurred, and both levels are added
back: the small one gives the tight glow around a lamp, the big one the
wide haze.
=============
*/
void RB_Bloom(FBO_t *srcFbo, ivec4_t srcBox)
{
	vec4_t color;
	vec2_t dirH, dirV;
	float intensity, threshold;
	int i;

	if (!tr.bloomFbo[0] || !tr.bloomShader[0].program || !tr.bloomShader[1].program)
		return;

	intensity = r_bloomIntensity->value;
	if (intensity <= 0.0f)
		return;

	threshold = Com_Clamp(0.0f, 0.99f, r_bloomThreshold->value);

	VectorSet2(dirH, 1.0f, 0.0f);
	VectorSet2(dirV, 0.0f, 1.0f);

	// bright pass + downsample to 1/4
	VectorSet4(color, threshold, 1.0f, 0.0f, 1.0f);
	FBO_Blit(srcFbo, srcBox, NULL, tr.bloomFbo[0], NULL, &tr.bloomShader[0], color, 0);

	for (i = 0; i < 4; i += 2)
	{
		if (i)
		{
			// plain downsample of the blurred 1/4 level to 1/16
			VectorSet4(color, 0.0f, 0.0f, 0.0f, 0.0f);
			FBO_Blit(tr.bloomFbo[0], NULL, NULL, tr.bloomFbo[2], NULL, &tr.bloomShader[0], color, 0);
		}

		FBO_Blit(tr.bloomFbo[i], NULL, dirH, tr.bloomFbo[i + 1], NULL, &tr.bloomShader[1], NULL, 0);
		FBO_Blit(tr.bloomFbo[i + 1], NULL, dirV, tr.bloomFbo[i], NULL, &tr.bloomShader[1], NULL, 0);
	}

	// add both levels to the image
	VectorSet4(color, intensity * 0.8f, intensity * 0.8f, intensity * 0.8f, 1.0f);
	FBO_Blit(tr.bloomFbo[0], NULL, NULL, srcFbo, srcBox, NULL, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);
	VectorSet4(color, intensity * 0.4f, intensity * 0.4f, intensity * 0.4f, 1.0f);
	FBO_Blit(tr.bloomFbo[2], NULL, NULL, srcFbo, srcBox, NULL, color, GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE);
}


/*
=============
RB_Fxaa

Q3Rally: FXAA on the final image (after tone mapping and bloom, before the
2D drawing).
=============
*/
void RB_Fxaa(FBO_t *srcFbo, ivec4_t srcBox)
{
	if (!tr.fxaaShader.program || !tr.screenScratchFbo)
		return;

	FBO_Blit(srcFbo, srcBox, NULL, tr.screenScratchFbo, srcBox, &tr.fxaaShader, NULL, 0);
	FBO_FastBlit(tr.screenScratchFbo, srcBox, srcFbo, srcBox, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

/*
=============
Q3Rally: camera motion blur

Every pixel of the main view is moved back to where it was on the screen in
the previous frame (depth + the previous view-projection matrix) and the
image is blurred along that way. Only the camera motion counts, so the car
in front of the chase camera (R_ComputeMotionMask) and anything nearer than
r_motionBlurNear is left sharp and is not smeared into the surroundings. The blur length is a share of a
1/60 s frame (r_motionBlur, independent of the frame rate) and goes up to
r_motionBlurNos while the viewed car burns NOS (RDF_NOS from the cgame).
=============
*/
#define MOTIONBLUR_NOS_FADE		300.0f	// ms to blend in and out of NOS

static qboolean Mat4InverseD( const double m[16], double inv[16] )
{
	double det;
	int i;

	inv[0] = m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15] + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] = -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15] - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] = m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15] + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14] - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
	inv[1] = -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15] - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] = m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15] + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] = -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15] - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] = m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14] + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] = m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15] + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] = -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15] - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] = m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15] + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14] - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] = -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11] - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] = m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11] + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11] - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] = m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10] + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

	det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
	if ( fabs( det ) < 1e-30 )
		return qfalse;

	det = 1.0 / det;
	for ( i = 0; i < 16; i++ )
		inv[i] *= det;
	return qtrue;
}

// column-major, out = a * b
static void Mat4MultiplyD( const double a[16], const double b[16], double out[16] )
{
	int i, j;

	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ )
			out[j * 4 + i] = a[i] * b[j * 4] + a[4 + i] * b[j * 4 + 1] + a[8 + i] * b[j * 4 + 2] + a[12 + i] * b[j * 4 + 3];
}

void RB_MotionBlur(FBO_t *srcFbo, ivec4_t srcBox)
{
	static double	prevViewProj[16];
	static vec3_t	prevOrigin;
	static int		prevTime;
	static qboolean	prevValid = qfalse;
	static float	nosBlend = 0.0f;

	double		proj[16], view[16], viewProj[16], invViewProj[16], reproject[16];
	mat4_t		reprojectF;
	vec4_t		params, viewInfo;
	int			i, dt;
	float		strength, maxLength;
	qboolean	blur;

	if (!tr.motionBlurShader.program || !tr.screenScratchFbo || !tr.renderDepthImage)
		return;

	for (i = 0; i < 16; i++)
	{
		proj[i] = backEnd.viewParms.projectionMatrix[i];
		view[i] = backEnd.viewParms.world.modelMatrix[i];
	}
	Mat4MultiplyD(proj, view, viewProj);

	dt = backEnd.refdef.time - prevTime;

	// NOS blends in and out over MOTIONBLUR_NOS_FADE ms
	if (dt > 0)
	{
		float target = (backEnd.refdef.rdflags & RDF_NOS) ? 1.0f : 0.0f;
		float step = MIN(1.0f, dt / MOTIONBLUR_NOS_FADE);

		nosBlend += (target - nosBlend) * step;
	}

	// no blur on the first frame, after a pause or hitch, a teleport or a
	// camera cut, or when the game time did not move
	blur = prevValid && dt > 0 && dt < 100 && Distance(prevOrigin, backEnd.viewParms.or.origin) < 400.0f
		&& Mat4InverseD(viewProj, invViewProj);

	if (blur)
	{
		Mat4MultiplyD(prevViewProj, invViewProj, reproject);
		for (i = 0; i < 16; i++)
			reprojectF[i] = reproject[i];

		strength = r_motionBlur->value + (MAX(r_motionBlur->value, r_motionBlurNos->value) - r_motionBlur->value) * nosBlend;
		maxLength = r_motionBlurMax->value + (MAX(r_motionBlurMax->value, r_motionBlurMaxNos->value) - r_motionBlurMax->value) * nosBlend;

		// x = scale of the screen motion of one frame (a share of a 1/60 s
		// frame, so the same at any frame rate), y = longest blur,
		// z = distance up to which the blur fades in, w = farthest view
		// distance of the car in front of the camera (0 = none)
		VectorSet4(params, strength * (1000.0f / 60.0f) / dt, maxLength, MAX(1.0f, r_motionBlurNear->value), backEnd.refdef.motionMaskFar);
		VectorSet4(viewInfo, backEnd.viewParms.zNear, backEnd.viewParms.zFar, 0.0f, 0.0f);

		GLSL_SetUniformMat4(&tr.motionBlurShader, UNIFORM_MODELMATRIX, reprojectF);
		GLSL_SetUniformVec4(&tr.motionBlurShader, UNIFORM_VIEWINFO, viewInfo);
		// screen rect of that car
		GLSL_SetUniformVec4(&tr.motionBlurShader, UNIFORM_SHADOWPARAMS, backEnd.refdef.motionMaskRect);
		GL_BindToTMU(tr.renderDepthImage, TB_LIGHTMAP);

		FBO_Blit(srcFbo, srcBox, NULL, tr.screenScratchFbo, srcBox, &tr.motionBlurShader, params, 0);
		FBO_FastBlit(tr.screenScratchFbo, srcBox, srcFbo, srcBox, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	if (dt != 0 || !prevValid)
	{
		for (i = 0; i < 16; i++)
			prevViewProj[i] = viewProj[i];
		VectorCopy(backEnd.viewParms.or.origin, prevOrigin);
		prevTime = backEnd.refdef.time;
		prevValid = qtrue;
	}
}


/*
=============
R_ParseHaze

Q3Rally: the haze of the map, from the worldspawn key "haze" or the sky
shader keyword q3r_haze:
  <r> <g> <b> <distance> [<ground distance> <ground height> <ground thickness> [<sun glow>]]
distance = units at which the haze hides half (0 = no thin haze); ground
distance = the same for the ground haze at the ground height (0 = none);
the ground haze thins out by e every ground thickness units upwards.
=============
*/
qboolean R_ParseHaze( const char *text )
{
	float v[9];
	int n;

	for ( n = 0; n < 9; n++ ) {
		v[n] = 0.0f;
	}
	n = sscanf( text, "%f %f %f %f %f %f %f %f", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7] );
	if ( n < 4 ) {
		ri.Printf( PRINT_WARNING, "WARNING: haze needs <r> <g> <b> <distance> [<ground distance> <ground height> <ground thickness> [<sun glow>]], got '%s'\n", text );
		return qfalse;
	}

	VectorSet( tr.hazeColor, v[0], v[1], v[2] );
	tr.hazeDistance = MAX( v[3], 0.0f );
	tr.hazeGroundDistance = n >= 5 ? MAX( v[4], 0.0f ) : 0.0f;
	tr.hazeGroundHeight = v[5];
	tr.hazeGroundThickness = n >= 7 ? MAX( v[6], 1.0f ) : 128.0f;
	tr.hazeSunGlow = n >= 8 ? MAX( v[7], 0.0f ) : 0.5f;
	tr.hazeSet = tr.hazeDistance > 0.0f || tr.hazeGroundDistance > 0.0f;

	return qtrue;
}

/*
=============
RB_Haze

Q3Rally: distance and ground haze over the scene, before tone mapping
(glsl/haze_fp.glsl). Main view only, needs the depth of the prepass.
=============
*/
void RB_Haze( FBO_t *dstFbo, ivec4_t dstBox )
{
	vec4_t vec, sunColor;
	float zNear, density, groundDensity, scale;

	if ( !tr.hazeShader.program || !tr.hdrDepthImage || !tr.hazeSet || r_hazeDensity->value <= 0.0f ) {
		return;
	}

	zNear = backEnd.viewParms.zNear > 0.0f ? backEnd.viewParms.zNear : r_znear->value;
	density = tr.hazeDistance > 0.0f ? 0.6931472f / tr.hazeDistance * r_hazeDensity->value : 0.0f;
	groundDensity = tr.hazeGroundDistance > 0.0f ? 0.6931472f / tr.hazeGroundDistance * r_hazeDensity->value : 0.0f;

	VectorSet4( vec, backEnd.viewParms.zFar / zNear, backEnd.viewParms.zFar,
		tan( backEnd.viewParms.fovX * M_PI / 360.0f ), tan( backEnd.viewParms.fovY * M_PI / 360.0f ) );
	GLSL_SetUniformVec4( &tr.hazeShader, UNIFORM_VIEWINFO, vec );
	GLSL_SetUniformVec3( &tr.hazeShader, UNIFORM_VIEWORIGIN, backEnd.viewParms.or.origin );
	GLSL_SetUniformVec3( &tr.hazeShader, UNIFORM_VIEWFORWARD, backEnd.viewParms.or.axis[0] );
	GLSL_SetUniformVec3( &tr.hazeShader, UNIFORM_VIEWLEFT, backEnd.viewParms.or.axis[1] );
	GLSL_SetUniformVec3( &tr.hazeShader, UNIFORM_VIEWUP, backEnd.viewParms.or.axis[2] );

	VectorSet4( vec, tr.hazeColor[0], tr.hazeColor[1], tr.hazeColor[2], 1.0f );
	GLSL_SetUniformVec4( &tr.hazeShader, UNIFORM_HAZECOLOR, vec );
	VectorSet4( vec, density, groundDensity, 1.0f / tr.hazeGroundThickness, tr.hazeGroundHeight );
	GLSL_SetUniformVec4( &tr.hazeShader, UNIFORM_HAZEPARAMS, vec );
	VectorSet4( vec, tr.sunDirection[0], tr.sunDirection[1], tr.sunDirection[2], tr.hazeSunGlow );
	GLSL_SetUniformVec4( &tr.hazeShader, UNIFORM_HAZESUN, vec );

	// the sun's colour at most 1, and how far the sky counts as
	scale = MAX( tr.sunLight[0], MAX( tr.sunLight[1], tr.sunLight[2] ) );
	if ( scale > 0.0f ) {
		VectorScale( tr.sunLight, 1.0f / scale, sunColor );
	} else {
		VectorSet( sunColor, 1.0f, 1.0f, 1.0f );
	}
	sunColor[3] = tr.hazeDistance > 0.0f ? tr.hazeDistance * 3.0f : 8000.0f;

	FBO_BlitFromTexture( tr.hdrDepthImage, NULL, NULL, dstFbo, dstBox, &tr.hazeShader, sunColor,
		GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHTEST_DISABLE );
}
