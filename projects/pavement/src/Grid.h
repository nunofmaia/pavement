#ifndef GRID_H
#define GRID_H

#include <vector>

#include "Engine.h"
#include "Camera.h"
#include "SceneGraph.h"

class Grid {
public:
	Grid();
	Grid(int);
	~Grid();

	void createBufferObjects();
	void generateGrid();
	void draw();
	void highlightGrid(int);
	
	void addNode(SceneNode*);
	SceneGraph* getScene();
	Camera* getCamera();

private:
	int _linesNr;
	GLfloat _interval;
	GLuint VaoId, VboId[4];
	std::vector<glm::vec4> _lines;
	std::vector<glm::vec4> _lineColors;

	SceneGraph _scene;
	Camera *_camera;
};

#endif

