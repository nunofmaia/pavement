#include "Mesh.h"

Mesh::Mesh(void)
{
	readMtl=false;
	readUV=false;
}

Mesh::Mesh(int id)
{
	readMtl=false;
	readUV=false;
	_id = id;
	_isCopy = false;
	_canDraw = true;
	_color = glm::vec4(1.0, 0.98, 0.92, 1.0);
	_angle = 0.0f;
}

Mesh::Mesh(Mesh* m) {
	_id = m->_id;
	_position = m->_position;
	_isCopy = true;
	vertices = m->vertices;
	normals = m->normals;
	_color = m->_color;
	_angle = m->_angle;
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
				if(data[1] == 'n')
					normalIndices.push_back(glm::vec4(vertice,1.0f));
				else 
					if(data[1] == 't'){
						textureIndices.push_back(glm::vec4(vertice,1.0f));
						readUV=true;
					}
					else readUV=false;
		}

		if(data[0]=='f'){
			int v1,v2,v3,t1,t2,t3,n1,n2,n3;
			char ignore;
			std::stringstream linestream(data.substr(2));
			if(!readUV){ //does not read UV coord
				linestream>>v1;	linestream>>ignore; linestream>>ignore; linestream>>n1; 
				linestream>>v2;	linestream>>ignore;	linestream>>ignore; linestream>>n2;
				linestream>>v3;	linestream>>ignore;	linestream>>ignore; linestream>>n3;
				
				vertices.push_back(vertexIndices[v1-1]);
				vertices.push_back(vertexIndices[v2-1]);
				vertices.push_back(vertexIndices[v3-1]);

				normals.push_back(normalIndices[n1-1]);
				normals.push_back(normalIndices[n2-1]);
				normals.push_back(normalIndices[n3-1]);
				//std::cout<<"f1: "<<i1<<" n1: "<<n1<<std::endl;
			}
			else{
				linestream>>v1;	linestream>>ignore; linestream>>t1; linestream>>ignore; linestream>>n1; 
				linestream>>v2;	linestream>>ignore;	linestream>>t2; linestream>>ignore;	linestream>>n2;
				linestream>>v3;	linestream>>ignore;	linestream>>t3; linestream>>ignore;	linestream>>n3;
				
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
		if(data.substr(0,6) == "usemtl")
			readMtl=true;
	}

}

void Mesh::setColor(glm::vec4 color){
	_color = color;
	std::vector<Mesh*>::iterator it;
	for (it = _copies.begin(); it != _copies.end(); it++) {
		(*it)->_color = color;
	}
}

void Mesh::loadMeshFile(std::string filePath){
	meshString = Utils::readFile(filePath);
	parse();
	std::cerr<<"Mesh : "<<filePath<<std::endl<<" Vertices: "<<vertices.size()<<" Normals: "<<normals.size()<<" UV: "<<textures.size()<<" Vertex Indices: "<< vertexIndices.size()<<" normal Indices: "<< normalIndices.size()<<" Texture Indices: "<< textureIndices.size()<<std::endl<<std::endl;
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


	if(_canDraw) {

		glBindVertexArray(VaoId);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, _id, 0xFF);


		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);

		glDisable(GL_STENCIL_TEST);
	}

}

void Mesh::reverseElements() {
	std::reverse(vertices.begin(), vertices.end());
	std::reverse(normals.begin(), normals.end());
}

void Mesh::addCopy(Mesh* copy) {
	_copies.push_back(copy);
}

void Mesh::updateCopies() {
	std::vector<Mesh*>::iterator it;
	for (it = _copies.begin(); it != _copies.end(); it++) {
		(*it)->setPosition(_position);
	}

}

void Mesh::setPosition(glm::vec3 pos) {
	_position = pos;
	if (!_isCopy) {
		updateCopies();
	}
}

void Mesh::setAngle(GLfloat angle) {
	_angle = angle;
	std::vector<Mesh*>::iterator it;
	for (it = _copies.begin(); it != _copies.end(); it++) {
		(*it)->_angle = angle;
	}
}

void Mesh::loadTextureFile(std::string filepath){
	int width,height;
	unsigned char* img = SOIL_load_image(filepath.c_str(), &width, &height, NULL, 0);
	std::cout<<"Texture imported:"<<"Width: "<<width<<" Height: "<<height<<std::endl;
}
