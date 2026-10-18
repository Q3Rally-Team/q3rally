// Q3Rally: headlight (spot light) pass, see ForwardSpotLights in tr_shade.c
attribute vec3 attr_Position;
attribute vec3 attr_Normal;
attribute vec4 attr_TexCoord0;

uniform mat4   u_ModelViewProjectionMatrix;
uniform mat4   u_ModelMatrix;

uniform vec4   u_DiffuseTexMatrix0;
uniform vec4   u_DiffuseTexMatrix1;
uniform vec4   u_DiffuseTexMatrix2;
uniform vec4   u_DiffuseTexMatrix3;
uniform vec4   u_DiffuseTexMatrix4;
uniform vec4   u_DiffuseTexMatrix5;
uniform vec4   u_DiffuseTexMatrix6;
uniform vec4   u_DiffuseTexMatrix7;

varying vec2   var_Tex;
varying vec3   var_Position;
varying vec3   var_Normal;

vec2 ModTexCoords(vec2 st, vec3 position, vec4 texMatrix[8])
{
	vec2 st2 = st;
	vec2 offsetPos = vec2(position.x + position.z, position.y);

	st2 = vec2(st2.x * texMatrix[0].x + st2.y * texMatrix[0].y + texMatrix[0].z,
	           st2.x * texMatrix[1].x + st2.y * texMatrix[1].y + texMatrix[1].z);
	st2 += texMatrix[0].w * sin(offsetPos * (2.0 * M_PI / 1024.0) + vec2(texMatrix[1].w * 2.0 * M_PI));

	st2 = vec2(st2.x * texMatrix[2].x + st2.y * texMatrix[2].y + texMatrix[2].z,
	           st2.x * texMatrix[3].x + st2.y * texMatrix[3].y + texMatrix[3].z);
	st2 += texMatrix[2].w * sin(offsetPos * (2.0 * M_PI / 1024.0) + vec2(texMatrix[3].w * 2.0 * M_PI));

	st2 = vec2(st2.x * texMatrix[4].x + st2.y * texMatrix[4].y + texMatrix[4].z,
	           st2.x * texMatrix[5].x + st2.y * texMatrix[5].y + texMatrix[5].z);
	st2 += texMatrix[4].w * sin(offsetPos * (2.0 * M_PI / 1024.0) + vec2(texMatrix[5].w * 2.0 * M_PI));

	st2 = vec2(st2.x * texMatrix[6].x + st2.y * texMatrix[6].y + texMatrix[6].z,
	           st2.x * texMatrix[7].x + st2.y * texMatrix[7].y + texMatrix[7].z);
	st2 += texMatrix[6].w * sin(offsetPos * (2.0 * M_PI / 1024.0) + vec2(texMatrix[7].w * 2.0 * M_PI));

	return st2;
}

void main()
{
	vec4 texMatrix[8];

	texMatrix[0] = u_DiffuseTexMatrix0;
	texMatrix[1] = u_DiffuseTexMatrix1;
	texMatrix[2] = u_DiffuseTexMatrix2;
	texMatrix[3] = u_DiffuseTexMatrix3;
	texMatrix[4] = u_DiffuseTexMatrix4;
	texMatrix[5] = u_DiffuseTexMatrix5;
	texMatrix[6] = u_DiffuseTexMatrix6;
	texMatrix[7] = u_DiffuseTexMatrix7;

	gl_Position = u_ModelViewProjectionMatrix * vec4(attr_Position, 1.0);
	var_Tex = ModTexCoords(attr_TexCoord0.st, attr_Position, texMatrix);
	var_Position = (u_ModelMatrix * vec4(attr_Position, 1.0)).xyz;
	var_Normal = (u_ModelMatrix * vec4(attr_Normal, 0.0)).xyz;
}
