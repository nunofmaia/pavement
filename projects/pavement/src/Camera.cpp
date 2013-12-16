#include "Camera.h"


//GLfloat lastAngleY,lastAngleX;

Camera::Camera(void)
{
	rotationAngleX=0.0f;
	rotationAngleY=0.0f;
	zoomFactor = 1.0f;
}

Camera::Camera(glm::vec3 e, glm::vec3 c, glm::vec3 u){
	rotationAngleX=0.0f;
	rotationAngleY=0.0f;
	eyeVector=e;
	centerVector=c;
	upVector=u;
	yAxis = glm::vec3(0.0,1.0,0.0);
	xAxis = glm::vec3(1.0,0.0,0.0f);
	updateVMatrixFlag=true;
	restartViewMatrixFlag=false;
	switchMode = 0;
	viewMatrix=glm::lookAt(eyeVector, centerVector, upVector);
	mainViewMatrix = viewMatrix;
	k=0.0f;
	glm::quat mainQ = glm::angleAxis(rotationAngleY,yAxis)*glm::angleAxis(rotationAngleX,xAxis);
	zoomFactor = 1.0f;
}

Camera::~Camera(void)
{
}

void Camera::setViewMatrix(glm::mat4 matrix){
	viewMatrix = matrix;
}

void Camera::setUpdateVMatrixFlag(bool boolean){
	updateVMatrixFlag=boolean;
}


void Camera::updateViewMatrix(){
	if(updateVMatrixFlag) {
		if (!restartViewMatrixFlag)	{

			/**/						//ONLY HORIZONTAL ROTATION
			//quaternion y Rotation
			glm::quat q1 = glm::angleAxis(rotationAngleY,yAxis);
			currentQ = (q1);
			glm::mat4 mf = glm::mat4_cast(currentQ);
			
			/** /						//DEBUG ROTATION
			glm::quat q1 = glm::angleAxis(rotationAngleY,yAxis);
			glm::mat4 m1 = glm::mat4_cast(q1);
			glm::quat q2 = glm::angleAxis(rotationAngleX,xAxis);
			glm::mat4 m2 = glm::mat4_cast(q2);
			currentQ = (q1*q2);
			glm::mat4 mf = glm::mat4_cast(currentQ);
			/**/

			// combining the 2 quaternion rotations into one single quaternion
			viewMatrix*=mf;

			yAxis = yAxis * currentQ;
			xAxis = xAxis * currentQ;

			updateVMatrixFlag=false; 
		}
		else {
			if (k<1)
			{
				std::cout<<"RESTART"<<std::endl;
				glm::quat viewMQuat = glm::quat_cast(viewMatrix);
				glm::quat newVMQuat = glm::quat_cast(mainViewMatrix);
				glm::quat q = glm::mix(viewMQuat, mainQ, k);

				glm::mat4 m = glm::mat4_cast(q);
				
				/*for(int i = 0 ; i<16; i++){
					std::cout<<" "<<m[i]<<" "<<m[i+1]<<" "<<m[i+2]<<" "<<m[3]<<" "<<std::endl;
				}*/

				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix*m));
				k+=0.01f;
			}
			restartViewMatrixFlag=false;
		}

	}
}

//void Camera::setViewMatrix(){
//	if(updateVMatrixFlag){
//		viewMatrix=glm::lookAt(eyeVector, centerVector, upVector);
//		updateVMatrixFlag=false;
//	}
//}

void Camera::lookAt(){
	//if(updateVMatrixFlag){
	updateViewMatrix();
	//updateVMatrixFlag=false;
	//}

	if (!restartViewMatrixFlag)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));

	}}

void Camera::zoom(int dir){
	if(dir<0){
		//eyeVector = eyeVector*1.01f;
		zoomFactor *= 1.02f;
	}else{
		//eyeVector = eyeVector*0.99f;
		zoomFactor *= 0.98f;
	}
	updateVMatrixFlag=true;
}

//void Camera::orthographic(GLfloat l, GLfloat r,GLfloat b, GLfloat t, GLfloat n, GLfloat f){
//
//	array<GLfloat,16> temp ={
//			2/(r-l), 0.0f, 0.0f, -(l+r)/(r-l),
//			0.0f, 2/(t-b), 0.0f, -(b+t)/(t-b),
//			0.0f, 0.0f, -2/(f-n), -(n+f)/(f-n),
//			0.0f, 0.0f, 0.0f, 1.0f
//		};
//	Matrix orthoM(temp);
//	orthoM.clean();
//	//orthoM.transpose().printMatrix();
//	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(const GLfloat[16]), sizeof(const GLfloat[16]), orthoM.transpose().matrix1D.data());
//}

void Camera::perspective(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f){
	glm::mat4 perspM = glm::perspective(fovy,aspect,n,f);
	//perspM.transpose().printMatrix();
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),glm::value_ptr(perspM));
}

void Camera::viewMode(){
	//if(switchMode == 0){
	perspective(30.0f * zoomFactor,(GLfloat)(windowX/windowY),2.0f,20.0f);			//perspective(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f)
	//}
	//else{
	//	orthographic(-2.0f,2.0f,-2.0f,2.0f,1.0f,10.0f);						//orthogonalGLfloat l, GLfloat r,GLfloat b, GLfloat t, GLfloat n, GLfloat f)
	//	//orthographic(-1.5f,1.5f,-1.5f,1.5f,1.0f,5.0f);	
	//}
}

void Camera::restartCamera(){
	updateVMatrixFlag=true;
	restartViewMatrixFlag=true;
	lookAt();
}