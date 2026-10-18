uniform sampler2D u_TextureMap;
uniform sampler2D u_LevelsMap;

uniform vec4      u_Color;


uniform vec2      u_AutoExposureMinMax;
uniform vec3      u_ToneMinAvgMaxLinear;

// Q3Rally: filmic tone curve and colour grading
uniform sampler2D u_ColorLutMap;
uniform vec4      u_ToneParams;		// x = curve (0 classic, 1 filmic), y = filmic exposure, z = filmic white
uniform vec4      u_LutParams;		// x = strength, y = LUT size N (0 = none)

varying vec2      var_TexCoords;
varying float     var_InvWhite;

const vec3  LUMINANCE_VECTOR =   vec3(0.2125, 0.7154, 0.0721); //vec3(0.299, 0.587, 0.114);

float FilmicTonemap(float x)
{
	const float SS  = 0.22; // Shoulder Strength
	const float LS  = 0.30; // Linear Strength
	const float LA  = 0.10; // Linear Angle
	const float TS  = 0.20; // Toe Strength
	const float TAN = 0.01; // Toe Angle Numerator
	const float TAD = 0.30; // Toe Angle Denominator

	return ((x*(SS*x+LA*LS)+TS*TAN)/(x*(SS*x+LS)+TS*TAD)) - TAN/TAD;
}

// ACES filmic curve (Narkowicz fit): a soft toe and a long shoulder, so
// bright parts roll off to white instead of clipping
vec3 ACESFilm(vec3 x)
{
	return (x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14);
}

// colour grading: a 3D LUT stored as N slices of N x N side by side (red
// across a slice, green down, blue from slice to slice)
vec3 ColorLut(vec3 c)
{
	float n = u_LutParams.y;
	float b = clamp(c.b, 0.0, 1.0) * (n - 1.0);
	float s0 = floor(b);
	float s1 = min(s0 + 1.0, n - 1.0);
	vec2  uv = vec2((clamp(c.r, 0.0, 1.0) * (n - 1.0) + 0.5) / (n * n),
	                (clamp(c.g, 0.0, 1.0) * (n - 1.0) + 0.5) / n);
	vec3  c0 = texture2D(u_ColorLutMap, uv + vec2(s0 / n, 0.0)).rgb;
	vec3  c1 = texture2D(u_ColorLutMap, uv + vec2(s1 / n, 0.0)).rgb;

	return mix(c0, c1, b - s0);
}

void main()
{
	vec4 color = texture2D(u_TextureMap, var_TexCoords) * u_Color;

#if defined(USE_PBR)
	color.rgb *= color.rgb;
#endif

	vec3 minAvgMax = texture2D(u_LevelsMap, var_TexCoords).rgb;
	vec3 logMinAvgMaxLum = clamp(minAvgMax * 20.0 - 10.0, -u_AutoExposureMinMax.y, -u_AutoExposureMinMax.x);

	float invAvgLum = u_ToneMinAvgMaxLinear.y * exp2(-logMinAvgMaxLum.y);

	color.rgb = color.rgb * invAvgLum - u_ToneMinAvgMaxLinear.xxx;
	color.rgb = max(vec3(0.0), color.rgb);

	if (u_ToneParams.x > 0.5)
	{
		// filmic, in linear light: values up to u_ToneParams.z times the
		// old white still show, u_ToneParams.y is the exposure
		float white = (u_ToneMinAvgMaxLinear.z - u_ToneMinAvgMaxLinear.x) * u_ToneParams.z;
#if defined(USE_PBR)
		vec3  lin = color.rgb;
		float linWhite = white;
#else
		vec3  lin = pow(color.rgb, vec3(2.2));
		float linWhite = pow(white, 2.2);
#endif
		lin = clamp(ACESFilm(lin * u_ToneParams.y) / ACESFilm(vec3(linWhite * u_ToneParams.y)), 0.0, 1.0);
#if defined(USE_PBR)
		color.rgb = lin;
#else
		color.rgb = pow(lin, vec3(1.0 / 2.2));
#endif
	}
	else
	{
		color.r = FilmicTonemap(color.r);
		color.g = FilmicTonemap(color.g);
		color.b = FilmicTonemap(color.b);

		color.rgb = clamp(color.rgb * var_InvWhite, 0.0, 1.0);
	}

#if defined(USE_PBR)
	color.rgb = sqrt(color.rgb);
#endif

	if (u_LutParams.y > 1.0)
	{
		color.rgb = mix(color.rgb, ColorLut(color.rgb), u_LutParams.x);	// on the final (display) colour
	}

	// add a bit of dither to reduce banding
	color.rgb += vec3(1.0/510.0 * mod(gl_FragCoord.x + gl_FragCoord.y, 2.0) - 1.0/1020.0);

	gl_FragColor = color;
}
