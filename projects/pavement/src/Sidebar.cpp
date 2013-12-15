#include "Sidebar.h"

Sidebar::Sidebar() {

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


void Sidebar::draw() {
	glBindVertexArray(VaoId);
	glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
	glBindVertexArray(0);
}


