// Q3Rally: headlight (spot light) pass, added to the lit surface
uniform sampler2D       u_DiffuseMap;
uniform sampler2DShadow u_ShadowMap;

uniform vec4   u_Color;        // light colour * intensity
uniform vec4   u_LightOrigin;  // xyz = position, w = radius
uniform vec4   u_SpotDir;      // xyz = direction, w = cos of the outer cone
uniform vec4   u_ShadowParams; // x = cos of the inner cone, y = 1 with shadow map
uniform mat4   u_ShadowMvp;

varying vec2   var_Tex;
varying vec3   var_Position;
varying vec3   var_Normal;

float PCF(const vec3 st)
{
	float s = 1.0 / r_headlightShadowMapSize;
	float m;

	m  = shadow2D(u_ShadowMap, st + vec3(-s, -s, 0.0));
	m += shadow2D(u_ShadowMap, st + vec3( s, -s, 0.0));
	m += shadow2D(u_ShadowMap, st + vec3(-s,  s, 0.0));
	m += shadow2D(u_ShadowMap, st + vec3( s,  s, 0.0));

	return m * 0.25;
}

void main()
{
	vec3 toLight = u_LightOrigin.xyz - var_Position;
	float dist = length(toLight);
	vec3 L = toLight / max(dist, 0.001);

	float cone = smoothstep(u_SpotDir.w, u_ShadowParams.x, dot(-L, u_SpotDir.xyz));
	// bright far into the beam, smoothly zero at the radius
	float fall = clamp(1.0 - (dist * dist) / (u_LightOrigin.w * u_LightOrigin.w), 0.0, 1.0);
	fall *= fall;

	// same convention as lightall: Q3's visible side is GL's back face
	vec3 N = normalize(gl_FrontFacing ? -var_Normal : var_Normal);
	// headlights sit low and hit the road at a grazing angle: plain N.L
	// would leave the road nearly black, the square root keeps it lit and
	// still darkens surfaces turned away
	float NL = sqrt(clamp(dot(N, L), 0.0, 1.0));

	float light = cone * fall * NL;
	if (light <= 0.001)
		discard;

	if (u_ShadowParams.y > 0.0)
	{
		// a little towards the light and off the surface: no self shadowing
		vec3 pos = var_Position + L * min(2.0 + dist * 0.01, 10.0) + N * 1.5;
		vec4 sp = u_ShadowMvp * vec4(pos, 1.0);

		if (sp.w > 0.0 && all(lessThan(abs(sp.xyz), vec3(sp.w))))
		{
			sp.xyz = sp.xyz * (0.5 / sp.w) + vec3(0.5);
			light *= PCF(sp.xyz);
		}
	}

	gl_FragColor = vec4(texture2D(u_DiffuseMap, var_Tex).rgb * u_Color.rgb * light, 1.0);
}
