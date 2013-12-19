#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "Utils.h"
#include "Engine.h"

class Camera {
public:
	class Mode {
	protected:
		glm::mat4 _projection;
		GLfloat _zoomFactor, _near, _far;
		Mode(GLfloat, GLfloat, GLfloat);
	public:
		glm::mat4 getProjection();
		virtual void updateProjection() {}
		void zoomIn();
		void zoomOut();
	};
	class Perspective : public Camera::Mode {
		GLfloat _fovY, _aspect;
	public:
		Perspective(GLfloat, GLfloat, GLfloat, GLfloat);
		void updateProjection();
	};
	class Ortho : public Camera::Mode {
		GLfloat _left, _right, _bottom, _top;
	public:
		Ortho(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
		void updateProjection();
	};

private:
	glm::mat4 _viewMatrix;
	glm::vec3 _xAxis, _yAxis;
	Mode *_projectionMode;

public:

	Camera(void);
	Camera(glm::vec3, glm::vec3, glm::vec3, Camera::Mode*);
	~Camera(void);
	
	void update(GLfloat, GLfloat);
	void cleanViewProjectionMatrix();
	void lookAt();
	void zoom(int);
	
	void orthographic(GLfloat, GLfloat, GLfloat,GLfloat,GLfloat,GLfloat);
	void perspective(GLfloat, GLfloat, GLfloat,GLfloat);
	void project();

};


#endif