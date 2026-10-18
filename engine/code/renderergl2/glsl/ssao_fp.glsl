// Q3Rally: screen space ambient occlusion, after Scalable Ambient
// Obscurance (McGuire, Mara, Luebke 2012). Runs at half resolution on the
// depth of the prepass (u_ScreenDepthMap, raw depth); the result is blurred
// with the depth aware blur and multiplied over the scene before tone
// mapping (RB_PostProcess).
//
// Each pixel is rebuilt in view space from the depth, its normal from the
// depth of its neighbours. Samples lie on a spiral inside a disc whose radius
// is u_AoParams.x world units; a sample occludes when it lies above the
// surface (along the normal) and close to the pixel.

uniform sampler2D u_ScreenDepthMap;

uniform vec4   u_ViewInfo;	// x = zfar / znear, y = zfar, z = tan(fov x / 2), w = tan(fov y / 2)
uniform vec4   u_AoParams;	// x = radius, y = intensity, z = bias, w = samples
uniform vec4   u_Color;		// x, y = 1 / depth texture size, z = fade start, w = fade end (world units)

varying vec2   var_ScreenTex;

#define MAX_SAMPLES 24
#define SPIRAL_TURNS 7.0

float LinearZ(vec2 tex)
{
	float d = texture2D(u_ScreenDepthMap, tex).r;
	return u_ViewInfo.y / mix(u_ViewInfo.x, 1.0, d);	// view distance in world units
}

vec3 ViewPos(vec2 tex, float z)
{
	return vec3((tex * 2.0 - 1.0) * u_ViewInfo.zw * z, z);
}

vec3 ViewPosAt(vec2 tex)
{
	return ViewPos(tex, LinearZ(tex));
}

void main()
{
	// the centre of the nearest full resolution depth texel (no depth
	// filtered across an edge)
	vec2  tex = (floor(var_ScreenTex / u_Color.xy) + 0.5) * u_Color.xy;
	float d = texture2D(u_ScreenDepthMap, tex).r;

	if (d >= 0.99999)
	{
		gl_FragColor = vec4(1.0);	// sky
		return;
	}

	float z = u_ViewInfo.y / mix(u_ViewInfo.x, 1.0, d);
	vec3  C = ViewPos(tex, z);

	// normal: of the two neighbours on each axis take the one on the same
	// surface (the smaller depth step), so edges do not get a wrong normal
	vec2  px = vec2(u_Color.x, 0.0);
	vec2  py = vec2(0.0, u_Color.y);
	vec3  r = ViewPosAt(tex + px) - C;
	vec3  l = C - ViewPosAt(tex - px);
	vec3  t = ViewPosAt(tex + py) - C;
	vec3  b = C - ViewPosAt(tex - py);
	vec3  dx = abs(r.z) < abs(l.z) ? r : l;
	vec3  dy = abs(t.z) < abs(b.z) ? t : b;
	vec3  n = normalize(cross(dy, dx));
	if (dot(n, C) > 0.0)
		n = -n;		// towards the camera

	float radius = u_AoParams.x;
	float radius2 = radius * radius;
	float bias = u_AoParams.z;

	// the disc on the screen, not too big (cache) and not under a pixel
	float discRadius = min(radius / (z * u_ViewInfo.z) * 0.5, 0.12);
	if (discRadius < u_Color.x * 2.0)
	{
		gl_FragColor = vec4(1.0);
		return;
	}

	// a different spiral angle for each pixel, the blur removes the noise
	float angle = 6.2831853 * fract(52.9829189 * fract(dot(gl_FragCoord.xy, vec2(0.06711056, 0.00583715))));

	float numSamples = u_AoParams.w;
	float sum = 0.0;
	int   i;

	for (i = 0; i < MAX_SAMPLES; i++)
	{
		if (float(i) >= numSamples)
			break;

		float alpha = (float(i) + 0.5) / numSamples;
		float a = alpha * SPIRAL_TURNS * 6.2831853 + angle;
		vec2  offset = vec2(cos(a), sin(a)) * alpha * discRadius;
		offset.y *= u_ViewInfo.z / u_ViewInfo.w;	// round on the screen

		vec3  Q = ViewPosAt(tex + offset);
		vec3  v = Q - C;
		float vv = dot(v, v);
		float vn = dot(v, n);
		float f = max(radius2 - vv, 0.0);

		sum += f * f * f * max((vn - bias) / (vv + 0.01 * radius2), 0.0);
	}

	float ao = max(0.0, 1.0 - sum * u_AoParams.y * (5.0 / numSamples) * radius / (radius2 * radius2 * radius2));

	// fade out in the distance
	ao = mix(ao, 1.0, smoothstep(u_Color.z, u_Color.w, z));

	gl_FragColor = vec4(vec3(ao), 1.0);
}
