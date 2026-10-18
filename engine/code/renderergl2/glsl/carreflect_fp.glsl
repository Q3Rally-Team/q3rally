// Q3Rally: car paint reflection, see RB_CarReflectionStage in tr_shade.c
uniform samplerCube u_CubeMap;

uniform vec3   u_ViewOrigin;
uniform vec4   u_Color;		// x = strength, y = reflection seen straight on, z = mip level

varying vec3   var_Position;
varying vec3   var_Normal;

void main()
{
	vec3 V = normalize(u_ViewOrigin - var_Position);
	vec3 N = normalize(var_Normal);
	vec3 R = reflect(-V, N);

	// Fresnel (Schlick): the reflection gets stronger towards the edges
	float f = 1.0 - clamp(dot(N, V), 0.0, 1.0);
	float fresnel = mix(u_Color.y, 1.0, f * f * f * f * f);

	// the sides are rendered as in R_RenderCubemapSide, which stores the
	// view along -d in direction d (lightall samples reflect(E, N) as well)
	vec3 reflection = textureCubeLod(u_CubeMap, -R, u_Color.z).rgb;

	gl_FragColor = vec4(reflection * (u_Color.x * fresnel), 1.0);
}
