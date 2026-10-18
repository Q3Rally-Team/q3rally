// Q3Rally: lamp shadows (r_lampShadows). One full screen pass per lamp,
// multiplied into the green channel of the screen shadow mask:
// 1 = lit, lower = a model shadow takes away this lamp's share of the light.
uniform sampler2D u_ScreenDepthMap;

uniform sampler2DShadow u_ShadowMap;   // models seen from the lamp
uniform sampler2DShadow u_ShadowMap2;  // the map seen from the lamp

uniform mat4   u_ShadowMvp;
uniform vec4   u_ShadowParams; // xyz = lamp origin, w = light * LAMP_LIGHT_SCALE
uniform vec4   u_Color;        // x = shadow strength * fade

uniform vec3   u_ViewOrigin;
uniform vec4   u_ViewInfo; // zfar / znear, zfar

varying vec2   var_DepthTex;
varying vec3   var_ViewDir;

// depth is GL_DEPTH_COMPONENT24
#define DEPTH_MAX_ERROR 0.000000059604644775390625

float PCF(const sampler2DShadow shadowmap, const vec2 st, const float dist)
{
	float scale = 1.5 / r_lampShadowMapSize;
	float mult;

	mult  = shadow2D(shadowmap, vec3(st + vec2(-scale, -scale), dist));
	mult += shadow2D(shadowmap, vec3(st + vec2( scale, -scale), dist));
	mult += shadow2D(shadowmap, vec3(st + vec2(-scale,  scale), dist));
	mult += shadow2D(shadowmap, vec3(st + vec2( scale,  scale), dist));

	return mult * 0.25;
}

float getLinearDepth(sampler2D depthMap, vec2 tex, float zFarDivZNear)
{
	float sampleZDivW = texture2D(depthMap, tex).r - DEPTH_MAX_ERROR;
	return 1.0 / mix(zFarDivZNear, 1.0, sampleZDivW);
}

void main()
{
	float result = 1.0;
	float depth = getLinearDepth(u_ScreenDepthMap, var_DepthTex, u_ViewInfo.x);

	if (depth < 0.999)
	{
		vec3 pos = u_ViewOrigin + var_ViewDir * (depth - 0.5 / u_ViewInfo.x);
		vec3 toLamp = u_ShadowParams.xyz - pos;
		float d2 = dot(toLamp, toLamp);
		float lit = sqrt(min(u_ShadowParams.w / max(d2, 1.0), 1.0));

		if (lit > 0.15)
		{
			float d = sqrt(d2);
			// a few units towards the lamp: no self-shadowing, and the
			// coarse map depth does not shadow the surface it belongs to
			vec4 shadowpos = u_ShadowMvp * vec4(pos + toLamp * (min(4.0 + d * 0.02, 16.0) / d), 1.0);

			if (shadowpos.w > 0.0 && all(lessThan(abs(shadowpos.xyz), vec3(shadowpos.w))))
			{
				float model, sun;

				shadowpos.xyz = shadowpos.xyz * (0.5 / shadowpos.w) + vec3(0.5);
				model = PCF(u_ShadowMap, shadowpos.xy, shadowpos.z);

				if (model < 0.999)
				{
					// only where the lamp reaches the map (its own shadows
					// are baked into the lightmap); half the taps lit = lit
					sun = clamp(PCF(u_ShadowMap2, shadowpos.xy, shadowpos.z) * 2.0, 0.0, 1.0);
					result = 1.0 - u_Color.x * lit * (1.0 - model) * sun;
				}
			}
		}
	}

	gl_FragColor = vec4(1.0, result, 1.0, 1.0);
}
