#version 330 core

in vec4 in_Position;
in vec4 in_Color;
in vec4 in_Normal;

out vec4 ex_Color;

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
	Position.z = -Position.z;
	Normal = in_Normal;
	Normal.z = -Normal.z;
	mat4 MM = ModelMatrix;
	MM[3][2] = -MM[3][2];

	mat4 RM = rotationMatrix(-Angle);
	mat4 MV = ViewMatrix * MM * RM;
	mat4 NM = transpose(inverse(MV));

	v = vec3(MV * Position);
	n = vec3(normalize(NM * Normal));

	gl_Position = ProjectionMatrix * ViewMatrix * MM * RM * Position;
	ex_Color = DefaultColor;
	
}