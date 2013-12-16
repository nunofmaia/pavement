#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <sstream> 
#include "Engine.h"

class Camera
{
public:
	glm::mat4 viewMatrix, mainViewMatrix;
	glm::vec3 eyeVector,centerVector,upVector,yAxis,xAxis;
	GLfloat rotationAngleX,rotationAngleY, k;
	glm::quat mainQ, currentQ;
	int windowX,windowY;
	bool updateVMatrixFlag;
	bool restartViewMatrixFlag;
	int switchMode;
	GLfloat zoomFactor;

	Camera(void);
	Camera(glm::vec3,glm::vec3,glm::vec3);
	~Camera(void);
	
	glm::mat4 getViewMatrix();
	void setViewMatrix(glm::mat4 matrix);
	void setUpdateVMatrixFlag(bool);
	void updateViewMatrix();
	void setViewMatrix();
	void cleanViewProjectionMatrix();
	void lookAt();
	void zoom(int);
	
	void orthographic(GLfloat,GLfloat,GLfloat,GLfloat,GLfloat,GLfloat);
	void perspective(GLfloat,GLfloat,GLfloat,GLfloat);
	void viewMode();
	
	void restartCamera();

	//GLfloat t,b,r,l,n,f;
};

#endif