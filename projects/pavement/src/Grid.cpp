#include "Grid.h"

Grid::Grid() {

}

Grid::Grid(int lines) {
	_camera = new Camera(glm::vec3(0.0 , 5.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), new Camera::Perspective(30.0f, 1.0f, 2.0f, 20.0f));
	_linesNr = lines;
	_interval = 0.25f;
	generateGrid();
}

Grid::~Grid() {

}

void Grid::generateGrid() {
	glm::vec4 startVerticalPoint(0.0, 0.0, -5.0, 1.0);
	glm::vec4 endVerticalPoint(0.0, 0.0, 5.0, 1.0);
	glm::vec4 startHorizontalPoint(-5.0, 0.0, 0.0, 1.0);
	glm::vec4 endHorizontalPoint(5.0, 0.0, 0.0, 1.0);
	// Z axis
	_lines.push_back(startVerticalPoint);
	_lines.push_back(endVerticalPoint);
	// X axis
	_lines.push_back(startHorizontalPoint);
	_lines.push_back(endHorizontalPoint);
	
	for (int i = 1; i < _linesNr / 2; i++) {
		//Vertical right
		_lines.push_back(glm::vec4(startVerticalPoint.x + _interval * i, startVerticalPoint.y, startVerticalPoint.z, startVerticalPoint.w));
		_lines.push_back(glm::vec4(endVerticalPoint.x + _interval * i, endVerticalPoint.y, endVerticalPoint.z, endVerticalPoint.w));
		//vertical left
		_lines.push_back(glm::vec4(startVerticalPoint.x - _interval * i, startVerticalPoint.y, startVerticalPoint.z, startVerticalPoint.w));
		_lines.push_back(glm::vec4(endVerticalPoint.x - _interval * i, endVerticalPoint.y, endVerticalPoint.z, endVerticalPoint.w));
		//Horizontal front
		_lines.push_back(glm::vec4(startHorizontalPoint.x, startHorizontalPoint.y, startHorizontalPoint.z + _interval * i, startHorizontalPoint.w));
		_lines.push_back(glm::vec4(endHorizontalPoint.x, endHorizontalPoint.y, endHorizontalPoint.z + _interval * i, endHorizontalPoint.w));
		//Horizontal back
		_lines.push_back(glm::vec4(startHorizontalPoint.x, startHorizontalPoint.y, startHorizontalPoint.z - _interval * i, startHorizontalPoint.w));
		_lines.push_back(glm::vec4(endHorizontalPoint.x, endHorizontalPoint.y, endHorizontalPoint.z - _interval * i, endHorizontalPoint.w));

	}


	for (int i = 0; i < (int)_lines.size() ; i++) {
		_lineColors.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0));
	}
}

void Grid::highlightGrid(int n) {
	switch(n){
	case 0:
		_lineColors[0] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[1] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[2] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[3] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		break;
	case 1:
		_lineColors[0] = glm::vec4(0.7, 0.7, 0.1, 1.0);
		_lineColors[1] = glm::vec4(0.7, 0.7, 0.1, 1.0);
		_lineColors[2] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[3] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		break;
	case 2:
		_lineColors[0] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[1] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[2] = glm::vec4(0.7, 0.7, 0.1, 1.0);
		_lineColors[3] = glm::vec4(0.7, 0.7, 0.1, 1.0);
		break;
	case 3:
		_lineColors[0] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[1] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[2] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		_lineColors[3] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		break;
	case 4:
		_lineColors[0] = glm::vec4(0.7, 0.7, 0.1, 1.0);
		_lineColors[1] = glm::vec4(0.7, 0.7, 0.1, 1.0);
		_lineColors[2] = glm::vec4(0.7, 0.7, 0.1, 1.0);
		_lineColors[3] = glm::vec4(0.7, 0.7, 0.1, 1.0);
		break;
	
	}
	createBufferObjects();
}

void Grid::createBufferObjects() {
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(4, VboId);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, _lines.size() * sizeof(glm::vec4), &_lines[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, _lineColors.size()*sizeof(glm::vec4), &_lineColors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(COLORS);
	glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(COLORS);
	glDisableVertexAttribArray(VERTICES);
	//checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void Grid::draw() {
	_camera->lookAt();
	_camera->project();

	glBindVertexArray(VaoId);
	glDrawArrays(GL_LINES, 0, _lines.size());
	glBindVertexArray(0);

	_scene.draw();
}

void Grid::addNode(SceneNode* n) {
	_scene.addNode(n);
}

SceneGraph Grid::getScene() {
	return _scene;
}

Camera* Grid::getCamera() {
	return _camera;
}
