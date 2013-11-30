#include "Grid.h"

Grid::Grid(int lines)
{
	NLines=lines;
}


Grid::~Grid(void)
{
}

void Grid::setNlines(int lines){
	NLines = lines;
}

void Grid::generateGrid(){
	glm::vec4 startVerticalPoint(0.0 , 0.0, -100.0, 1.0);
	glm::vec4 endVerticalPoint(0.0 , 0.0, 100.0, 1.0);
	glm::vec4 startHorizontalPoint(-100.0 , 0.0, 0.0, 1.0);
	glm::vec4 endHorizontalPoint(100.0 , 0.0, 0.0, 1.0);
	lines.push_back(startVerticalPoint);
	lines.push_back(endVerticalPoint);
	lines.push_back(startHorizontalPoint);
	lines.push_back(endHorizontalPoint);
	
	for(int i = 1; i<NLines; i++){
		lines.push_back(glm::vec4(startVerticalPoint.x+0.2*i, startVerticalPoint.y, startVerticalPoint.z, startVerticalPoint.w));
		lines.push_back(glm::vec4(endVerticalPoint.x+0.2*i, endVerticalPoint.y, endVerticalPoint.z, endVerticalPoint.w));

		lines.push_back(glm::vec4(startVerticalPoint.x-0.2*i, startVerticalPoint.y, startVerticalPoint.z, startVerticalPoint.w));
		lines.push_back(glm::vec4(endVerticalPoint.x-0.2*i, endVerticalPoint.y, endVerticalPoint.z, endVerticalPoint.w));

		lines.push_back(glm::vec4(startHorizontalPoint.x, startHorizontalPoint.y, startHorizontalPoint.z+0.2*i, startHorizontalPoint.w));
		lines.push_back(glm::vec4(endHorizontalPoint.x, endHorizontalPoint.y, endHorizontalPoint.z+0.2*i, endHorizontalPoint.w));

		lines.push_back(glm::vec4(startHorizontalPoint.x, startHorizontalPoint.y, startHorizontalPoint.z-0.2*i, startHorizontalPoint.w));
		lines.push_back(glm::vec4(endHorizontalPoint.x, endHorizontalPoint.y, endHorizontalPoint.z-0.2*i, endHorizontalPoint.w));
	}
	/*for(int i=0; i<(int)lines.size();i++){
		std::cout<<i+1<<" "<<lines[i].x<<" "<<lines[i].y<<" "<<lines[i].z<<std::endl;
	}*/
}

void Grid::createBufferObjects(){
	generateGrid();
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(4, VboId);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	glBufferData(GL_ARRAY_BUFFER, lines.size()*sizeof(glm::vec4), &lines[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(VERTICES);

	//checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void Grid::drawGrid(){
	glBindVertexArray(VaoId);
	glDrawArrays(GL_LINES, 0, lines.size());
	glBindVertexArray(0);
}