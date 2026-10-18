// Q3Rally: camera motion blur, see RB_MotionBlur in tr_postprocess.c
uniform sampler2D u_TextureMap;
uniform sampler2D u_ScreenDepthMap;

uniform mat4   u_ModelMatrix;	// this frame's NDC -> last frame's clip space
uniform vec4   u_ViewInfo;		// x = zNear, y = zFar
uniform vec4   u_Color;			// x = blur scale, y = longest blur, z = fade-in distance, w = far distance of the car at the camera
uniform vec4   u_ShadowParams;	// screen rect of the car at the camera (min xy, max xy)
uniform vec2   u_InvTexRes;

varying vec2   var_Tex1;

#define SAMPLES 12

float BlurMask(vec2 tc, float depth)
{
	float n = u_ViewInfo.x;
	float f = u_ViewInfo.y;
	float z = 2.0 * n * f / (f + n - (2.0 * depth - 1.0) * (f - n));

	// the car in front of the camera moves with it: no blur on it
	if (z < u_Color.w && all(greaterThan(tc, u_ShadowParams.xy)) && all(lessThan(tc, u_ShadowParams.zw)))
		return 0.0;

	return smoothstep(0.0, u_Color.z, z);
}

void main()
{
	vec4 color = texture2D(u_TextureMap, var_Tex1);
	float depth = texture2D(u_ScreenDepthMap, var_Tex1).r;
	float mask = BlurMask(var_Tex1, depth);

	vec4 prev = u_ModelMatrix * vec4(var_Tex1 * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	if (mask <= 0.0 || prev.w <= 0.0)
	{
		gl_FragColor = color;
		return;
	}

	vec2 velocity = (var_Tex1 - (prev.xy / prev.w * 0.5 + 0.5)) * (u_Color.x * mask);
	float len = length(velocity);
	if (len > u_Color.y)
		velocity *= u_Color.y / len;

	// shorter than half a pixel: nothing to do
	if (len * max(1.0 / u_InvTexRes.x, 1.0 / u_InvTexRes.y) < 0.5)
	{
		gl_FragColor = color;
		return;
	}

	// samples along the motion, centred on the pixel and shifted by a
	// per-pixel noise (no visible steps on long blurs); samples on the car
	// at the camera are left out so it does not smear outwards
	float jitter = fract(52.9829189 * fract(dot(gl_FragCoord.xy, vec2(0.06711056, 0.00583715)))) - 0.5;
	vec3 sum = color.rgb;
	float weight = 1.0;
	for (int i = 0; i < SAMPLES; i++)
	{
		float t = (float(i) + 0.5 + jitter) / float(SAMPLES) - 0.5;
		vec2 tc = clamp(var_Tex1 + velocity * t, u_InvTexRes * 0.5, 1.0 - u_InvTexRes * 0.5);
		float w = BlurMask(tc, texture2D(u_ScreenDepthMap, tc).r);

		sum += texture2D(u_TextureMap, tc).rgb * w;
		weight += w;
	}

	gl_FragColor = vec4(sum / weight, color.a);
}
