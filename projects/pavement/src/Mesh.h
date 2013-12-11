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
#include "Engine.h"


class Mesh
{
public:
	
	std::vector< glm::vec4 > vertices;
	std::vector< glm::vec4 > normals;
	std::vector< glm::vec4 > textures;

	Mesh(void);
	Mesh(Mesh*);
	~Mesh(void);

	void setAngle(GLfloat);

	void parse();
	void loadMeshFile(std::string filePath);
	void printMesh();
	void createBufferObjects();
	void draw();

	std::vector< glm::vec4 >*getVertices();
	void reverseElements();
	void addCopy(Mesh*);
	void updateCopies();
	void setPosition(glm::vec3);

private:
	std::string meshString;

	std::vector< glm::vec4 > vertexIndices;
	std::vector< glm::vec4 > normalIndices;
	std::vector< glm::vec4 > textureIndices;
	bool readMtl,readUV;

	GLuint VaoId, VboId[4];
};


#endif

