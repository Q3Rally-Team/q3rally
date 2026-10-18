uniform sampler2D u_ScreenDepthMap;

uniform sampler2DShadow u_ShadowMap;
#if defined(USE_SHADOW_CASCADE)
uniform sampler2DShadow u_ShadowMap2;
uniform sampler2DShadow u_ShadowMap3;
uniform sampler2DShadow u_ShadowMap4;
#endif

uniform mat4      u_ShadowMvp;
#if defined(USE_SHADOW_CASCADE)
uniform mat4      u_ShadowMvp2;
uniform mat4      u_ShadowMvp3;
uniform mat4      u_ShadowMvp4;
#endif

uniform vec3   u_ViewOrigin;
uniform vec4   u_ViewInfo; // zfar / znear, zfar
#if defined(USE_SHADOW_MODELS_ONLY)
uniform vec4   u_ShadowParams; // xyz = map bias towards the sun, w = model bias
#endif

varying vec2   var_DepthTex;
varying vec3   var_ViewDir;

// depth is GL_DEPTH_COMPONENT24
// so the maximum error is 1.0 / 2^24
#define DEPTH_MAX_ERROR 0.000000059604644775390625

// Input: It uses texture coords as the random number seed.
// Output: Random number: [0,1), that is between 0.0 and 0.999999... inclusive.
// Author: Michael Pohoreski
// Copyright: Copyleft 2012 :-)
// Source: http://stackoverflow.com/questions/5149544/can-i-generate-a-random-number-inside-a-pixel-shader

float random( const vec2 p )
{
  // We need irrationals for pseudo randomness.
  // Most (all?) known transcendental numbers will (generally) work.
  const vec2 r = vec2(
    23.1406926327792690,  // e^pi (Gelfond's constant)
     2.6651441426902251); // 2^sqrt(2) (Gelfond-Schneider constant)
  //return fract( cos( mod( 123456789., 1e-7 + 256. * dot(p,r) ) ) );
  return mod( 123456789., 1e-7 + 256. * dot(p,r) );  
}

float PCF(const sampler2DShadow shadowmap, const vec2 st, const float dist)
{
	float mult;
	float scale = 2.0 / r_shadowMapSize;

#if 0
	// from http://http.developer.nvidia.com/GPUGems/gpugems_ch11.html
	vec2 offset = vec2(greaterThan(fract(var_DepthTex.xy * r_FBufScale * 0.5), vec2(0.25)));
	offset.y += offset.x;
	if (offset.y > 1.1) offset.y = 0.0;
	
	mult = shadow2D(shadowmap, vec3(st + (offset + vec2(-1.5,  0.5)) * scale, dist))
	     + shadow2D(shadowmap, vec3(st + (offset + vec2( 0.5,  0.5)) * scale, dist))
	     + shadow2D(shadowmap, vec3(st + (offset + vec2(-1.5, -1.5)) * scale, dist))
	     + shadow2D(shadowmap, vec3(st + (offset + vec2( 0.5, -1.5)) * scale, dist));
	 
	mult *= 0.25;
#endif

#if defined(USE_SHADOW_FILTER)
	float r = random(var_DepthTex.xy);
	float sinr = sin(r) * scale;
	float cosr = cos(r) * scale;
	mat2 rmat = mat2(cosr, sinr, -sinr, cosr);

	mult =  shadow2D(shadowmap, vec3(st + rmat * vec2(-0.7055767, 0.196515), dist));
	mult += shadow2D(shadowmap, vec3(st + rmat * vec2(0.3524343, -0.7791386), dist));
	mult += shadow2D(shadowmap, vec3(st + rmat * vec2(0.2391056, 0.9189604), dist));
  #if defined(USE_SHADOW_FILTER2)
	mult += shadow2D(shadowmap, vec3(st + rmat * vec2(-0.07580382, -0.09224417), dist));
	mult += shadow2D(shadowmap, vec3(st + rmat * vec2(0.5784913, -0.002528916), dist));
	mult += shadow2D(shadowmap, vec3(st + rmat * vec2(0.192888, 0.4064181), dist));
	mult += shadow2D(shadowmap, vec3(st + rmat * vec2(-0.6335801, -0.5247476), dist));
	mult += shadow2D(shadowmap, vec3(st + rmat * vec2(-0.5579782, 0.7491854), dist));
	mult += shadow2D(shadowmap, vec3(st + rmat * vec2(0.7320465, 0.6317794), dist));

	mult *= 0.11111;
  #else
    mult *= 0.33333;
  #endif
#else
	mult = shadow2D(shadowmap, vec3(st, dist));
#endif

	return mult;
}

float getLinearDepth(sampler2D depthMap, vec2 tex, float zFarDivZNear)
{
	float sampleZDivW = texture2D(depthMap, tex).r - DEPTH_MAX_ERROR;
	return 1.0 / mix(zFarDivZNear, 1.0, sampleZDivW);
}

#if defined(USE_SHADOW_MODELS_ONLY)
// Q3Rally (r_sunShadowCasters 1): the camera cascades 1-3 hold only models,
// cascade 4 holds only the map (rendered once per level). A model shadow is
// drawn only where the map itself is in the sun, because the map's own
// shadows are already baked into the lightmap.
float CascadeShadow(const mat4 mvp, const sampler2DShadow map, const vec4 pos, inout float found)
{
	vec4 shadowpos = mvp * pos;

	if (found > 0.5 || !all(lessThan(abs(shadowpos.xyz), vec3(abs(shadowpos.w)))))
		return 1.0;

	found = 1.0;
	shadowpos.xyz = shadowpos.xyz * (0.5 / shadowpos.w) + vec3(0.5);
	return PCF(map, shadowpos.xy, shadowpos.z);
}

void main()
{
	float result = 1.0;

	float depth = getLinearDepth(u_ScreenDepthMap, var_DepthTex, u_ViewInfo.x);
	vec3 viewPos = u_ViewOrigin + var_ViewDir * (depth - 0.5 / u_ViewInfo.x);

  #if defined(USE_SHADOW_CASCADE)
	if (depth < 0.999)
	{
		vec3 sunDir = normalize(u_ShadowParams.xyz + vec3(0.0, 0.0, 1e-6));
		vec4 modelPos = vec4(viewPos + sunDir * u_ShadowParams.w, 1.0);
		float found = 0.0;
		float model;

		model = CascadeShadow(u_ShadowMvp, u_ShadowMap, modelPos, found);
		model *= CascadeShadow(u_ShadowMvp2, u_ShadowMap2, modelPos, found);
		model *= CascadeShadow(u_ShadowMvp3, u_ShadowMap3, modelPos, found);

		if (model < 0.999)
		{
			vec4 shadowpos = u_ShadowMvp4 * vec4(viewPos + u_ShadowParams.xyz, 1.0);
			float sun;

			shadowpos.xyz = shadowpos.xyz * (0.5 / shadowpos.w) + vec3(0.5);
			// generous: half of the taps in the sun counts as sunlit, so
			// the coarse map cascade does not punch holes into car shadows
			sun = clamp(PCF(u_ShadowMap4, shadowpos.xy, shadowpos.z) * 2.0, 0.0, 1.0);

			result = 1.0 - (1.0 - model) * sun;
		}
	}
  #endif

	gl_FragColor = vec4(result, 1.0, 1.0, 1.0);	// Q3Rally: green = lamp shadows
}
#else
void main()
{
	float result;

	float depth = getLinearDepth(u_ScreenDepthMap, var_DepthTex, u_ViewInfo.x);
	vec4 biasPos = vec4(u_ViewOrigin + var_ViewDir * (depth - 0.5 / u_ViewInfo.x), 1.0);

	vec4 shadowpos = u_ShadowMvp * biasPos;

	if ( depth >= 0.999 )
	{
		result = 1.0;
	}
	else
#if defined(USE_SHADOW_CASCADE)
	if (all(lessThan(abs(shadowpos.xyz), vec3(abs(shadowpos.w)))))
#endif
	{
		shadowpos.xyz = shadowpos.xyz * (0.5 / shadowpos.w) + vec3(0.5);
		result = PCF(u_ShadowMap, shadowpos.xy, shadowpos.z);
	}
#if defined(USE_SHADOW_CASCADE)
	else
	{
		shadowpos = u_ShadowMvp2 * biasPos;

		if (all(lessThan(abs(shadowpos.xyz), vec3(abs(shadowpos.w)))))
		{
			shadowpos.xyz = shadowpos.xyz * (0.5 / shadowpos.w) + vec3(0.5);
			result = PCF(u_ShadowMap2, shadowpos.xy, shadowpos.z);
		}
		else
		{
			shadowpos = u_ShadowMvp3 * biasPos;

			if (all(lessThan(abs(shadowpos.xyz), vec3(abs(shadowpos.w)))))
			{
				shadowpos.xyz = shadowpos.xyz * (0.5 / shadowpos.w) + vec3(0.5);
				result = PCF(u_ShadowMap3, shadowpos.xy, shadowpos.z);
			}
			else
			{
				shadowpos = u_ShadowMvp4 * biasPos;
				shadowpos.xyz = shadowpos.xyz * (0.5 / shadowpos.w) + vec3(0.5);
				result = PCF(u_ShadowMap4, shadowpos.xy, shadowpos.z);
			}
		}
	}
#endif

	gl_FragColor = vec4(result, 1.0, 1.0, 1.0);	// Q3Rally: green = lamp shadows
}
#endif
