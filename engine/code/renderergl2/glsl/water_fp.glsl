// Q3Rally: q3r_water surfaces (see tr_shade.c RB_WaterPass)
//
// The mirror view of the water plane has been rendered into
// u_ScreenImageMap just before the main view. It is looked up in screen
// space and shifted by two scrolling normal map layers, then mixed with the
// water colour by a Fresnel term, so the water is see-through when looked
// at from above and a mirror at grazing angles.

uniform sampler2D u_ScreenImageMap;      // mirror view of the water plane
uniform sampler2D u_NormalMap;           // tiling water normal map

uniform vec3      u_ViewOrigin;
uniform float     u_Time;
uniform vec2      u_InvTexRes;           // 1 / reflection texture size

uniform vec4      u_Color;               // rgb water colour, a = opacity looking straight down
uniform vec4      u_NormalScale;         // 1/tile size, speed, distortion, reflection available
uniform vec4      u_SpecularScale;       // fresnel bias, fresnel power, glint strength, unused
uniform vec4      u_PrimaryLightOrigin;  // sun direction
uniform vec3      u_PrimaryLightColor;   // sun colour, 0 without q3map_sun

varying vec3      var_Position;
varying vec3      var_Normal;

vec2 WaveNormal(vec2 uv)
{
	return texture2D(u_NormalMap, uv).xy * 2.0 - 1.0;
}

void main()
{
	vec3 N = normalize(var_Normal);
	vec3 an = abs(N);
	vec2 p;
	vec3 T, B;

	// project the normal map along the main axis of the surface
	if (an.z >= an.x && an.z >= an.y)
	{
		p = var_Position.xy; T = vec3(1.0, 0.0, 0.0); B = vec3(0.0, 1.0, 0.0);
	}
	else if (an.x >= an.y)
	{
		p = var_Position.yz; T = vec3(0.0, 1.0, 0.0); B = vec3(0.0, 0.0, 1.0);
	}
	else
	{
		p = var_Position.xz; T = vec3(1.0, 0.0, 0.0); B = vec3(0.0, 0.0, 1.0);
	}

	float t  = u_Time * u_NormalScale.y;
	vec2  uv = p * u_NormalScale.x;
	vec2  w  = WaveNormal(uv + vec2(t, t * 0.6))
	         + WaveNormal(uv * 1.73 + vec2(-t * 0.71, t * 0.93));
	w *= 0.5;

	vec3  Nw    = normalize(N + (T * w.x + B * w.y) * 0.8);

	vec3  toEye = u_ViewOrigin - var_Position;
	float dist  = length(toEye);
	vec3  V     = toEye / max(dist, 0.001);

	float NdotV   = max(dot(Nw, V), 0.0);
	float fresnel = u_SpecularScale.x + (1.0 - u_SpecularScale.x) * pow(1.0 - NdotV, u_SpecularScale.y);
	fresnel = clamp(fresnel, 0.0, 1.0);

	// ripples get smaller with distance, so does the offset
	float fade  = clamp(512.0 / max(dist, 1.0), 0.2, 1.0);
	vec2  coord = gl_FragCoord.xy * u_InvTexRes + w * (u_NormalScale.z * fade);
	coord = clamp(coord, vec2(0.001), vec2(0.999));
	vec3  refl  = texture2D(u_ScreenImageMap, coord).rgb;

	vec3 color = mix(u_Color.rgb, refl, fresnel * u_NormalScale.w);

	// sun glint
	vec3  L    = normalize(u_PrimaryLightOrigin.xyz + vec3(0.0, 0.0, 0.0001));
	vec3  H    = normalize(L + V);
	float spec = pow(max(dot(Nw, H), 0.0), 300.0) * u_SpecularScale.z;
	color += u_PrimaryLightColor * spec;

	float alpha = mix(u_Color.a, 1.0, fresnel * mix(0.6, 1.0, u_NormalScale.w));
	alpha = clamp(alpha + spec, 0.0, 1.0);

	gl_FragColor = vec4(color, alpha);
}
