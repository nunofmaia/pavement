#version 330 core

in vec4 ex_Color;
in vec3 v;
in vec3 n;
in vec2 ex_TexCoord;

out vec4 out_Color;

uniform sampler2D texture_uniform;

// TESTING NOISE

float f1(vec2 p){
	float a=fract(p.x*p.y);
	for(int i=0;i<5;i++){
		a=fract(123.456*a*p.y+123.456*p.x);
	}
	return a;
}

float f0(vec2 p){
	vec2 p0=vec2(floor(p.x),floor(p.y));
	vec2 p1=vec2(floor(p.x)+1.0,floor(p.y));
	vec2 p2=vec2(floor(p.x),floor(p.y)+1.0);
	vec2 p3=vec2(floor(p.x)+1.0,floor(p.y)+1.0);
	float r0=f1(p0);
	float r1=f1(p1);
	float r2=f1(p2);
	float r3=f1(p3);
	float s0=fract(p.x);
	float s1=fract(p.y);
	float a0=r0*(1.0-s0)+r1*s0;
	float a1=r2*(1.0-s0)+r3*s0;
	float a2=a0*(1.0-s1)+a1*s1;
	return a2;
}

void main(void) {
	vec3 normal = normalize(n);
	vec3 eye = vec3(0.0, 0.0, 5.0);
	vec3 lightSource = vec3(-1.0, 4.0,5.0);
	vec4 diffuseLight = ex_Color;
	vec4 ambientLight = vec4(0.1,0.1,0.1,1.0);
	vec4 specularLight = vec4(0.1, 0.1, 0.1,1.0);
	float shininess = 5.0; 
	vec3 L = normalize(lightSource - v);
	//vec3 L = normalize(-lightSource);
	//vec3 E = normalize(-v);
	vec3 E = normalize(eye - v);
	vec3 R = normalize(reflect(-L, normal));

	vec4 Idiff = max(dot(normal, L), 0.0) * diffuseLight;
	Idiff = clamp(Idiff, 0.0, 1.0);
	vec4 Iamb = ambientLight;
	vec4 Ispec = pow(max(dot(R, E), 0.0), 0.3 * shininess) * specularLight;
	Ispec = clamp(Ispec, 0.0, 1.0);
	
	//out_Color = vec4(f0(texture2D(texture_uniform, ex_TexCoord).xy * 2.0)) * (Idiff + Iamb + Ispec);
	out_Color = (Idiff + Iamb + Ispec);
	//out_Color = ex_Color;
	
}