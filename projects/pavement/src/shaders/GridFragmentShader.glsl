#version 330 core

in vec4 ex_Color;

out vec4 out_Color;

uniform vec4 DefaultColor;

void main(void)
{
	out_Color = ex_Color;
}