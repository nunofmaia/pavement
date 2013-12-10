#version 330 core

in vec4 in_Position;
in vec4 in_Color;
in vec4 in_Normal;

out vec4 ex_Color;

uniform mat4 ModelMatrix;

vec4 Position;

layout(std140) uniform SharedMatrices
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
};

void main(void)
{	
	Position = in_Position;

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * Position;
	ex_Color = in_Color;
	
}