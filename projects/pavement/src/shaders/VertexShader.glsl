#version 330 core

in vec4 in_Position;
in vec4 in_Color;
in vec4 in_Normal;
in vec2 in_TexCoord;

out vec4 ex_Color;
out vec3 v;
out vec3 n;
out vec2 ex_TexCoord;

uniform mat4 ModelMatrix;
uniform vec4 DefaultColor;
uniform float Angle;

vec4 Position;

layout(std140) uniform SharedMatrices
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
};

mat4 rotationMatrix(float angle)
{    
	angle = angle * 3.14 / 180;
    return mat4( cos( angle ),  0.0, sin( angle ), 0.0,
			             0.0,           1.0, 0.0, 0.0,
			     -sin( angle ),  0.0 ,cos( angle ), 0.0,
				     0.0,           0.0, 0.0, 1.0 );
}

void main(void)
{	
	mat4 RM = rotationMatrix(Angle);
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * RM * in_Position;

	mat4 MV = ViewMatrix * ModelMatrix * RM;
	mat4 NM = transpose(inverse(MV));

	v = vec3(MV * in_Position);
	n= vec3(NM * in_Normal);
	ex_Color = DefaultColor;
	ex_TexCoord = in_TexCoord;
}