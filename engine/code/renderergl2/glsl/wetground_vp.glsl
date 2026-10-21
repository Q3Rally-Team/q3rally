// Q3Rally: wet ground and snow cover, see RB_WetGroundPass in tr_shade.c
attribute vec3 attr_Position;
attribute vec3 attr_Normal;
attribute vec2 attr_TexCoord1;
attribute vec4 attr_Color;

uniform mat4   u_ModelViewProjectionMatrix;

varying vec3   var_Position;
varying vec3   var_Normal;
varying vec2   var_LightTex;
varying vec3   var_VertLight;

void main()
{
	gl_Position   = u_ModelViewProjectionMatrix * vec4(attr_Position, 1.0);
	var_Position  = attr_Position;	// world surfaces: model space is world space
	var_Normal    = attr_Normal;
	var_LightTex  = attr_TexCoord1;
	var_VertLight = attr_Color.rgb;
}
