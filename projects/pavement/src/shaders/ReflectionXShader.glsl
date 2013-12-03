#version 330 core

in vec4 in_Position;
in vec4 in_Color;
in vec4 in_Normal;

out vec4 ex_Color;

out vec3 v;
out vec3 n;

uniform mat4 ModelMatrix;
uniform vec4 DefaultColor;

vec4 Position;
vec4 Normal;

layout(std140) uniform SharedMatrices
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
};

void main(void)
{
	Position = in_Position;
	Position.x = -Position.x;
	Normal = in_Normal;
	Normal.x = -Normal.x;
	mat4 MM = ModelMatrix;
	MM[3][0] = -MM[3][0];

	mat4 MV = ViewMatrix * MM;
	mat4 NM = transpose(inverse(MV));

	v = vec3(MV * in_Position);
	n = vec3(normalize(NM * Normal));

	gl_Position = ProjectionMatrix * ViewMatrix * MM * Position;
	ex_Color = DefaultColor;
	
}