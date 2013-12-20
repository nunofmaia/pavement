#ifndef _SIDEBAR_H
#define _SIDEBAR_H

#include <vector>

#include "Engine.h"
#include "Camera.h"
#include "SceneGraph.h"

class Sidebar {

	GLuint VaoId;
	GLuint VboId[2];

	std::vector<glm::vec4> _vertices;
	std::vector<glm::vec4> _colors;

	SceneGraph _scene;
	Camera *_camera;

public:

	Sidebar();
	~Sidebar();

	void createBufferObjects();
	void draw();
	void addNode(SceneNode*);
	SceneGraph getScene();
};


#endif