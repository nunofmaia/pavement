#include "Mesh.h"

Mesh::Mesh(void)
{
	readMtl=false;
	readUV=false;
	hasTexture=false;
}

Mesh::Mesh(Mesh* m) {
	vertices = m->vertices;
	normals = m->normals;
	textures = m->textures;
	readUV = m->readUV;
	hasTexture = m->hasTexture;
	texturePath = m->texturePath;
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
						vertice.t = 1 - vertice.t;
						textureIndices.push_back(glm::vec2(vertice));
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

void Mesh::loadMeshFile(std::string filePath,std::string texturePath = "" ){
	if(texturePath != ""){
		Mesh::texturePath = texturePath;
		hasTexture = true;
	}
	meshString = Utils::readFile(filePath);
	parse();
	//std::cerr<<"Mesh : "<<filePath<<std::endl<<" Vertices: "<<vertices.size()<<" Normals: "<<normals.size()<<" UV: "<<textures.size()<<" Vertex Indices: "<< vertexIndices.size()<<" normal Indices: "<< normalIndices.size()<<" Texture Indices: "<< textureIndices.size()<<std::endl<<std::endl;
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
	glDisableVertexAttribArray(VERTICES);
	glDisableVertexAttribArray(TEXTURES);
	glDisableVertexAttribArray(NORMALS);

	//checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void Mesh::draw(){

	glBindVertexArray(VaoId);
	if (hasTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		GLint uniform_mytexture = glGetUniformLocation(pId, "texture_uniform");
		glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0); 
	}
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
	//glDeleteTextures(1, &texture_id)
}

void Mesh::reverseElements() {
	std::reverse(vertices.begin(), vertices.end());
	std::reverse(normals.begin(), normals.end());
}

void Mesh::loadTextureFile(GLuint pId){
	
	

	if (hasTexture)
	{
		Mesh::pId=pId;

		int width,height;
		unsigned char* img = SOIL_load_image(texturePath.c_str(), &width, &height, NULL, SOIL_LOAD_RGB);
		//std::cout<<"Texture imported:"<<"Width: "<<width<<" Height: "<<height<<std::endl;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	}
}