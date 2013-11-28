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


typedef struct {
	std::vector< glm::vec4 > vertices;
	std::vector< glm::vec4 > normals;
	std::vector< unsigned int > elements;
	std::vector< int > normal_indexes;
} Model;

class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

	void parseMesh();
	void loadMeshFile(std::string filePath);
	void printMesh();
	void printVertices();
	void printNormals();
	void printElements();
	void printNormalIndexes();
	void createBufferObjects();
	void drawMesh();

	std::vector< glm::vec4 >*getVertices();
	Model getModel();


private:
	std::string meshString;
	std::vector< glm::vec4 > vertices;
	std::vector< glm::vec4 > normals;
	std::vector< unsigned int > elements;
	std::vector< int > normal_indexes;
	bool readMtl;
	GLuint VaoId, VboId[3];
};

#endif

