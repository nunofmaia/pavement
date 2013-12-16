#include "Mesh.h"

Mesh::Mesh(void) {
	readUV = false;
}

Mesh::Mesh(Mesh* m) {
	vertices = m->vertices;
	normals = m->normals;
	textures = m->textures;
	readUV = m->readUV;
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
			} else { 
				if (line[1] == 't') {
					vertice.t = 1 - vertice.t;
					textureIndices.push_back(glm::vec2(vertice));
					readUV=true;
				} else {
					readUV=false;
				}
			}
		} else if (line[0] == 'f') {
			int v1, v2, v3, t1, t2, t3, n1, n2, n3;
			char ignore;
			std::stringstream linestream(line.substr(2));

			if (!readUV) { //does not read UV coord
				linestream >> v1; linestream >> ignore;	linestream >> ignore; linestream >> n1; 
				linestream >> v2; linestream >> ignore;	linestream >> ignore; linestream >> n2;
				linestream >> v3; linestream >> ignore;	linestream >> ignore; linestream >> n3;

				vertices.push_back(vertexIndices[v1-1]);
				vertices.push_back(vertexIndices[v2-1]);
				vertices.push_back(vertexIndices[v3-1]);

				normals.push_back(normalIndices[n1-1]);
				normals.push_back(normalIndices[n2-1]);
				normals.push_back(normalIndices[n3-1]);
			} else {
				linestream >> v1; linestream >> ignore; linestream >> t1; linestream >> ignore; linestream >> n1; 
				linestream >> v2; linestream >> ignore;	linestream >> t2; linestream >> ignore;	linestream >> n2;
				linestream >> v3; linestream >> ignore;	linestream >> t3; linestream >> ignore;	linestream >> n3;

				vertices.push_back(vertexIndices[v1-1]);
				vertices.push_back(vertexIndices[v2-1]);
				vertices.push_back(vertexIndices[v3-1]);

				textures.push_back(textureIndices[t1-1]);
				textures.push_back(textureIndices[t2-1]);
				textures.push_back(textureIndices[t3-1]);

				normals.push_back(normalIndices[n1-1]);
				normals.push_back(normalIndices[n2-1]);
				normals.push_back(normalIndices[n3-1]);
			}
		}
	}
}


void Mesh::createBufferObjects() {

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(4, VboId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	if (readUV)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
		glBufferData(GL_ARRAY_BUFFER, textures.size()*sizeof(glm::vec2), &textures[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(TEXTURES);
		glVertexAttribPointer(TEXTURES, 2, GL_FLOAT, GL_FALSE, sizeof( glm::vec2 ), 0); 
	}

	glBindBuffer(GL_ARRAY_BUFFER, VboId[3]);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec4), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMALS);
	glVertexAttribPointer(NORMALS, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(VERTICES);
	glDisableVertexAttribArray(TEXTURES);
	glDisableVertexAttribArray(NORMALS);

	//checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void Mesh::draw() {

	glBindVertexArray(VaoId);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}

void Mesh::reverseElements() {
	std::reverse(vertices.begin(), vertices.end());
	std::reverse(normals.begin(), normals.end());
}
