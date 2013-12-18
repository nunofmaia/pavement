#version 330 core

in vec4 in_Position;
in vec4 in_Color;
in vec4 in_Normal;
in vec2 in_TexCoord;

out vec4 ex_Color;
out vec2 ex_TexCoord;

out vec3 v;
out vec3 n;

uniform mat4 ModelMatrix;
uniform vec4 DefaultColor;
uniform float Angle;

vec4 Position;
vec4 Normal;

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
	Position = in_Position;
	Position.x = -Position.x;
	Normal = in_Normal;
	Normal.x = -Normal.x;
	mat4 MM = ModelMatrix;
	MM[3][0] = -MM[3][0];

	mat4 RM = rotationMatrix(-Angle);
	mat4 MV = ViewMatrix * MM * RM;
	mat4 NM = transpose(inverse(MV));

	v = vec3(MV * Position);
	n = normalize(vec3(NM * Normal));

	gl_Position = ProjectionMatrix * ViewMatrix * MM * RM * Position;
	ex_Color = DefaultColor;
	ex_TexCoord = in_TexCoord;
	
}