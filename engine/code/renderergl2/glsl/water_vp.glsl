// Q3Rally: q3r_water surfaces (see tr_shade.c RB_WaterPass)
attribute vec3 attr_Position;
attribute vec3 attr_Normal;

uniform mat4   u_ModelViewProjectionMatrix;
uniform mat4   u_ModelMatrix;

varying vec3   var_Position;
varying vec3   var_Normal;

void main()
{
	gl_Position  = u_ModelViewProjectionMatrix * vec4(attr_Position, 1.0);
	var_Position = (u_ModelMatrix * vec4(attr_Position, 1.0)).xyz;
	var_Normal   = (u_ModelMatrix * vec4(attr_Normal, 0.0)).xyz;
}
