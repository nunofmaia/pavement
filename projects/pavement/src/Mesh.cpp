#include "Mesh.h"

const Vertex Color[] = {
		{ 0.0f, 0.0f, 1.0f, 1.0f }, // 2 - TOP
		{ 0.0f, 0.0f, 1.0f, 1.0f }, // 6
		{ 0.0f, 0.0f, 1.0f, 1.0f }, // 7
		{ 0.0f, 0.0f, 1.0f, 1.0f }, // 7	
		{ 0.0f, 0.0f, 1.0f, 1.0f }, // 3
		{ 0.0f, 0.0f, 1.0f, 1.0f }, // 2

		{ 0.0f, 1.0f, 0.0f, 1.0f }, // 1 - BOTTOM
		{ 0.0f, 1.0f, 0.0f, 1.0f }, // 5
		{ 0.0f, 1.0f, 0.0f, 1.0f }, // 6
		{ 0.0f, 1.0f, 0.0f, 1.0f }, // 6	
		{ 0.0f, 1.0f, 0.0f, 1.0f }, // 2
		{ 0.0f, 1.0f, 0.0f, 1.0f }, // 1

		{ 1.0f, 0.0f, 0.0f, 1.0f }, // 0 - FRONT
		{ 1.0f, 0.0f, 0.0f, 1.0f }, // 1
		{ 1.0f, 0.0f, 0.0f, 1.0f }, // 2
		{ 1.0f, 0.0f, 0.0f, 1.0f }, // 2	
		{ 1.0f, 0.0f, 0.0f, 1.0f }, // 3
		{ 1.0f, 0.0f, 0.0f, 1.0f }, // 0

		{ 0.0f, 1.0f, 1.0f, 1.0f }, // 5 - BACK
		{ 0.0f, 1.0f, 1.0f, 1.0f }, // 4
		{ 0.0f, 1.0f, 1.0f, 1.0f }, // 7
		{ 0.0f, 1.0f, 1.0f, 1.0f }, // 7	
		{ 0.0f, 1.0f, 1.0f, 1.0f }, // 6
		{ 0.0f, 1.0f, 1.0f, 1.0f }, // 5

		{ 1.0f, 0.0f, 1.0f, 1.0f }, // 4 - LEFT
		{ 1.0f, 0.0f, 1.0f, 1.0f }, // 0
		{ 1.0f, 0.0f, 1.0f, 1.0f }, // 3
		{ 1.0f, 0.0f, 1.0f, 1.0f }, // 3	
		{ 1.0f, 0.0f, 1.0f, 1.0f }, // 7
		{ 1.0f, 0.0f, 1.0f, 1.0f }, // 4

		{ 1.0f, 1.0f, 0.0f, 1.0f }, // 0 - RIGHT
		{ 1.0f, 1.0f, 0.0f, 1.0f }, // 4
		{ 1.0f, 1.0f, 0.0f, 1.0f }, // 5
		{ 1.0f, 1.0f, 0.0f, 1.0f }, // 5	
		{ 1.0f, 1.0f, 0.0f, 1.0f }, // 1
		{ 1.0f, 1.0f, 0.0f, 1.0f }  // 0
	};

Mesh::Mesh(void)
{
	readMtl=false;
}


Mesh::~Mesh(void)
{
}

void Mesh::parseMesh(){
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
				vertices.push_back(glm::vec4(vertice,1.0f));
			else
				normals.push_back(glm::vec4(vertice,1.0f));
		}
		if(data[0]=='f'){
			std::stringstream linestream(data.substr(2));
			if(!readMtl){
				int i1,i2,i3,n1,n2,n3;
				char ignore;
				linestream>>i1;	linestream>>ignore;	linestream>>ignore;	linestream>>n1;
				linestream>>i2;	linestream>>ignore;	linestream>>ignore;	linestream>>n2;
				linestream>>i3;	linestream>>ignore;	linestream>>ignore;	linestream>>n3;
				elements.push_back(i1-1);
				elements.push_back(i2-1);
				elements.push_back(i3-1);
				normal_indexes.push_back(n1-1);
				normal_indexes.push_back(n2-1);
				normal_indexes.push_back(n3-1);
				//std::cout<<"f1: "<<i1<<" n1: "<<n1<<std::endl;
			}
		}
		if(data.substr(0,6) == "usemtl")
			readMtl=true;
	}

}

void Mesh::loadMeshFile(std::string filePath){
	meshString = Utils::readFile(filePath);
	parseMesh();
	std::cerr<<"Mesh loaded: "<<filePath<<" Vertices: "<<vertices.size()<<" Normals: "<<normals.size()<<" Elements: "<<elements.size()<<std::endl;
}

void Mesh::printMesh(){
	std::cout<<meshString<<std::endl;
}

void Mesh::printVertices(){
	std::cout<<"Vertices: "<<std::endl;
	for(int i=0; i<(int)vertices.size();i++){
		std::cout<<i+1<<" "<<vertices[i].x<<" "<<vertices[i].y<<" "<<vertices[i].z<<std::endl;
	}
}

void Mesh::printNormals(){
	std::cout<<"Normals: "<<std::endl;
	for(int i=0; i<(int)normals.size();i++){
		std::cout<<i+1<<" "<<normals[i].x<<" "<<normals[i].y<<" "<<normals[i].z<<std::endl;
	}
}

void Mesh::printElements(){
	std::cout<<"Indexes: "<<std::endl;
	for(int i=0; i<(int)elements.size();i++){
		std::cout<<i+1<<" "<<elements[i]<<std::endl;
	}
}

void Mesh::printNormalIndexes(){
	std::cout<<"Normal Indexes: "<<std::endl;
	for(int i=0; i<(int)normal_indexes.size();i++){
		std::cout<<i+1<<" "<<normal_indexes[i]<<std::endl;
	}
}

std::vector< glm::vec4 >*Mesh::getVertices(){
	return &vertices;
}

Model Mesh::getModel(){
	Model tmp;
	tmp.elements=elements;
	tmp.normals=normals;
	tmp.normal_indexes=normal_indexes;
	tmp.vertices=vertices;
	return tmp;
}

void Mesh::createBufferObjects(){

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(4, VboId);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color), Color, GL_STATIC_DRAW);
	glEnableVertexAttribArray(COLORS);
	glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof(Color), 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[3]);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec4), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMALS);
	glVertexAttribPointer(NORMALS, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboId[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), &elements[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(VERTICES);
	glDisableVertexAttribArray(COLORS);

	//checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void Mesh::drawMesh(){
	glBindVertexArray(VaoId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboId[2]);
	glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT,(void*)0 );
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Mesh::reverseElements() {
	std::reverse(elements.begin(), elements.end());
}