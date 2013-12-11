///////////////////////////////////////////////////////////////////////
//
// Assignment 4
//
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "Engine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Grid.h"
#include "Camera.h"
#include "SceneGraph.h"

#define CAPTION "Tangram"

#define VERTEX_SHADER_FILE "../src/shaders/VertexShader.glsl"
#define FRAGMENT_SHADER_FILE "../src/shaders/FragmentShader.glsl"

int WinX = 640, WinY = 640;
int WindowHandle = 0;
unsigned int FrameCount = 0;
bool canDrag=false;

GLuint VaoId, VboId[4];
GLint UboId, UniformId;
const GLuint UBO_BP = 0;

ShaderProgram *Shader; 
ShaderProgram *ReflectionX;
ShaderProgram *ReflectionZ;
ShaderProgram *ReflectionO;
ShaderProgram *GridShader;

SceneGraph *Scene = new SceneGraph();

Camera *myCamera = new Camera(glm::vec3(0.0 , 5.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
Grid grid(20);

int ID = 1;
int lastMx = 0, lastMy = 0;
SceneNode* SelectedNode;

std::map<ShaderProgram*, std::vector<Mesh*>*> MeshManager;
enum SymmetryMode {
	NONE,
	XAXIS,
	ZAXIS,
	XZAXIS,
	O
};

int SymMode = SymmetryMode::NONE;

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

void createMesh(std::string filePath){

	// Original solid
	Mesh* m = new Mesh();
	SceneNode* n = new SceneNode(ID++, m, Shader);
	m->loadMeshFile(filePath);
	m->createBufferObjects();
	n->_position = glm::vec3(0.125, 0.125, 0.125);

	// X reflection solid
	//Mesh* x = new Mesh();
	SceneNode* nX = new SceneNode(n, ReflectionX);
	nX->_mesh->reverseElements();
	nX->_mesh->createBufferObjects();

	// Z reflection solid
	//Mesh* z = new Mesh();
	SceneNode* nZ = new SceneNode(n, ReflectionZ);
	nZ->_mesh->reverseElements();
	nZ->_mesh->createBufferObjects();

	// Origin reflection solid
	//Mesh* o = new Mesh();
	SceneNode* nO = new SceneNode(n, ReflectionO);
	nO->_mesh->createBufferObjects();


	//m->addCopy(x);
	//m->addCopy(z);
	//m->addCopy(o);

	//MeshManager[Shader]->push_back(m);
	//MeshManager[ReflectionX]->push_back(x);
	//MeshManager[ReflectionZ]->push_back(z);
	//MeshManager[ReflectionO]->push_back(o);

	n->addCopy(nX);
	n->addCopy(nZ);
	n->addCopy(nO);
	Scene->addNode(n);

	switch (SymMode) {
	case SymmetryMode::NONE:
		nX->_canDraw = false;
		nZ->_canDraw = false;
		nO->_canDraw = false;
		break;
	case SymmetryMode::XAXIS:
		nZ->_canDraw = false;
		nO->_canDraw = false;
		break;
	case SymmetryMode::ZAXIS:
		nX->_canDraw = false;
		nO->_canDraw = false;
		break;
	case SymmetryMode::XZAXIS:
		break;
	case SymmetryMode::O:
		nX->_canDraw = false;
		nZ->_canDraw = false;
		break;
	}
}

//Mesh* findMesh(int id) {
//	std::vector<Mesh*>::iterator it;
//	for (it = MeshManager[Shader]->begin(); it != MeshManager[Shader]->end(); it++) {
//		if ((*it)->_id == id) {
//			return (*it);
//		}
//	}
//
//	return NULL;
//
//}

void hideSolids(ShaderProgram* p) {

	//std::vector<Mesh*> *v = MeshManager[p];

	//std::vector<Mesh*>::iterator it;
	//for (it = v->begin(); it != v->end(); it++) {
	//	(*it)->_canDraw = false;
	//}
	Scene->hideSolids(p);
}

void showSolids(ShaderProgram* p) {

	//std::vector<Mesh*> *v = MeshManager[p];

	//std::vector<Mesh*>::iterator it;
	//for (it = v->begin(); it != v->end(); it++) {
	//	(*it)->_canDraw = true;
	//}
	
	Scene->showSolids(p);

}

void deleteAllMeshes() {
	//MeshManager[Shader]->clear();
	//MeshManager[ReflectionX]->clear();
	//MeshManager[ReflectionZ]->clear();
	//MeshManager[ReflectionO]->clear();

	//Scene->_nodes.clear();
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

	MeshManager[Shader] = new std::vector<Mesh*>();

	ReflectionX = new ShaderProgram();

	ReflectionX->setProgramId();
	ReflectionX->addShaderFromFile("../src/shaders/ReflectionXShader.glsl", GL_VERTEX_SHADER);
	ReflectionX->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	ReflectionX->bindAttribLocation(VERTICES, "in_Position");
	ReflectionX->bindAttribLocation(COLORS, "in_Color");
	ReflectionX->bindAttribLocation(NORMALS, "in_Normal");
	ReflectionX->linkShaderProgram();

	UboId = glGetUniformBlockIndex(ReflectionX->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(ReflectionX->getProgramId(), UboId, UBO_BP);

	MeshManager[ReflectionX] = new std::vector<Mesh*>();

	ReflectionZ = new ShaderProgram();

	ReflectionZ->setProgramId();
	ReflectionZ->addShaderFromFile("../src/shaders/ReflectionZShader.glsl", GL_VERTEX_SHADER);
	ReflectionZ->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	ReflectionZ->bindAttribLocation(VERTICES, "in_Position");
	ReflectionZ->bindAttribLocation(COLORS, "in_Color");
	ReflectionZ->bindAttribLocation(NORMALS, "in_Normal");
	ReflectionZ->linkShaderProgram();

	UboId = glGetUniformBlockIndex(ReflectionZ->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(ReflectionZ->getProgramId(), UboId, UBO_BP);

	MeshManager[ReflectionZ] = new std::vector<Mesh*>();

	ReflectionO = new ShaderProgram();

	ReflectionO->setProgramId();
	ReflectionO->addShaderFromFile("../src/shaders/ReflectionOShader.glsl", GL_VERTEX_SHADER);
	ReflectionO->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	ReflectionO->bindAttribLocation(VERTICES, "in_Position");
	ReflectionO->bindAttribLocation(COLORS, "in_Color");
	ReflectionO->bindAttribLocation(NORMALS, "in_Normal");
	ReflectionO->linkShaderProgram();

	UboId = glGetUniformBlockIndex(ReflectionO->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(ReflectionO->getProgramId(), UboId, UBO_BP);

	MeshManager[ReflectionO] = new std::vector<Mesh*>();

	GridShader = new ShaderProgram();

	GridShader->setProgramId();
	GridShader->addShaderFromFile("../src/shaders/GridVertexShader.glsl", GL_VERTEX_SHADER);
	GridShader->addShaderFromFile("../src/shaders/GridFragmentShader.glsl", GL_FRAGMENT_SHADER);
	GridShader->bindAttribLocation(VERTICES, "in_Position");
	ReflectionX->bindAttribLocation(COLORS, "in_Color");
	ReflectionX->bindAttribLocation(NORMALS, "in_Normal");
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
	delete(ReflectionX);
	delete(GridShader);

	checkOpenGLError("ERROR: Could not destroy shaders.");
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

typedef GLfloat Matrix[16];

void createBufferObjects(){
	glGenBuffers(1, VboId);

	grid.createBufferObjects();
	glBindBuffer(GL_UNIFORM_BUFFER, VboId[0]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, 0, GL_STREAM_DRAW);
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

void drawScene()
{
	glBindBuffer(GL_UNIFORM_BUFFER, VboId[0]);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix), glm::value_ptr(glm::lookAt(glm::vec3(LAX, 5.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
	myCamera->lookAt();
	myCamera->viewMode();
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix), sizeof(Matrix), ProjectionMatrix2);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	//std::map<ShaderProgram*, std::vector<Mesh*>*>::iterator it;
	//for (it = MeshManager.begin(); it != MeshManager.end(); it++) {
	//	it->first->useShaderProgram();
	//	
	//	std::vector<Mesh*>::iterator ot;
	//	for (ot = it->second->begin(); ot != it->second->end(); ot++) {
	//		it->first->setUniform("ModelMatrix", glm::translate(glm::mat4(1.0), (*ot)->_position));
	//		it->first->setUniform("DefaultColor", (*ot)->_color);
	//		it->first->setUniform("Angle", (*ot)->_angle);
	//		(*ot)->draw();
	//	}
	//}

	Scene->draw();

	GridShader->useShaderProgram();

	GridShader->setUniform("ModelMatrix", glm::mat4(1.0f));
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearStencil(0);
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

	std::vector<Mesh*>::iterator it;

	switch (key) {
	case 'q':
		glutDestroyWindow(WindowHandle);
		break;
	case 'c':
		createMesh("../src/meshes/cube.obj");
		break;
	case 'v':
		createMesh("../src/meshes/halfCube.obj");
		break;
	case 'b':
		createMesh("../src/meshes/prism.obj");
		break;
	case 'n':
		createMesh("../src/meshes/halfPrism.obj");
		break;
	case 'm':
		createMesh("../src/meshes/quarterCube.obj");
		break;
	case 'l':
		createMesh("../src/meshes/cubeTest.obj");
		break;
	case 'd':
		Scene->deleteAllNodes();
		break;
	case 'p':
		if(SelectedNode != NULL){
			SelectedNode->setColor(glm::vec4(0.164, 0.168, 0.22, 1.0));
		}
		break;
	case 'o':
		if(SelectedNode != NULL){
			SelectedNode->setColor(glm::vec4(1.0, 0.98, 0.92, 1.0));
		}
		break;
	case 'r':
		if(SelectedNode != NULL){
			GLfloat newAngle = SelectedNode->_angle + 90.0f;
			SelectedNode->setAngle(newAngle);
		}
		break;
	/*case 'l':
		std::cout<<"restart"<<std::endl;
		myCamera->restartCamera();
		break;*/
	case '0':
		SymMode = SymmetryMode::NONE;
		hideSolids(ReflectionX);
		hideSolids(ReflectionZ);
		hideSolids(ReflectionO);
		grid.highlightGrid(0);
		break;
	case '1':
		SymMode = SymmetryMode::XAXIS;
		hideSolids(ReflectionZ);
		hideSolids(ReflectionO);
		showSolids(ReflectionX);
		grid.highlightGrid(1);
		break;
	case '2':
		SymMode = SymmetryMode::ZAXIS;
		hideSolids(ReflectionX);
		hideSolids(ReflectionO);
		showSolids(ReflectionZ);
		grid.highlightGrid(2);
		break;
	case '3':
		SymMode = SymmetryMode::O;
		hideSolids(ReflectionX);
		hideSolids(ReflectionZ);
		showSolids(ReflectionO);
		grid.highlightGrid(3);
		break;

	case '4':
		SymMode = SymmetryMode::XZAXIS;
		showSolids(ReflectionX);
		showSolids(ReflectionZ);
		showSolids(ReflectionO);
		grid.highlightGrid(4);
		break;
	}
}

void keyboardSpecial(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		if (SelectedNode != NULL) {
			glm::vec3 newPosition = SelectedNode->_position;
			newPosition.x -= 0.125;
			SelectedNode->setPosition(newPosition);
		}

		break;
	case GLUT_KEY_RIGHT:
		if (SelectedNode != NULL) {
			glm::vec3 newPosition = SelectedNode->_position;
			newPosition.x += 0.125;
			SelectedNode->setPosition(newPosition);
		}
		break;
	case GLUT_KEY_UP:
		if (SelectedNode != NULL) {
			glm::vec3 newPosition = SelectedNode->_position;
			newPosition.z -= 0.125;
			SelectedNode->setPosition(newPosition);
		}

		break;
	case GLUT_KEY_DOWN:
		if (SelectedNode != NULL) {
			glm::vec3 newPosition = SelectedNode->_position;
			newPosition.z += 0.125;
			SelectedNode->setPosition(newPosition);
		}
		break;
	}
}

int MouseX = 0;
int MouseY = 0;

void mouse(GLint button, GLint state, GLint x, GLint y) {

	switch (button) {
	case GLUT_LEFT_BUTTON:

		if (state == GLUT_DOWN) {
			MouseX = x;
			MouseY = y;
			lastMx=x;
			lastMy=y;
			GLfloat data;
			glReadPixels(MouseX, WinY - MouseY - 1, 1, 1, GL_STENCIL_INDEX, GL_FLOAT, &data);

			std::cout << "Stencil data: " << data << std::endl;
			if (data == 0) {
				if (SelectedNode != NULL) {
					glm::vec3 currentPosition = SelectedNode->_position;
					currentPosition.y -= 0.25;
					SelectedNode->setPosition(currentPosition);
				}

				SelectedNode = NULL;
				canDrag=true;
			} else {
				if (SelectedNode != NULL) {
					glm::vec3 currentPosition = SelectedNode->_position;
					currentPosition.y -= 0.25;
					SelectedNode->setPosition(currentPosition);
				}

				SelectedNode = Scene->findNode(GLint(data));
				glm::vec3 currentPosition = SelectedNode->_position;
				currentPosition.y += 0.25;
				SelectedNode->setPosition(currentPosition);
			}
		}
		if(state==GLUT_UP){
			myCamera->rotationAngleX=0.0f;
			myCamera->rotationAngleY=0.0f;
			//myCamera->setUpdateVMatrixFlag(false);
			canDrag=false;
		}
		break;
	}
}

void mouseMotion(int x, int y){

	if (canDrag){
		myCamera->rotationAngleY = (float)(x - lastMx);
		myCamera->rotationAngleX = (float)(y - lastMy);
		myCamera->setUpdateVMatrixFlag(true);
		lastMx = x;
		lastMy = y; 
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
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
}

void setupOpenGL() {
	std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
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
	//createMeshes();
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
