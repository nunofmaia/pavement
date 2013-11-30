#ifndef GRID_H
#define GRID_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <sstream> 
#include "Engine.h"


class Grid
{
public:
	Grid(int);
	~Grid(void);

	void createBufferObjects();
	void setNlines(int);
	void generateGrid();
	void drawGrid();

private:
	int NLines;
	GLuint VaoId, VboId[4];
	std::vector< glm::vec4 > lines;
};

#endif

