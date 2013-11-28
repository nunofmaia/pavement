#version 330 core

in vec4 in_Position;
in vec4 in_Color;
out vec4 ex_Color;
uniform mat4 ModelMatrix;

layout(std140) uniform SharedMatrices
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
};

void main(void)
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * in_Position;
	ex_Color = (in_Position * vec4(1.0, 1.0, 1.0, 0.0));;
}