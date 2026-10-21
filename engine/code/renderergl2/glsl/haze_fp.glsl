// Q3Rally: distance and height haze, see RB_Haze in tr_postprocess.c
//
// Drawn over the scene before tone mapping (blend SRC_ALPHA,
// ONE_MINUS_SRC_ALPHA). Every pixel is rebuilt in world space from the depth
// of the prepass. The haze has two parts: a thin one everywhere (density
// u_HazeParams.x) and a ground haze whose density falls off with the height
// (u_HazeParams.y at the height u_HazeParams.w, falling by e every
// 1 / u_HazeParams.z units); the ground part is integrated along the view
// ray, so it is thick looking along a valley and thin looking up. The sky
// only gets the haze at the horizon. Towards the sun the haze glows in the
// sun's colour. In rain or snow the sky clouds over (u_HazeWeather).

uniform sampler2D u_TextureMap;		// depth of the prepass

uniform vec4   u_ViewInfo;		// x = zfar / znear, y = zfar, z = tan(fov x / 2), w = tan(fov y / 2)
uniform vec3   u_ViewOrigin;
uniform vec3   u_ViewForward;
uniform vec3   u_ViewLeft;
uniform vec3   u_ViewUp;
uniform vec4   u_HazeColor;		// rgb, a = most it hides
uniform vec4   u_HazeParams;	// x = density, y = ground density, z = 1 / ground thickness, w = ground height
uniform vec4   u_HazeSun;		// xyz = to the sun, w = glow
uniform vec4   u_Color;			// rgb = sun colour, a = how far the sky is
uniform vec4   u_HazeWeather;	// x = how much the clouds of rain or snow cover the sky, y = time

varying vec2   var_ScreenTex;

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

void main()
{
	float d = texture2D(u_TextureMap, var_ScreenTex).r;
	bool  sky = d >= 0.99999;

	vec3  dir = u_ViewForward + (1.0 - 2.0 * var_ScreenTex.x) * u_ViewInfo.z * u_ViewLeft
	          + (2.0 * var_ScreenTex.y - 1.0) * u_ViewInfo.w * u_ViewUp;
	float len = length(dir);
	vec3  ray = dir / len;

	float z = sky ? u_Color.a : u_ViewInfo.y / mix(u_ViewInfo.x, 1.0, d);
	float dist = z * len;

	// thin haze everywhere; the sky only gets it towards the horizon.
	// Below the horizon the sky is the void under a map seen through
	// water or glass, which do not write depth: only a thin band there, or
	// the water turns into a flat wall of haze
	float tau = u_HazeParams.x * dist;
	if (sky)
		tau *= 1.0 - smoothstep(0.0, 0.35, ray.z);

	// ground haze, density g(h) = y * exp(-b * (h - h0)), integrated
	// from the eye along the ray
	if (u_HazeParams.y > 0.0)
	{
		float b = u_HazeParams.z;
		float h = u_ViewOrigin.z - u_HazeParams.w;
		float dz = ray.z * dist;
		float k = b * dz;
		float along = abs(k) > 0.001 ? (1.0 - exp(-k)) / k : 1.0;

		tau += u_HazeParams.y * exp(-clamp(b * h, -8.0, 40.0)) * dist * along;
	}

	if (sky && ray.z < 0.0)
		tau *= 1.0 - smoothstep(0.0, 0.05, -ray.z);

	float haze = (1.0 - exp(-tau)) * u_HazeColor.a;

	// rain or snow: the sky clouds over, in slowly drifting patches
	if (sky && u_HazeWeather.x > 0.0)
	{
		vec2  p = ray.xy / max(abs(ray.z), 0.15) * 3.0 + vec2(u_HazeWeather.y * 0.01, 0.0);
		float n = Noise(p) * 0.6 + Noise(p * 2.7 + 5.0) * 0.4;
		haze = max(haze, u_HazeWeather.x * (0.8 + 0.25 * n));
	}

	// glow towards the sun
	float sun = pow(max(dot(ray, u_HazeSun.xyz), 0.0), 8.0) * u_HazeSun.w;
	vec3  color = u_HazeColor.rgb + u_Color.rgb * sun;

	gl_FragColor = vec4(color, haze);
}
