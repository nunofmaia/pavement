#version 330 core

in vec4 in_Position;
in vec4 in_Color;

out vec4 ex_Color;

layout(std140) uniform SharedMatrices
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
};

void main(void)
{	
	gl_Position = ProjectionMatrix * ViewMatrix *  in_Position;
	ex_Color = in_Color;
	
}