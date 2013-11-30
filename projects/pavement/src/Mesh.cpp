#include "Mesh.h"

Mesh::Mesh(void)
{
	readMtl=false;
}

Mesh::Mesh(int id)
{
	readMtl=false;
	_id = id;
}

Mesh::~Mesh(void)
{
}

void Mesh::parse(){
	std::string data;
	std::stringstream meshstream(meshString);
	while(getline(meshstream,data)){
		if(data[0]=='v'){
			//std::cout<<"Vertice: "<<data<<std::endl;
			std::stringstream linestream(data.substr(2));
			glm::vec3 vertice;
			linestream>>vertice.x;
			linestream>>vertice.y;
			linestream>>vertice.z;
			if(data[1]==' ')
				vertexIndices.push_back(glm::vec4(vertice,1.0f));
			else
				normalIndices.push_back(glm::vec4(vertice,1.0f));
		}
		if(data[0]=='f'){
			std::stringstream linestream(data.substr(2));
			if(!readMtl){
				int i1,i2,i3,n1,n2,n3;
				char ignore;
				linestream>>i1;	linestream>>ignore;	linestream>>ignore;	linestream>>n1; //TODO: read texture coord when needed l8r
				linestream>>i2;	linestream>>ignore;	linestream>>ignore;	linestream>>n2;
				linestream>>i3;	linestream>>ignore;	linestream>>ignore;	linestream>>n3;
				vertices.push_back(vertexIndices[i1-1]);
				vertices.push_back(vertexIndices[i2-1]);
				vertices.push_back(vertexIndices[i3-1]);
				normals.push_back(normalIndices[n1-1]);
				normals.push_back(normalIndices[n2-1]);
				normals.push_back(normalIndices[n3-1]);
				//std::cout<<"f1: "<<i1<<" n1: "<<n1<<std::endl;
			}
		}
		if(data.substr(0,6) == "usemtl")
			readMtl=true;
	}

}

void Mesh::loadMeshFile(std::string filePath){
	meshString = Utils::readFile(filePath);
	parse();
	std::cerr<<"Mesh : "<<filePath<<std::endl<<" Vertices: "<<vertices.size()<<" Normals: "<<normals.size()<<" Vertex Indices: "<< vertexIndices.size()<<" normal Indices: "<< normalIndices.size()<<std::endl<<std::endl;
}

void Mesh::printMesh(){
	std::cout<<meshString<<std::endl;
}

std::vector< glm::vec4 >*Mesh::getVertices(){
	return &vertices;
}

void Mesh::createBufferObjects(){

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(4, VboId);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[3]);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec4), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMALS);
	glVertexAttribPointer(NORMALS, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(VERTICES);
	glDisableVertexAttribArray(COLORS);

	//checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void Mesh::draw(){
	glBindVertexArray(VaoId);
	
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, _id, 0xFF);

	
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

	glDisable(GL_STENCIL_TEST);

}

void Mesh::reverseElements() {
	std::reverse(vertices.begin(), vertices.end());
	std::reverse(normals.begin(), normals.end());
}