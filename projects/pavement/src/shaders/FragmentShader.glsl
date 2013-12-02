#version 330 core

in vec4 ex_Color;
in vec3 v;
in vec3 n;

out vec4 out_Color;

void main(void) {

	vec3 normal = normalize(n);
	vec3 eye = vec3(0.0, 5.0, 5.0);
	vec3 lightSource = vec3(0.0,5.0,0.0);
	vec4 diffuseLight = vec4(0.8, 0.2, 0.4,1.0);
	vec4 ambientLight = vec4(0.1,0.1,0.1,1.0);
	vec4 specularLight = vec4(0.5,0.5,0.5,1.0);
	float shininess = 10.0; 

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

	out_Color = Idiff + Iamb + Ispec;
}
