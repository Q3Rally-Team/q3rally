// Q3Rally: bloom passes. Without USE_BLUR: 4x downsample (16 texels from
// 4 bilinear taps) with a bright pass, u_Color.x = threshold, u_Color.w = 0
// for a plain downsample. With USE_BLUR: 9-tap gaussian
// in 5 bilinear taps along u_InvTexRes (texel size times direction).
uniform sampler2D u_DiffuseMap;
uniform vec4      u_Color;
uniform vec2      u_InvTexRes;

varying vec2      var_Tex1;

// glow weight rises from the threshold to full white, squared, so clipped
// highlights (lamps, lava, sun glints) glow much more than a merely bright
// wall; weighted per tap, so a small lamp is not averaged away first
vec3 Bright(vec3 c)
{
	if (u_Color.w <= 0.0)
		return c;

	float br = max(c.r, max(c.g, c.b));
	float w = smoothstep(u_Color.x, 1.0, br);
	return c * (w * w);
}

void main()
{
#if defined(USE_BLUR)
	vec3 c = texture2D(u_DiffuseMap, var_Tex1).rgb * 0.2270270;
	vec2 o1 = u_InvTexRes * 1.3846154;
	vec2 o2 = u_InvTexRes * 3.2307692;
	c += texture2D(u_DiffuseMap, var_Tex1 + o1).rgb * 0.3162162;
	c += texture2D(u_DiffuseMap, var_Tex1 - o1).rgb * 0.3162162;
	c += texture2D(u_DiffuseMap, var_Tex1 + o2).rgb * 0.0702703;
	c += texture2D(u_DiffuseMap, var_Tex1 - o2).rgb * 0.0702703;
	gl_FragColor = vec4(c, 1.0);
#else
	vec3 c = Bright(texture2D(u_DiffuseMap, var_Tex1 + u_InvTexRes * vec2(-1.0, -1.0)).rgb);
	c += Bright(texture2D(u_DiffuseMap, var_Tex1 + u_InvTexRes * vec2( 1.0, -1.0)).rgb);
	c += Bright(texture2D(u_DiffuseMap, var_Tex1 + u_InvTexRes * vec2(-1.0,  1.0)).rgb);
	c += Bright(texture2D(u_DiffuseMap, var_Tex1 + u_InvTexRes * vec2( 1.0,  1.0)).rgb);
	gl_FragColor = vec4(c * 0.25, 1.0);
#endif
}
