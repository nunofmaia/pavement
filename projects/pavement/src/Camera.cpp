#include "Camera.h"

Camera::Camera(void) {

}

Camera::Camera(glm::vec3 e, glm::vec3 c, glm::vec3 u, Camera::Mode* projectionMode) {
	_yAxis = glm::vec3(0.0,1.0,0.0);
	_xAxis = glm::vec3(1.0,0.0,0.0f);
	_viewMatrix = glm::lookAt(e, c, u);
	_projectionMode = projectionMode;
}

Camera::~Camera(void) {

}

void Camera::update(GLfloat rotX, GLfloat rotY){
	/**/
	//ONLY HORIZONTAL ROTATION
	glm::quat q1 = glm::angleAxis(rotX, _yAxis);
	glm::quat currentQ = (q1);
	glm::mat4 mf = glm::mat4_cast(currentQ);
		
	/** /
	//DEBUG ROTATION
	glm::quat q1 = glm::angleAxis(rotX, _yAxis);
	glm::mat4 m1 = glm::mat4_cast(q1);
	glm::quat q2 = glm::angleAxis(rotY, _xAxis);
	glm::mat4 m2 = glm::mat4_cast(q2);
	glm::quat currentQ = (q1*q2);
	glm::mat4 mf = glm::mat4_cast(currentQ);
	/**/

	_viewMatrix *= mf;

	_yAxis = _yAxis * currentQ;
	_xAxis = _xAxis * currentQ;
}

void Camera::lookAt(){
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(_viewMatrix));
}

void Camera::project(){
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),glm::value_ptr(_projectionMode->getProjection()));
}

void Camera::zoom(int dir){
	if (dir < 0) {
		_projectionMode->zoomOut();
	} else {
		_projectionMode->zoomIn();
	}
}

Camera::Mode::Mode(GLfloat zoomFactor, GLfloat n, GLfloat f) :
	_zoomFactor(zoomFactor),
	_near(n),
	_far(f) {
}

glm::mat4 Camera::Mode::getProjection() {
	return _projection;
}

void Camera::Mode::zoomIn() {
	_zoomFactor *= 0.98f;
	updateProjection();
}

void Camera::Mode::zoomOut() {
	_zoomFactor *= 1.02f;
	updateProjection();
}


Camera::Perspective::Perspective(GLfloat fovY, GLfloat aspect, GLfloat n, GLfloat f) : 
	Mode(1.0f, n, f),
	_fovY(fovY),
	_aspect(aspect) {
		updateProjection();
}

void Camera::Perspective::updateProjection() {
	_projection = glm::perspective(_fovY * _zoomFactor, _aspect, _near, _far);
}

Camera::Ortho::Ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat n, GLfloat f) :
	Mode(1.0f, n, f),
	_left(left),
	_right(right),
	_bottom(bottom),
	_top(top) {
		updateProjection();
}

void Camera::Ortho::updateProjection() {
	_projection = glm::ortho(_left, _right, _bottom, _top, _near, _far);
}
