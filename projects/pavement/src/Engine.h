#ifndef ENGINE_H
#define ENGINE_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "GL/glew.h"
#include "GL/freeglut.h"

//#include "Shader.h"
//#include "Utils.h"
#include "Mesh.h"

const float qThreshold = (float)1.0e-5;
const float vThreshold = (float)1.0e-5;
const float mThreshold = (float)1.0e-5;

typedef struct {
	GLfloat XYZW[4];
} Vertex;

#define VERTICES 0
#define COLORS 1
#define NORMALS 2
#define TEXTURES 1

#define PI 3.14159265
#define DEGREES_TO_RADIANS 0.01745329251994329547
#define RADIANS_TO_DEGREES 57.29577951308232185913

#define X 0
#define Y 1
#define Z 2
#define W 3

#define R 0
#define G 1
#define B 2
#define A 3

#endif