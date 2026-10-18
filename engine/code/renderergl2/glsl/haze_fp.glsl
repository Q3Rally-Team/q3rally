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
// sun's colour.

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

varying vec2   var_ScreenTex;

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

	// thin haze everywhere; the sky only gets it towards the horizon
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

	float haze = (1.0 - exp(-tau)) * u_HazeColor.a;

	// glow towards the sun
	float sun = pow(max(dot(ray, u_HazeSun.xyz), 0.0), 8.0) * u_HazeSun.w;
	vec3  color = u_HazeColor.rgb + u_Color.rgb * sun;

	gl_FragColor = vec4(color, haze);
}
