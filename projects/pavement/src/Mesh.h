#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <sstream> 

#include "soil\src\SOIL.h"
#include "Engine.h"

class Mesh {

	std::vector< glm::vec4 > vertices;
	std::vector< glm::vec4 > normals;
	std::vector< glm::vec2 > textures;
	bool readUV;
	GLuint VaoId, VboId[4];

public:

	Mesh(void);
	Mesh(std::string);
	Mesh(Mesh*);
	~Mesh(void);

	void draw();
	void createBufferObjects();
	void parse(std::string);
	void reverseElements();

};


#endif

