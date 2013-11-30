#include "Shader.h"

#include <iostream>

ShaderProgram::ShaderProgram(void)
{
	linked = false;
}


ShaderProgram::~ShaderProgram(void)
{

}

bool ShaderProgram::checkShaderCompilation(GLuint id, std::string name){
	GLint status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog);

		std::cerr<<name<<" : Compile failure in shader "<<id<<": "<<std::endl<<strInfoLog;
		delete[] strInfoLog;
		return false;
	}
	else
	{
		std::cerr<<"Shader "<<id<<" compiled!"<<std::endl;
		return true;
	}
}

bool ShaderProgram::checkProgramLinkage(){
	GLint result;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	if(result == GL_FALSE) {
		GLint infoLogLength;

		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(programId, infoLogLength, &result, strInfoLog);

		std::cerr<<"linkage failure in program "<<programId<<": "<<std::endl<<strInfoLog;
		delete[] strInfoLog;

		
		glDeleteProgram(programId);
		programId = 0;
		linked=false;
		return false;
	}
	linked=true;
	return true;
}

bool ShaderProgram::addShaderFromFile(std::string filePath, GLenum shaderType){
	std::string str = Utils::readFile(filePath);
	const GLchar* _shader = str.c_str();
	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId,1,&_shader,0);
	glCompileShader(shaderId);
	if(checkShaderCompilation(shaderId,filePath)){
		glAttachShader(programId,shaderId);
		return true;
	}
	else return false;
}

bool ShaderProgram::addShaderFromString(const GLchar* shader, GLenum shaderType){
	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId,1,&shader,0);
	glCompileShader(shaderId);
	if(checkShaderCompilation(shaderId, "")){
		glAttachShader(programId,shaderId);
		return true;
	}
	else return false;
}



bool ShaderProgram::linkShaderProgram(){
	glLinkProgram(programId);
	return checkProgramLinkage();
}

void ShaderProgram::useShaderProgram(){
	if(linked)
		glUseProgram(programId);
}

void ShaderProgram::bindAttribLocation(GLuint location, const char * name){
	//if(glGetAttribLocation(programId,name)==-1)
	//	cerr<<"Attribute: "<<name<<" not found."<<endl;
	//else
		glBindAttribLocation(programId,location,name);

}

void ShaderProgram::setUniform(const char *name,float x,float y, float z){
	GLint uniformLocation = glGetUniformLocation(programId,name);
	if(uniformLocation!=-1)
		glUniform3f(uniformLocation,x,y,z);
	else
		std::cerr<<"Uniform: "<<name<<" not found."<<std::endl;
}
	
void ShaderProgram::setUniform(const char *name, const glm::vec3 & v){
	GLint uniformLocation = glGetUniformLocation(programId,name);
	if(uniformLocation!=-1)
		glUniform3fv(uniformLocation,1, glm::value_ptr(v));
	else
		std::cerr<<"Uniform: "<<name<<" not found."<<std::endl;
}

void ShaderProgram::setUniform(const char *name, const glm::vec4 & v){
	GLint uniformLocation = glGetUniformLocation(programId,name);
	if(uniformLocation!=-1)
		glUniform3fv(uniformLocation,1, glm::value_ptr(v));
	else
		std::cerr<<"Uniform: "<<name<<" not found."<<std::endl;
}

void ShaderProgram::setUniform(const char *name, const glm::mat4 & m){
	GLint uniformLocation = glGetUniformLocation(programId,name);
	if(uniformLocation!=-1)
		glUniformMatrix4fv(uniformLocation,1,GL_FALSE,glm::value_ptr(m));
	else
		std::cerr<<"Uniform: "<<name<<" not found."<<std::endl;
}

void ShaderProgram::setUniform(const char *name, const glm::mat3 & m){
	GLint uniformLocation = glGetUniformLocation(programId,name);
	if(uniformLocation!=-1)
		glUniformMatrix3fv(uniformLocation,1,GL_FALSE,glm::value_ptr(m));
	else
		std::cerr<<"Uniform: "<<name<<" not found."<<std::endl;
}

void ShaderProgram::setUniform(const char *name, float val ){
	GLint uniformLocation = glGetUniformLocation(programId,name);
	if(uniformLocation!=-1)
		glUniform1f(uniformLocation,val);
	else
		std::cerr<<"Uniform: "<<name<<" not found."<<std::endl;
}

void ShaderProgram::setUniform(const char *name, int val ){
	GLint uniformLocation = glGetUniformLocation(programId,name);
	if(uniformLocation!=-1)
		glUniform1i(uniformLocation,val);
	else
		std::cerr<<"Uniform: "<<name<<" not found."<<std::endl;
}

void ShaderProgram::setUniform(const char *name, bool val ){
	GLint uniformLocation = glGetUniformLocation(programId,name);
	if(uniformLocation!=-1)
		glUniform1i(uniformLocation,val);
	else
		std::cerr<<"Uniform: "<<name<<" not found."<<std::endl;
}

void ShaderProgram::printActiveAttribs(){
	GLint totalActiveAttribs;
	int maxNameLenght;
	glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &totalActiveAttribs);
	glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLenght);
	std::cerr<<"ProgramId: "<<programId<<std::endl<<"Active Attributes: "<<std::endl;
	for(int i = 0; i<totalActiveAttribs; i++){
		GLchar *attribName = new GLchar[maxNameLenght+1];
		int lenght;
		int num;
		GLenum type;
		glGetActiveAttrib(programId,i,maxNameLenght,&lenght,&num,&type,attribName);
		std::cerr<<"NAME: "<<attribName<<" TYPE: "<<type<<" SIZE: "<<num<<std::endl;
	}	
}

void ShaderProgram::printActiveUniforms(){
	GLint totalActiveUniforms;
	int maxNameLenght;
	glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &totalActiveUniforms);
	glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLenght);
	std::cerr<<"ProgramId: "<<programId<<std::endl<<"Active Uniforms: "<<std::endl;
	for(int i = 0; i<totalActiveUniforms; i++){
		GLchar *uniformName = new GLchar[maxNameLenght+1];
		int lenght;
		int num;
		GLenum type;
		glGetActiveAttrib(programId,i,maxNameLenght,&lenght,&num,&type,uniformName);
		std::cerr<<"NAME: "<<uniformName<<" TYPE: "<<type<<" SIZE: "<<num<<std::endl;
	}	
}

int ShaderProgram::getProgramId(){
	return programId;
}

void ShaderProgram::setProgramId(){
	programId = glCreateProgram();
	std::cout<<"programid is: "<< programId<<std::endl;
}

bool ShaderProgram::isLinked(){
	return linked;
}

//void Shader::createShaderProgram(){
//
//	string vertShadFile = filereader.fileToString(VERTEXSHADERPATH);
//	const GLchar* VertexShader = vertShadFile.c_str();
//
//	string fragShadFile = filereader.fileToString(FRAGMENTSHADERPATH);
//	const GLchar* FragmentShader = fragShadFile.c_str();
//
//	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(VertexShaderId, 1, &VertexShader, 0);
//	glCompileShader(VertexShaderId);
//	myErrorChecker.checkShaderCompilation(VertexShaderId);
//
//	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(FragmentShaderId, 1, &FragmentShader, 0);
//	glCompileShader(FragmentShaderId);
//	myErrorChecker.checkShaderCompilation(FragmentShaderId);
//
//	ProgramId[0] = glCreateProgram();
//	glAttachShader(ProgramId[0], VertexShaderId);
//	glAttachShader(ProgramId[0], FragmentShaderId);
//
//	glBindAttribLocation(ProgramId[0], VERTICES, "in_Position");
//	glBindAttribLocation(ProgramId[0], COLORS, "in_Color");
//	glLinkProgram(ProgramId[0]);
//	myErrorChecker.checkProgramLinkage(ProgramId[0]);
//
//	UniformId[0] = glGetUniformLocation(ProgramId[0], "ModelMatrix");
//	UniformId[1] = glGetUniformLocation(ProgramId[0], "uniform_color");
//	UboId = glGetUniformBlockIndex(ProgramId[0], "SharedMatrices");
//	glUniformBlockBinding(ProgramId[0], UboId, UBO_BP);
//
//	myErrorChecker.checkOpenGLError("ERROR: Could not create shaders.");
//}
//
//
//void Shader::destroyShaderProgram()
//{
//	glUseProgram(0);
//	glDetachShader(ProgramId[0], VertexShaderId);
//	glDetachShader(ProgramId[0], FragmentShaderId);
//
//	glDeleteShader(FragmentShaderId);
//	glDeleteShader(VertexShaderId);
//	glDeleteProgram(ProgramId[0]);
//
//	myErrorChecker.checkOpenGLError("ERROR: Could not destroy shaders.");
//}
//
//GLuint Shader::getProgramId(int n){
//	return ProgramId[n];
//}
//
//GLuint Shader::getUniformId(int n){
//	return UniformId[n];
//}
//
//GLuint* Shader::getUniformIds(){
//	return UniformId;
//}
