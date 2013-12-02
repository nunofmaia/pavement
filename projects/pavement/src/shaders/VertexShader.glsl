#version 330 core

in vec4 in_Position;
in vec4 in_Color;
in vec4 in_Normal;
out vec4 ex_Color;
out vec3 v;
out vec3 n;

uniform mat4 ModelMatrix;

vec4 Position;

layout(std140) uniform SharedMatrices
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
};

void main(void)
{	
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * in_Position;

	mat4 MV = ViewMatrix * ModelMatrix;
	mat4 NM = transpose(inverse(MV));

	v = vec3(MV * in_Position);
	n = vec3(NM * in_Normal);
	
}