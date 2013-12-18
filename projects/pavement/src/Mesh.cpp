#include "Mesh.h"

Mesh::Mesh(void) {

}

Mesh::Mesh(Mesh* m) {
	_vertices = m->_vertices;
	_normals = m->_normals;
	_textures = m->_textures;
}

Mesh::Mesh(std::string obj) {
	std::string file = Utils::readFile(obj);
	parse(file);
}



Mesh::~Mesh(void) {

}

void Mesh::parse(std::string meshString) {

	std::string line;
	std::stringstream meshstream(meshString);

	std::vector<glm::vec4> vertexIndices;
	std::vector<glm::vec4> normalIndices;
	std::vector<glm::vec2> textureIndices;

	while (getline(meshstream, line)) {
		if (line[0]=='v') {
			std::stringstream linestream(line.substr(2));
			glm::vec3 vertice;
			linestream >> vertice.x;
			linestream >> vertice.y;
			linestream >> vertice.z;

			if (line[1] == ' ') {
				vertexIndices.push_back(glm::vec4(vertice,1.0f));
			} else if (line[1] == 'n') {
				normalIndices.push_back(glm::vec4(vertice,1.0f));
			} else if (line[1] == 't') {
				vertice.t = 1 - vertice.t;
				textureIndices.push_back(glm::vec2(vertice));
			}
		} else if (line[0] == 'f') {
			int v1, v2, v3, t1, t2, t3, n1, n2, n3;
			char ignore;
			std::stringstream linestream(line.substr(2));

			//if (!readUV) { //does not read UV coord
			//	linestream >> v1; linestream >> ignore;	linestream >> ignore; linestream >> n1; 
			//	linestream >> v2; linestream >> ignore;	linestream >> ignore; linestream >> n2;
			//	linestream >> v3; linestream >> ignore;	linestream >> ignore; linestream >> n3;

			//	_vertices.push_back(vertexIndices[v1-1]);
			//	_vertices.push_back(vertexIndices[v2-1]);
			//	_vertices.push_back(vertexIndices[v3-1]);

			//	_normals.push_back(normalIndices[n1-1]);
			//	_normals.push_back(normalIndices[n2-1]);
			//	_normals.push_back(normalIndices[n3-1]);
			//} else {
				linestream >> v1; linestream >> ignore; linestream >> t1; linestream >> ignore; linestream >> n1; 
				linestream >> v2; linestream >> ignore;	linestream >> t2; linestream >> ignore;	linestream >> n2;
				linestream >> v3; linestream >> ignore;	linestream >> t3; linestream >> ignore;	linestream >> n3;

				_vertices.push_back(vertexIndices[v1-1]);
				_vertices.push_back(vertexIndices[v2-1]);
				_vertices.push_back(vertexIndices[v3-1]);

				_textures.push_back(textureIndices[t1-1]);
				_textures.push_back(textureIndices[t2-1]);
				_textures.push_back(textureIndices[t3-1]);

				_normals.push_back(normalIndices[n1-1]);
				_normals.push_back(normalIndices[n2-1]);
				_normals.push_back(normalIndices[n3-1]);
			//}
		}
	}
}

void Mesh::draw() {
	glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
}


MeshManager::MeshManager() {

}

MeshManager::~MeshManager() {

}

void MeshManager::addMesh(int id, Mesh* m) {
	_meshes[id] = m;
}

Mesh* MeshManager::getMesh(int id) {
	std::map<int, Mesh*>::iterator it;
	it = _meshes.find(id);

	if (it->second != NULL) {
		return it->second;
	}

	return NULL;
}


