#version 330 core

in vec4 ex_Color;
in vec3 v;
in vec3 n;
in vec2 ex_TexCoord;

out vec4 out_Color;

uniform sampler2D texture_uniform;

#define ONE 0.00390625
#define ONEHALF 0.001953125
// The numbers above are 1/256 and 0.5/256, change accordingly
// if you change the code to use another texture size.


/*
 * The interpolation function. This could be a 1D texture lookup
 * to get some more speed, but it's not the main part of the algorithm.
 */
float fade(float t) {
  // return t*t*(3.0-2.0*t); // Old fade, yields discontinuous second derivative
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}


/*
 * 2D classic Perlin noise. Fast, but less useful than 3D noise.
 */
float noise(vec2 P)
{
  vec2 Pi = ONE*floor(P)+ONEHALF; // Integer part, scaled and offset for texture lookup
  vec2 Pf = fract(P);             // Fractional part for interpolation

  // Noise contribution from lower left corner
  vec2 grad00 = texture2D(texture_uniform, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);

  // Noise contribution from lower right corner
  vec2 grad10 = texture2D(texture_uniform, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));

  // Noise contribution from upper left corner
  vec2 grad01 = texture2D(texture_uniform, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));

  // Noise contribution from upper right corner
  vec2 grad11 = texture2D(texture_uniform, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));

  // Blend contributions along x
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  // Blend contributions along y
  float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

  // We're done, return the final noise value.
  return n_xy;
}

float rand(vec2 co){
	float ret = fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
    return ret;
}

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
	vec3 eye = vec3(0.0, 5.0, 5.0);
	vec3 lightSource = vec3(0.0,5.0,0.0);
	vec4 diffuseLight = ex_Color;
	vec4 ambientLight = vec4(0.1,0.1,0.1,1.0);
	vec4 specularLight = vec4(0.5,0.5,0.5,1.0);
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
	float val = noise(ex_TexCoord);
	//out_Color = texture2D(texture_uniform, ex_TexCoord*val) * (Idiff + Iamb + Ispec);
	
	out_Color = vec4(f0(texture2D(texture_uniform, ex_TexCoord).xy * 2.0)) * (Idiff + Iamb + Ispec);
	//float n1 = (cnoise(textureCoordinate * scale) + 1.0) / 2.0; 
	
	}