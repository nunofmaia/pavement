#include "Sidebar.h"

Sidebar::Sidebar() {
	_camera = new Camera(glm::vec3(0.0 , 1.5, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), new Camera::Ortho(-0.5f, 0.5f, -1.5f, 1.5f, 2.0f, 7.0f));
	_whiteColor = glm::vec4(0.9, 0.9, 0.9, 1.0);
	_blackColor = glm::vec4(0.3, 0.3, 0.35, 1.0);
	_currentColor = _whiteColor;
}

Sidebar::~Sidebar() {

}

void Sidebar::createBufferObjects() {

	_vertices.push_back(glm::vec4(-4.0, -4.0, -2.0, 1.0));
	_vertices.push_back(glm::vec4(4.0, -4.0, -2.0, 1.0));
	_vertices.push_back(glm::vec4(-4.0, 4.0, -2.0, 1.0));
	_vertices.push_back(glm::vec4(-4.0, 4.0, -2.0, 1.0));
	_vertices.push_back(glm::vec4(4.0, -4.0, -2.0, 1.0));
	_vertices.push_back(glm::vec4(4.0, 4.0, -2.0, 1.0));

	_colors.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0));
	_colors.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0));
	_colors.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0));
	_colors.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0));
	_colors.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0));
	_colors.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0));

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(2, VboId);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size()*sizeof(glm::vec4), &_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, _colors.size()*sizeof(glm::vec4), &_colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(COLORS);
	glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(COLORS);
	glDisableVertexAttribArray(VERTICES);
}

void Sidebar::addNode(SceneNode* n) {
	_scene.addNode(n);
}

SceneGraph* Sidebar::getScene() {
	return &_scene;
}

void Sidebar::swapColors(glm::vec4 color) {

	_currentColor = color;
	
	std::vector<SceneNode*>::iterator it;
	for (it = _scene._nodes.begin(); it != _scene._nodes.end(); it++) {
		(*it)->setColor(_currentColor);
	}

}


void Sidebar::draw() {
	_camera->lookAt();
	_camera->project();

	glBindVertexArray(VaoId);
	glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
	glBindVertexArray(0);

	_scene.draw();

	_blackNode.draw();
	_whiteNode.draw();
}


