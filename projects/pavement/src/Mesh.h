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
#include <map>

#include "soil\src\SOIL.h"
#include "Engine.h"

struct Mesh {

	std::vector< glm::vec4 > _vertices;
	std::vector< glm::vec4 > _normals;
	std::vector< glm::vec2 > _textures;

	Mesh(void);
	Mesh(std::string);
	Mesh(Mesh*);
	~Mesh(void);

	void draw();
	void parse(std::string);

};

class MeshManager {

	std::map<int, Mesh*> _meshes;

public:

	MeshManager();
	~MeshManager();

	void addMesh(int, Mesh*);
	Mesh* getMesh(int);
};

#endif

