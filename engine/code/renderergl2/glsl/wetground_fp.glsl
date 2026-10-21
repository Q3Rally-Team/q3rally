// Q3Rally: wet ground and snow cover, see RB_WetGroundPass in tr_shade.c
//
// Drawn over a world surface after its normal stages (depth equal) with
// blendFunc GL_ONE GL_SRC_ALPHA: the alpha darkens what is already there,
// the colour adds the reflection.
//
// The reflection is a screen space ray march: the reflected view ray is
// stepped through the depth of this frame (u_ScreenDepthMap, copied after
// the depth prepass), and the colour at the hit is read from the previous
// frame (u_TextureMap, half size with mip maps), reprojected with
// u_PrevViewProj. Rays that leave the screen or find nothing fade out.
//
// Snow covers the upward facing ground where the weather reaches: patchy
// while it builds up and melts, closed when there is a lot of it. Its
// colour replaces what is there, lit by the lightmap of the surface (or
// its vertex light) and the sun shadow.

uniform sampler2D u_ScreenDepthMap;
uniform sampler2D u_TextureMap;
uniform sampler2D u_LightMap;
uniform sampler2D u_ShadowMap;

uniform mat4   u_ModelViewProjectionMatrix;
uniform mat4   u_PrevViewProj;
uniform vec3   u_ViewOrigin;
uniform vec4   u_ViewInfo;		// x = zNear, y = zFar, z = steps, w = highest mip level
uniform vec4   u_WetParams;		// x = wetness of the map, y = reflection, z = darkening, w = puddles
uniform vec4   u_Color;			// rgb = tint of the puddle water (muddy on loose ground)
uniform float  u_Time;
uniform vec4   u_Weather;		// where the sky is: x = wetness, y = snow, z = rain, w = snowfall
uniform sampler2D u_WeatherGridMap;	// sky grid: height + 32768, r = high byte, g = low byte
uniform vec4   u_WeatherGrid;	// xy = origin, z = 1 / cell, w = 1 if there is a grid
uniform vec4   u_WeatherGridSize;	// xy = cells, zw = 1 / cells
uniform vec4   u_SnowParams;	// x = 1 lightmap, 0 vertex light, y = buildup, z = sun ambient (< 0: no sun shadow), w = sparkle

varying vec3   var_Position;
varying vec3   var_Normal;
varying vec2   var_LightTex;
varying vec3   var_VertLight;

#define MAX_STEPS 40

float Hash(vec2 p)
{
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float Noise(vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);
	vec2 u = f * f * (3.0 - 2.0 * f);

	return mix(mix(Hash(i), Hash(i + vec2(1.0, 0.0)), u.x),
	           mix(Hash(i + vec2(0.0, 1.0)), Hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

// how far the weather reaches the point, 0 to 1: out under the open sky
// unless far below the highest surface of the grid cell (a bridge, a
// roof); soft over a cell, so the rain blows a little way under an edge
float CellExposure(vec2 cell, float z)
{
	vec2  uv = (clamp(cell, vec2(0.0), u_WeatherGridSize.xy - 1.0) + 0.5) * u_WeatherGridSize.zw;
	vec4  t = texture2D(u_WeatherGridMap, uv);
	float h = floor(t.r * 255.0 + 0.5) * 256.0 + floor(t.g * 255.0 + 0.5) - 32768.0;
	return smoothstep(h - 64.0, h - 40.0, z);
}

float WeatherExposure(vec3 p)
{
	if (u_WeatherGrid.w < 0.5)
		return 0.0;

	vec2 c = (p.xy - u_WeatherGrid.xy) * u_WeatherGrid.z - 0.5;
	vec2 c0 = floor(c);
	vec2 f = c - c0;

	return mix(mix(CellExposure(c0, p.z), CellExposure(c0 + vec2(1.0, 0.0), p.z), f.x),
	           mix(CellExposure(c0 + vec2(0.0, 1.0), p.z), CellExposure(c0 + vec2(1.0, 1.0), p.z), f.x), f.y);
}

float LinearZ(float depth)
{
	float n = u_ViewInfo.x;
	float f = u_ViewInfo.y;
	return 2.0 * n * f / (f + n - (2.0 * depth - 1.0) * (f - n));
}

vec3 Project(vec3 p)
{
	vec4 c = u_ModelViewProjectionMatrix * vec4(p, 1.0);
	if (c.w <= 0.001)
		return vec3(-1.0);
	return c.xyz / c.w * 0.5 + 0.5;
}

float EdgeFade(vec2 uv)
{
	vec2 e = smoothstep(vec2(0.0), vec2(0.06), uv) * smoothstep(vec2(0.0), vec2(0.06), 1.0 - uv);
	return e.x * e.y;
}

vec3 SampleScene(vec2 uv, float mip)
{
#if __VERSION__ >= 130
	return textureLod(u_TextureMap, uv, mip).rgb;
#else
	return texture2D(u_TextureMap, uv, mip).rgb;
#endif
}

// the wet ground: colour adds the reflection, alpha darkens
vec4 WetGround(float wet, float rain)
{
	vec3  N = normalize(var_Normal);
	vec3  V = normalize(var_Position - u_ViewOrigin);

	// water gathers on the flat parts: walls get a bit darker, no mirror
	float up = smoothstep(0.55, 0.85, N.z);

	// puddles: they appear once the ground is properly wet and grow with
	// it; u_WetParams.w is how much water the surface holds
	float n = Noise(var_Position.xy / 230.0) * 0.65 + Noise(var_Position.xy / 61.0) * 0.35;
	float level = 0.70 - 0.22 * u_WetParams.w * smoothstep(0.25, 1.0, wet);
	float puddle = smoothstep(level, level + 0.05, n) * step(0.25, wet) * up;

	// wet asphalt is rough: the normal wobbles a little outside the puddles
	vec2 wobble = vec2(Noise(var_Position.xy / 9.0), Noise(var_Position.yx / 9.0 + 17.0)) - 0.5;
	N = normalize(N + vec3(wobble * 0.05 * (1.0 - puddle), 0.0));

	// rain drops make rings on the puddles: one drop per cell at a time
	if (rain > 0.0 && puddle > 0.0)
	{
		vec2  cell = floor(var_Position.xy / 26.0);
		float h = Hash(cell);
		float t = fract(u_Time * (0.9 + h * 0.6) + h);
		vec2  centre = (cell + 0.25 + 0.5 * vec2(h, Hash(cell + 7.0))) * 26.0;
		vec2  d = var_Position.xy - centre;
		float dist = length(d);
		float ring = sin((dist - t * 13.0) * 1.6) * exp(-abs(dist - t * 13.0) * 0.5) * (1.0 - t);
		N = normalize(N + vec3(d / max(dist, 0.01) * ring * 0.18 * rain * puddle, 0.0));
	}

	float cosT = clamp(-dot(V, N), 0.0, 1.0);
	float fresnel = 0.03 + 0.97 * pow(1.0 - cosT, 5.0);

	// the surface itself reflects by its material, a puddle is water on
	// any ground and reflects like a mirror
	float reflection = wet * up * mix(u_WetParams.y * 0.45, 1.0, puddle) * fresnel;
	float dark = u_WetParams.z * wet * mix(0.5, 1.0, up);

	vec3 color = vec3(0.0);
	float found = 0.0;

	if (reflection > 0.002)
	{
		vec3  R = reflect(V, N);
		vec3  p = var_Position + N * 1.5;
		vec3  last = p;
		float stepLen = 4.0 + LinearZ(gl_FragCoord.z) * 0.01;
		float jitter = Hash(gl_FragCoord.xy);
		float mip = mix(u_ViewInfo.w * 0.6, 0.0, puddle * puddle);
		vec2  lastUV = vec2(-1.0);
		bool  hit = false;
		bool  blocked = false;
		int   i;

		p += R * stepLen * jitter;

		for (i = 0; i < MAX_STEPS; i++)
		{
			if (float(i) >= u_ViewInfo.z)
				break;

			last = p;
			p += R * stepLen;
			stepLen *= 1.18;

			vec3 s = Project(p);
			if (s.x < 0.0 || s.x > 1.0 || s.y < 0.0 || s.y > 1.0 || s.z < 0.0 || s.z > 1.0)
				break;
			lastUV = s.xy;

			float sceneDepth = texture2D(u_ScreenDepthMap, s.xy).r;
			if (s.z > sceneDepth)
			{
				// behind something: a hit if it is not far behind it
				if (LinearZ(s.z) - LinearZ(sceneDepth) < stepLen * 2.0 + 16.0)
				{
					vec3 a = last;
					vec3 b = p;
					int j;

					for (j = 0; j < 5; j++)
					{
						vec3 m = (a + b) * 0.5;
						vec3 sm = Project(m);
						if (sm.z > texture2D(u_ScreenDepthMap, sm.xy).r)
							b = m;
						else
							a = m;
					}
					p = b;
					hit = true;
				}
				else
				{
					blocked = true;		// passed behind a thick object
				}
				break;
			}
		}

		if (hit)
		{
			vec4 prev = u_PrevViewProj * vec4(p, 1.0);
			if (prev.w > 0.001)
			{
				vec2 uv = prev.xy / prev.w * 0.5 + 0.5;
				vec3 s = Project(p);

				found = EdgeFade(uv) * EdgeFade(s.xy);
				color = SampleScene(clamp(uv, 0.0, 1.0), mip);
			}
		}
		else if (!blocked && lastUV.x >= 0.0 && R.z > 0.0
			&& (i < MAX_STEPS && float(i) < u_ViewInfo.z || texture2D(u_ScreenDepthMap, lastUV).r > 0.99999))
		{
			// the ray went off the screen or ran into the sky: take the
			// colour where it left (mostly sky), blurred
			color = SampleScene(lastUV, u_ViewInfo.w);
			found = 0.6 * smoothstep(0.0, 0.2, R.z);
		}
	}

	reflection *= found;

	// muddy puddles on loose ground
	color *= mix(vec3(1.0), u_Color.rgb, puddle);

	return vec4(color * reflection, (1.0 - dark) * (1.0 - reflection));
}

// how much of the ground the snow covers, 0 to 1
float SnowCover(vec3 N, float snow)
{
	if (snow <= 0.001)
		return 0.0;

	// it stays on the flat parts, slides off the steep ones
	float up = smoothstep(0.4, 0.8, N.z);
	if (up <= 0.0)
		return 0.0;

	vec2  p = var_Position.xy;
	float s = clamp(snow * u_SnowParams.y, 0.0, 1.0);

	// drifts: the big patches first, then everything closes up
	float n = Noise(p / 310.0) * 0.55 + Noise(p / 75.0) * 0.3 + Noise(p / 13.0) * 0.15;
	float patches = smoothstep(n, n + 0.15, s * 1.3 - 0.15);

	// a thin powder from the first flakes on
	float powder = 0.5 * smoothstep(0.0, 0.25, s) * (0.6 + 0.4 * Noise(p / 4.0));

	return max(patches, powder) * up;
}

vec3 SnowColor(vec3 N)
{
	vec3 light;

	if (u_SnowParams.x > 0.5)
		light = texture2D(u_LightMap, var_LightTex).rgb;
	else
		light = var_VertLight;

	// under the clouds the baked sunlight fades, as on the ground (lightall)
	float overcast = min(1.0, max(u_Weather.z, u_Weather.w) * 1.15);
	float l = max(light.r, max(light.g, light.b));
	if (overcast > 0.0 && l > 0.45)
		light *= (0.45 + (l - 0.45) * (1.0 - 0.3 * overcast)) / l;	// less: snow scatters the light

	// the sun shadow of the map's models and the cars, and the lamp shadows
	if (u_SnowParams.z >= 0.0)
	{
		vec4 sh = texture2D(u_ShadowMap, gl_FragCoord.xy * r_FBufScale);
		light *= (sh.r * (1.0 - u_SnowParams.z) + u_SnowParams.z) * sh.g;
	}

	// wind ripples and soft drifts, so it is not one flat colour
	vec2  p = var_Position.xy;
	float drift = Noise(p / 140.0) * 0.6 + Noise(vec2(p.x + p.y * 0.3, p.y) / 23.0) * 0.4;
	vec3  albedo = vec3(0.78, 0.81, 0.87) * (0.88 + 0.12 * N.z) * (0.9 + 0.12 * drift);

	// a few ice crystals glitter close by, each one only from some
	// directions
	vec3  d = var_Position - u_ViewOrigin;
	vec3  V = normalize(d);
	vec2  cell = floor(p * 0.8);
	float glint = step(0.995, Hash(cell + floor(V.xy * 24.0) * 17.0)) * u_SnowParams.w
		* (1.0 - smoothstep(150.0, 500.0, length(d)));

	return light * (albedo + glint * 1.5);
}

void main()
{
	float wet = u_WetParams.x;
	float rain = 0.0;
	float snow = 0.0;

	// r_weatherDebug: blue where the weather reaches, red where covered
	if (u_Color.a > 0.5)
	{
		float e = WeatherExposure(var_Position);
		gl_FragColor = vec4(mix(vec3(0.5, 0.05, 0.0), vec3(0.0, 0.2, 0.6), e), 0.5);
		return;
	}

	// the weather wets the ground where the sky is above, and the snow
	// settles there
	if (u_Weather.x > 0.0 || u_Weather.y > 0.0)
	{
		float e = WeatherExposure(var_Position);

		wet = max(wet, u_Weather.x * e);
		rain = u_Weather.z * e;
		snow = u_Weather.y * e;
	}

	vec3  N = normalize(var_Normal);
	float cover = SnowCover(N, snow);

	if (wet <= 0.001 && cover <= 0.001)
	{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);	// dry here
		return;
	}

	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
	if (wet > 0.001)
		result = WetGround(wet, rain);

	if (cover > 0.001)
	{
		result.rgb = mix(result.rgb, SnowColor(N), cover);
		result.a *= 1.0 - cover;
	}

	gl_FragColor = result;
}
