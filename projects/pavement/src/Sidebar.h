#ifndef _SIDEBAR_H
#define _SIDEBAR_H

#include <vector>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Engine.h"

class Sidebar {

	GLuint VaoId;
	GLuint VboId[2];

	std::vector<glm::vec4> _vertices;
	std::vector<glm::vec4> _colors;

	

public:

	Sidebar();
	~Sidebar();

	void createBufferObjects();
	void draw();
};


#endif