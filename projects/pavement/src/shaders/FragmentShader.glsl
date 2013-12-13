#version 330 core

in vec4 ex_Color;
in vec3 v;
in vec3 n;
in vec2 ex_TexCoord;

out vec4 out_Color;

uniform sampler2D texture_uniform;

float persistence;
float octaves;


float Noise(int x, int y){
   int nn = x + y * 57;
   float a = pow((nn<<13), float(nn));
   return ( 1.0 - ( int(a * (a * a * 15731 + 789221) + 1376312589) & int(0x7fffffff)) / 1073741824.0);    
}

  float SmoothNoise(int x, int y){
	float corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
	float sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
	float center  =  Noise(x, y) / 4;
	return (corners + sides + center);
  }

  float InterpolatedNoise(float x, float y){

      int integer_X    = int(x);
      float fractional_X = x - integer_X;

      int integer_Y    = int(y);
      float fractional_Y = y - integer_Y;

     float v1 = SmoothNoise(integer_X,     integer_Y);
     float v2 = SmoothNoise(integer_X + 1, integer_Y);
     float v3 = SmoothNoise(integer_X,     integer_Y + 1);
     float v4 = SmoothNoise(integer_X + 1, integer_Y + 1);

     float i1 = mix(v1 , v2 , fractional_X);
     float i2 = mix(v3 , v4 , fractional_X);

     return mix(i1 , i2 , fractional_Y);
}

float PerlinNoise_2D(float x, float y){

      float total = 0;
	  float p = persistence;
      float n = octaves - 1;

      for(int i = 0 ; i < n ; i++){ 

          float frequency = 2*i;
          float amplitude = p*i;

          total = total + InterpolatedNoise(x * frequency, y * frequency) * amplitude;

      }

      return total;

}


void main(void) {

	vec3 normal = normalize(n);
	vec3 eye = vec3(0.0, 5.0, 5.0);
	vec3 lightSource = vec3(0.0,5.0,0.0);
	vec4 diffuseLight = ex_Color;
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

	
	out_Color = texture2D(texture_uniform, ex_TexCoord) * (Idiff + Iamb + Ispec);
}