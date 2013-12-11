#include "Grid.h"

Grid::Grid(int lines)
{
	_lines = lines;
	_interval = 0.25f;
	generateGrid();
}

Grid::Grid(int lines, GLfloat interval) : _lines(lines), _interval(interval) {

}


Grid::~Grid(void)
{
}

void Grid::setNlines(int lines){
	_lines = lines;
}

void Grid::generateGrid(){
	glm::vec4 startVerticalPoint(0.0 , 0.0, -5.0, 1.0);
	glm::vec4 endVerticalPoint(0.0 , 0.0, 5.0, 1.0);
	glm::vec4 startHorizontalPoint(-5.0 , 0.0, 0.0, 1.0);
	glm::vec4 endHorizontalPoint(5.0 , 0.0, 0.0, 1.0);
	lines.push_back(startVerticalPoint);
	lines.push_back(endVerticalPoint);
	lines.push_back(startHorizontalPoint);
	lines.push_back(endHorizontalPoint);
	
	for(int i = 1; i < _lines; i++){
		lines.push_back(glm::vec4(startVerticalPoint.x + _interval * i, startVerticalPoint.y, startVerticalPoint.z, startVerticalPoint.w));
		lines.push_back(glm::vec4(endVerticalPoint.x+_interval*i, endVerticalPoint.y, endVerticalPoint.z, endVerticalPoint.w));

		lines.push_back(glm::vec4(startVerticalPoint.x - _interval * i, startVerticalPoint.y, startVerticalPoint.z, startVerticalPoint.w));
		lines.push_back(glm::vec4(endVerticalPoint.x - _interval * i, endVerticalPoint.y, endVerticalPoint.z, endVerticalPoint.w));

		lines.push_back(glm::vec4(startHorizontalPoint.x, startHorizontalPoint.y, startHorizontalPoint.z + _interval * i, startHorizontalPoint.w));
		lines.push_back(glm::vec4(endHorizontalPoint.x, endHorizontalPoint.y, endHorizontalPoint.z + _interval * i, endHorizontalPoint.w));

		lines.push_back(glm::vec4(startHorizontalPoint.x, startHorizontalPoint.y, startHorizontalPoint.z - _interval * i, startHorizontalPoint.w));
		lines.push_back(glm::vec4(endHorizontalPoint.x, endHorizontalPoint.y, endHorizontalPoint.z - _interval * i, endHorizontalPoint.w));
	}


	for(int i=0; i<(int)lines.size();i++){
		line_colors.push_back(glm::vec4(0.2, 0.2, 0.2, 1.0));
	}
	std::cout<<lines.size()<<" "<<line_colors.size()<<std::endl;
}

void Grid::highlightGrid(int n){
	switch(n){
	case 0:
		line_colors[0] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[1] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[2] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[3] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		break;
	case 1:
		line_colors[0] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		line_colors[1] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		line_colors[2] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[3] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		break;
	case 2:
		line_colors[0] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[1] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[2] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		line_colors[3] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		break;
	case 3:
		line_colors[0] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[1] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[2] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		line_colors[3] = glm::vec4(0.2, 0.2, 0.2, 1.0);
		break;
	case 4:
		line_colors[0] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		line_colors[1] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		line_colors[2] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		line_colors[3] = glm::vec4(0.5, 0.5, 0.5, 1.0);
		break;
	
	}
	createBufferObjects();
}

void Grid::createBufferObjects(){
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(4, VboId);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, lines.size()*sizeof(glm::vec4), &lines[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, line_colors.size()*sizeof(glm::vec4), &line_colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(COLORS);
	glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(COLORS);
	glDisableVertexAttribArray(VERTICES);
	//checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void Grid::drawGrid(){
	glBindVertexArray(VaoId);
	glDrawArrays(GL_LINES, 0, lines.size());
	glBindVertexArray(0);
}
