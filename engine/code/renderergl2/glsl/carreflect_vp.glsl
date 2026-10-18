// Q3Rally: car paint reflection, see RB_CarReflectionStage in tr_shade.c
attribute vec3 attr_Position;
attribute vec3 attr_Normal;
attribute vec3 attr_Position2;
attribute vec3 attr_Normal2;

uniform mat4   u_ModelViewProjectionMatrix;
uniform mat4   u_ModelMatrix;
uniform float  u_VertexLerp;

varying vec3   var_Position;
varying vec3   var_Normal;

void main()
{
	vec3 position = mix(attr_Position, attr_Position2, u_VertexLerp);
	vec3 normal   = mix(attr_Normal,   attr_Normal2,   u_VertexLerp);

	gl_Position  = u_ModelViewProjectionMatrix * vec4(position, 1.0);
	var_Position = (u_ModelMatrix * vec4(position, 1.0)).xyz;
	var_Normal   = (u_ModelMatrix * vec4(normal, 0.0)).xyz;
}
