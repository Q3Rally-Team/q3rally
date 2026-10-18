uniform sampler2D u_DiffuseMap;

uniform int       u_AlphaTest;

// Q3Rally: soft particles
uniform sampler2D u_ScreenDepthMap;
uniform vec4      u_SoftParticle;	// x = fade distance (0 = off), y = blend mode, zw = 1 / depth texture size
uniform vec4      u_ViewInfo;		// x = zNear, y = zFar

varying vec2      var_DiffuseTex;

varying vec4      var_Color;


void main()
{
	vec4 color  = texture2D(u_DiffuseMap, var_DiffuseTex);

	float alpha = color.a * var_Color.a;
	if (u_AlphaTest == 1)
	{
		if (alpha == 0.0)
			discard;
	}
	else if (u_AlphaTest == 2)
	{
		if (alpha >= 0.5)
			discard;
	}
	else if (u_AlphaTest == 3)
	{
		if (alpha < 0.5)
			discard;
	}
	
	gl_FragColor.rgb = color.rgb * var_Color.rgb;
	gl_FragColor.a = alpha;

	// Q3Rally: soft particles fade out where they cut into the scene
	if (u_SoftParticle.x > 0.0)
	{
		float n = u_ViewInfo.x;
		float f = u_ViewInfo.y;
		float sceneDepth = texture2D(u_ScreenDepthMap, gl_FragCoord.xy * u_SoftParticle.zw).r;
		float sceneZ = 2.0 * n * f / (f + n - (2.0 * sceneDepth - 1.0) * (f - n));
		float fragZ  = 2.0 * n * f / (f + n - (2.0 * gl_FragCoord.z - 1.0) * (f - n));
		float fade = clamp((sceneZ - fragZ) / u_SoftParticle.x, 0.0, 1.0);

		// no depth there (copy missing or empty): leave the particle as it is
		if (sceneDepth <= 0.0)
			fade = 1.0;

		if (u_SoftParticle.y < 0.5)
			gl_FragColor.a *= fade;
		else if (u_SoftParticle.y < 1.5)
			gl_FragColor.rgb *= fade;
		else
			gl_FragColor.rgb = mix(vec3(1.0), gl_FragColor.rgb, fade);
	}
}
