#ifndef SHADER_H
#define SHADER_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include <string>
#include "Utils.h"

class ShaderProgram
{
public:
	ShaderProgram(void);
	~ShaderProgram(void);

	//Function: Compile and add Shader from GLSL file to a ShaderProgram
	bool addShaderFromFile(std::string filePath, GLenum shaderType);

	//Function: Compile and add Shader from GLchar sequence to a ShaderProgram
	bool addShaderFromString(const GLchar* shader, GLenum shaderType);

	//Function: check compiler errors from shader compilation. true if ok, false if nok.
	bool checkShaderCompilation(GLuint id);
	
	//Function: check compiler errors from program linkage. true if ok, false if nok.
	bool checkProgramLinkage();

	//Function: links all shaders attached to ShaderProgram
	bool linkShaderProgram();

	void useShaderProgram();

	void bindAttribLocation( GLuint location, const char * name);
	
	//void bindFragDataLocation( GLuint location, const char * name );
	
	void setUniform(const char *name,float x,float y, float z);
	
	void setUniform(const char *name, const glm::vec3 & v);
	
	void setUniform(const char *name, const glm::vec4 & v);
	
	void setUniform(const char *name, const glm::mat4 & m);
	
	void setUniform(const char *name, const glm::mat3 & m);
	
	void setUniform(const char *name, float val );
	
	void setUniform(const char *name, int val );
	
	void setUniform(const char *name, bool val );
	
	void printActiveUniforms(); 
	
	void printActiveAttribs(); 

	int getProgramId();
	
	void setProgramId();

	bool isLinked();

private:
	int programId;
	bool linked;
};

#endif

