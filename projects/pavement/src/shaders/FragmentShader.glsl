#version 330 core

in vec4 ex_Color;
in vec3 v;
in vec3 n;
in vec2 ex_TexCoord;

out vec4 out_Color;

uniform sampler2D texture_uniform;
uniform sampler2D noise_texture_uniform;

#define ONE 1/256
#define ONEHALF 0.5/256
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
  vec2 grad00 = texture2D(noise_texture_uniform, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);

  // Noise contribution from lower right corner
  vec2 grad10 = texture2D(noise_texture_uniform, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));

  // Noise contribution from upper left corner
  vec2 grad01 = texture2D(noise_texture_uniform, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));

  // Noise contribution from upper right corner
  vec2 grad11 = texture2D(noise_texture_uniform, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));

  // Blend contributions along x
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  // Blend contributions along y
  float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

  // We're done, return the final noise value.
  return n_xy;
}

void main(void) {
	//vec3 normal = normalize(n);
	vec3 normal = n;
	vec3 eye = vec3(0.0, 5.0, 5.0);
	vec3 lightSource = vec3(0.0, 5.0, 0.0);
	//vec4 diffuseLight = vec4(0.9, 0.9, 0.9, 1.0);
	vec4 diffuseLight = ex_Color;
	vec4 ambientLight = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 specularLight = vec4(0.5, 0.5, 0.5, 1.0);
	float shininess = 10.0;

	vec3 L = normalize(lightSource - v);
	vec3 E = normalize(eye - v);
	vec3 R = normalize(reflect(-L, normal));

	//vec4 Idiff = ex_Color * max(dot(normal, L), 0.0) * diffuseLight;
	vec4 Idiff = max(dot(normal, L), 0.0) * diffuseLight;
	Idiff = clamp(Idiff, 0.0, 1.0);

	vec4 Iamb = ambientLight;

	vec4 Ispec = pow(max(dot(R, E), 0.0), shininess) * specularLight;
	Ispec = clamp(Ispec, 0.0, 1.0);

	float noise_val = cos(texture2D(noise_texture_uniform,ex_TexCoord).r);
	//float noise_val = noise(ex_TexCoord);

	out_Color = texture2D(texture_uniform,ex_TexCoord) * noise_val * Idiff + (Iamb + Ispec);

}