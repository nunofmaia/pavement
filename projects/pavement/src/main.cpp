///////////////////////////////////////////////////////////////////////
//
// Assignment 4
//
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>

#include "Engine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Grid.h"

#define CAPTION "Tangram"

#define VERTEX_SHADER_FILE "../src/shaders/VertexShader.glsl"
#define FRAGMENT_SHADER_FILE "../src/shaders/FragmentShader.glsl"

int WinX = 640, WinY = 480;
int WindowHandle = 0;
unsigned int FrameCount = 0;

GLuint VaoId, VboId[4];
GLint UboId, UniformId;
const GLuint UBO_BP = 0;

ShaderProgram *Shader; 
ShaderProgram *Reflection;
ShaderProgram *GridShader;
Mesh mesh, copy;
Model test;
Grid grid(20);
/////////////////////////////////////////////////////////////////////// ERRORS

bool isOpenGLError() {
	bool isError = false;
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		isError = true;
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
	}
	return isError;
}

void checkOpenGLError(std::string error)
{
	if(isOpenGLError()) {
		std::cerr << error << std::endl;
		exit(EXIT_FAILURE);
	}
}

/////////////////////////////////////////////////////////////////////// SHADERs

void createMeshes(){
	/**/
	mesh.loadMeshFile("../src/meshes/knot.obj");
	/** /
	mesh.loadMeshFile("../src/meshes/cube.obj");
	/** /
	mesh.loadMeshFile("../src/meshes/cube_small.obj");
	/**/
	copy = mesh;
	copy.reverseElements();
}

void createShaderProgram()
{
	Shader = new ShaderProgram();

	Shader->setProgramId();
	Shader->addShaderFromFile(VERTEX_SHADER_FILE, GL_VERTEX_SHADER);
	Shader->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	Shader->bindAttribLocation(VERTICES, "in_Position");
	Shader->bindAttribLocation(COLORS, "in_Color");
	Shader->bindAttribLocation(NORMALS, "in_Normal");
	Shader->linkShaderProgram();

	UboId = glGetUniformBlockIndex(Shader->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(Shader->getProgramId(), UboId, UBO_BP);

	Reflection = new ShaderProgram();

	Reflection->setProgramId();
	Reflection->addShaderFromFile("../src/shaders/ReflectionShader.glsl", GL_VERTEX_SHADER);
	Reflection->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	Reflection->bindAttribLocation(VERTICES, "in_Position");
	Reflection->bindAttribLocation(COLORS, "in_Color");
	Reflection->bindAttribLocation(NORMALS, "in_Normal");
	Reflection->linkShaderProgram();

	UboId = glGetUniformBlockIndex(Reflection->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(Reflection->getProgramId(), UboId, UBO_BP);

	GridShader = new ShaderProgram();

	GridShader->setProgramId();
	GridShader->addShaderFromFile("../src/shaders/GridVertexShader.glsl", GL_VERTEX_SHADER);
	GridShader->addShaderFromFile("../src/shaders/GridFragmentShader.glsl", GL_FRAGMENT_SHADER);
	GridShader->bindAttribLocation(VERTICES, "in_Position");
	Reflection->bindAttribLocation(COLORS, "in_Color");
	Reflection->bindAttribLocation(NORMALS, "in_Normal");
	GridShader->linkShaderProgram();

	UboId = glGetUniformBlockIndex(GridShader->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(GridShader->getProgramId(), UboId, UBO_BP);
	
	checkOpenGLError("ERROR: Could not create shaders.");
}

void destroyShaderProgram()
{
	glUseProgram(0);

	//TODO: Detach shaders

	delete(Shader);
	delete(Reflection);
	delete(GridShader);

	checkOpenGLError("ERROR: Could not destroy shaders.");
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

typedef GLfloat Matrix[16];

void createBufferObjects(){
	glGenBuffers(1, VboId);

	mesh.createBufferObjects();
	copy.createBufferObjects();
	grid.createBufferObjects();
	glBindBuffer(GL_UNIFORM_BUFFER, VboId[0]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix)*2, 0, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, UBO_BP, VboId[0]);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void destroyBufferObjects()
{
	glDisableVertexAttribArray(VERTICES);
	glDisableVertexAttribArray(COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(4, VboId);
	glDeleteVertexArrays(1, &VaoId);
	checkOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
}

/////////////////////////////////////////////////////////////////////// SCENE

const Matrix I = {
	1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,
	0.0f,  0.0f,  0.0f,  1.0f
};

const Matrix ModelMatrix = {
	1.0f,  0.0f,  0.0f,  0.0f,
    0.0f,  1.0f,  0.0f,  0.0f,
	0.0f,  0.0f,  1.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,  1.0f
}; // Column Major

// Eye(5,5,5) Center(0,0,0) Up(0,1,0)
const Matrix ViewMatrix1 = {
    0.70f, -0.41f,  0.58f,  0.00f,
	0.00f,  0.82f,  0.58f,  0.00f,
   -0.70f, -0.41f,  0.58f,  0.00f,
	0.00f,  0.00f, -8.70f,  1.00f
}; // Column Major

// Eye(-5,-5,-5) Center(0,0,0) Up(0,1,0)
const Matrix ViewMatrix2 = {
   -0.70f, -0.41f, -0.58f,  0.00f,
	0.00f,  0.82f, -0.58f,  0.00f,
    0.70f, -0.41f, -0.58f,  0.00f,
	0.00f,  0.00f, -8.70f,  1.00f
}; // Column Major

// Orthographic LeftRight(-2,2) TopBottom(-2,2) NearFar(1,10)
const Matrix ProjectionMatrix1 = {
	0.50f,  0.00f,  0.00f,  0.00f,
	0.00f,  0.50f,  0.00f,  0.00f,
	0.00f,  0.00f, -0.22f,  0.00f,
	0.00f,  0.00f, -1.22f,  1.00f
}; // Column Major

// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(10)
const Matrix ProjectionMatrix2 = {
	2.79f,  0.00f,  0.00f,  0.00f,
	0.00f,  3.73f,  0.00f,  0.00f,
	0.00f,  0.00f, -1.22f, -1.00f,
	0.00f,  0.00f, -2.22f,  0.00f
}; // Column Major

GLfloat LAX = 0.0;

void drawScene()
{
	glBindBuffer(GL_UNIFORM_BUFFER, VboId[0]);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix), ViewMatrix1);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix), glm::value_ptr(glm::lookAt(glm::vec3(LAX, 7.0, 7.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix), sizeof(Matrix), ProjectionMatrix2);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	Shader->useShaderProgram();



	Shader->setUniform("ModelMatrix", glm::mat4(1.0f));
	mesh.drawMesh();

	Reflection->useShaderProgram();
	Reflection->setUniform("ModelMatrix", glm::mat4(1.0f));
	copy.drawMesh();

	GridShader->useShaderProgram();

	Shader->setUniform("ModelMatrix", glm::mat4(1.0f));
	grid.drawGrid();

	glUseProgram(0); //TODO: Use ShaderProgram
	checkOpenGLError("ERROR: Could not draw scene.");
}

/////////////////////////////////////////////////////////////////////// CALLBACKS

void cleanup()
{
	destroyShaderProgram();
	destroyBufferObjects();
}

void display()
{
	++FrameCount;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
	glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	WinX = w;
	WinY = h;
	glViewport(0, 0, WinX, WinY);
}

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
    FrameCount = 0;
    glutTimerFunc(1000, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'q':
		glutDestroyWindow(WindowHandle);
		break;
	}
}

void keyboardSpecial(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		LAX -= 1.0;
		break;
	case GLUT_KEY_RIGHT:
		LAX += 1.0;
		break;
	}
}

/////////////////////////////////////////////////////////////////////// SETUP

void setupCallbacks() 
{
	glutCloseFunc(cleanup);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutTimerFunc(0,timer,0);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecial);
}

void setupOpenGL() {
	std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDepthRange(0.0, 1.0);
	glClearDepth(1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void setupGLEW() {
	glewExperimental = GL_TRUE;
	GLenum result = glewInit() ; 
	if (result != GLEW_OK) { 
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	} 
	GLenum err_code = glGetError();
}

void setupGLUT(int argc, char* argv[])
{
	glutInit(&argc, argv);
	
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutInitWindowSize(WinX, WinY);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	WindowHandle = glutCreateWindow(CAPTION);
	if(WindowHandle < 1) {
		std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void init(int argc, char* argv[])
{
	setupGLUT(argc, argv);
	setupGLEW();
	setupOpenGL();
	createMeshes();
	createShaderProgram();
	createBufferObjects();
	setupCallbacks();
}

int main(int argc, char* argv[])
{
	init(argc, argv);
	glutMainLoop();	
	exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////
