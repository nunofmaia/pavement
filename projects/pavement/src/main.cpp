///////////////////////////////////////////////////////////////////////
//
// Assignment 4
//
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>

#include "Engine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Grid.h"
#include "Camera.h"
#include "SceneGraph.h"

#include "Sidebar.h"

#define CAPTION "Tangram"

#define VERTEX_SHADER_FILE "../src/shaders/VertexShader.glsl"
#define FRAGMENT_SHADER_FILE "../src/shaders/FragmentShader.glsl"
#define MESH_PATH "../src/meshes/"
#define TEXTURE_PATH "../src/meshes/basalt2rough.png"

//int WinX = 640, WinY = 640;
int WinX = 900, WinY = 640;
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
ShaderProgram *SidebarShader;

SceneGraph *Scene = new SceneGraph();

Camera *myCamera = new Camera(glm::vec3(0.0 , 5.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
Grid grid(20);
Sidebar sb;


int ID = 1;
int lastMx = 0, lastMy = 0;
SceneNode* SelectedNode;

std::map<int, std::string> Shapes;

enum SymmetryMode {
	NONE,
	XAXIS,
	ZAXIS,
	XZAXIS,
	O
};

int SymMode = SymmetryMode::NONE;


void initializeShapes() {
	Shapes[0] = "cube";
	Shapes[1] = "halfCube";
	Shapes[2] = "prism";
	Shapes[3] = "halfPrism";
	Shapes[4] = "quarterCube";
}

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

//this should be renamed to addNode
SceneNode* createMesh(int shape) {

	// Original solid
	Mesh* m = new Mesh();
	m->loadMeshFile(MESH_PATH + Shapes[shape] + ".obj", TEXTURE_PATH);
	m->createBufferObjects();
	SceneNode* n = new SceneNode(ID++, shape, m, Shader);
	n->_position = glm::vec3(0.125, 0.125, 0.125);

	// X reflection solid
	SceneNode* nX = new SceneNode(n, ReflectionX);
	nX->_mesh->reverseElements();
	nX->_mesh->createBufferObjects();

	// Z reflection solid
	SceneNode* nZ = new SceneNode(n, ReflectionZ);
	nZ->_mesh->reverseElements();
	nZ->_mesh->createBufferObjects();

	// Origin reflection solid
	SceneNode* nO = new SceneNode(n, ReflectionO);
	nO->_mesh->createBufferObjects();

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

	return n;
}


void createShaderProgram()
{
	Shader = new ShaderProgram();

	Shader->setProgramId();
	Shader->addShaderFromFile(VERTEX_SHADER_FILE, GL_VERTEX_SHADER);
	Shader->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	Shader->bindAttribLocation(VERTICES, "in_Position");
	Shader->bindAttribLocation(TEXTURES, "in_Texcoord");
	Shader->bindAttribLocation(NORMALS, "in_Normal");
	Shader->linkShaderProgram();

	UboId = glGetUniformBlockIndex(Shader->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(Shader->getProgramId(), UboId, UBO_BP);

	ReflectionX = new ShaderProgram();

	ReflectionX->setProgramId();
	ReflectionX->addShaderFromFile("../src/shaders/ReflectionXShader.glsl", GL_VERTEX_SHADER);
	ReflectionX->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	ReflectionX->bindAttribLocation(VERTICES, "in_Position");
	ReflectionX->bindAttribLocation(COLORS, "in_Color");
	ReflectionX->bindAttribLocation(TEXTURES, "in_Texcoord");
	ReflectionX->bindAttribLocation(NORMALS, "in_Normal");
	ReflectionX->linkShaderProgram();

	UboId = glGetUniformBlockIndex(ReflectionX->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(ReflectionX->getProgramId(), UboId, UBO_BP);

	ReflectionZ = new ShaderProgram();

	ReflectionZ->setProgramId();
	ReflectionZ->addShaderFromFile("../src/shaders/ReflectionZShader.glsl", GL_VERTEX_SHADER);
	ReflectionZ->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	ReflectionZ->bindAttribLocation(VERTICES, "in_Position");
	ReflectionZ->bindAttribLocation(COLORS, "in_Color");
	ReflectionZ->bindAttribLocation(TEXTURES, "in_Texcoord");
	ReflectionZ->bindAttribLocation(NORMALS, "in_Normal");
	ReflectionZ->linkShaderProgram();

	UboId = glGetUniformBlockIndex(ReflectionZ->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(ReflectionZ->getProgramId(), UboId, UBO_BP);

	ReflectionO = new ShaderProgram();

	ReflectionO->setProgramId();
	ReflectionO->addShaderFromFile("../src/shaders/ReflectionOShader.glsl", GL_VERTEX_SHADER);
	ReflectionO->addShaderFromFile(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);
	ReflectionO->bindAttribLocation(VERTICES, "in_Position");
	ReflectionO->bindAttribLocation(COLORS, "in_Color");
	ReflectionO->bindAttribLocation(TEXTURES, "in_Texcoord");
	ReflectionO->bindAttribLocation(NORMALS, "in_Normal");
	ReflectionO->linkShaderProgram();

	UboId = glGetUniformBlockIndex(ReflectionO->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(ReflectionO->getProgramId(), UboId, UBO_BP);

	GridShader = new ShaderProgram();

	GridShader->setProgramId();
	GridShader->addShaderFromFile("../src/shaders/GridVertexShader.glsl", GL_VERTEX_SHADER);
	GridShader->addShaderFromFile("../src/shaders/GridFragmentShader.glsl", GL_FRAGMENT_SHADER);
	GridShader->bindAttribLocation(VERTICES, "in_Position");
	GridShader->bindAttribLocation(COLORS, "in_Color");
	GridShader->bindAttribLocation(NORMALS, "in_Normal");
	GridShader->linkShaderProgram();

	UboId = glGetUniformBlockIndex(GridShader->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(GridShader->getProgramId(), UboId, UBO_BP);

	SidebarShader = new ShaderProgram();

	SidebarShader->setProgramId();
	SidebarShader->addShaderFromFile("../src/shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	SidebarShader->addShaderFromFile("../src/shaders/SidebarFragmentShader.glsl", GL_FRAGMENT_SHADER);
	SidebarShader->bindAttribLocation(VERTICES, "in_Position");
	SidebarShader->bindAttribLocation(COLORS, "in_Color");
	SidebarShader->bindAttribLocation(NORMALS, "in_Normal");
	ReflectionO->bindAttribLocation(TEXTURES, "in_Texcoord");
	SidebarShader->linkShaderProgram();

	UboId = glGetUniformBlockIndex(SidebarShader->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(SidebarShader->getProgramId(), UboId, UBO_BP);
	
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

SceneNode *white;
SceneNode *black;

void createSidebar() {
	int id = 240;

	Mesh *sq = new Mesh();
	sq->loadMeshFile("../src/meshes/sidebar/cube.obj", TEXTURE_PATH);
	sq->createBufferObjects();
	SceneNode *sqn = new SceneNode(id++, 0, sq, SidebarShader);
	sqn->_position = glm::vec3(-0.15f, 0.6f, 0.0f);
	sqn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	sqn->_angle = 45.0f;
	sqn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *pr = new Mesh();
	pr->loadMeshFile("../src/meshes/sidebar/prism.obj", "");
	pr->createBufferObjects();
	SceneNode *prn = new SceneNode(id++, 2, pr, SidebarShader);
	prn->_position = glm::vec3(0.15f, 0.6f, 0.0f);
	prn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	prn->_angle = 45.0f;
	prn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *hs = new Mesh();
	hs->loadMeshFile("../src/meshes/sidebar/halfCube.obj", "");
	hs->createBufferObjects();
	SceneNode *hsn = new SceneNode(id++, 1, hs, SidebarShader);
	hsn->_position = glm::vec3(-0.15f, 0.3f, 0.0f);
	hsn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	hsn->_angle = 45.0f;
	hsn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *qs = new Mesh();
	qs->loadMeshFile("../src/meshes/sidebar/quarterCube.obj", "");
	qs->createBufferObjects();
	SceneNode *qsn = new SceneNode(id++, 4, qs, SidebarShader);
	qsn->_position = glm::vec3(-0.15f, 0.0f, 0.0f);
	qsn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	qsn->_angle = 45.0f;
	qsn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *lpr = new Mesh();
	lpr->loadMeshFile("../src/meshes/sidebar/halfPrism.obj", "");
	lpr->createBufferObjects();
	SceneNode *lprn = new SceneNode(id++, 3, lpr, SidebarShader);
	lprn->_position = glm::vec3(0.15f, 0.3f, 0.0f);
	lprn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	lprn->_angle = 45.0f;
	lprn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *cw = new Mesh();
	cw->loadMeshFile("../src/meshes/sidebar/cube.obj", "");
	cw->createBufferObjects();
	white = new SceneNode(id++, 0, sq, SidebarShader);
	white->_position = glm::vec3(-0.10f, -0.3f, 0.0f);
	white->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	white->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *cb = new Mesh();
	cb->loadMeshFile("../src/meshes/sidebar/cube.obj", "");
	cb->createBufferObjects();
	black = new SceneNode(id++, 0, sq, SidebarShader);
	black->_position = glm::vec3(0.10f, -0.3f, 0.0f);
	black->_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	black->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	sb.addNode(sqn);
	sb.addNode(prn);
	sb.addNode(hsn);
	sb.addNode(qsn);
	sb.addNode(lprn);
}

void createBufferObjects() {
	glGenBuffers(1, VboId);

	grid.createBufferObjects();
	sb.createBufferObjects();

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

void drawScene()
{
	glBindBuffer(GL_UNIFORM_BUFFER, VboId[0]);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix), glm::value_ptr(glm::lookAt(glm::vec3(LAX, 5.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
	myCamera->lookAt();
	myCamera->viewMode();
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix), sizeof(Matrix), ProjectionMatrix2);

	glViewport(0, 0, 640, 640);

	Scene->draw();

	GridShader->useShaderProgram();

	GridShader->setUniform("ModelMatrix", glm::mat4(1.0f));
	grid.drawGrid();
	
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::lookAt(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::lookAt(glm::vec3(0.0 , 1.5, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::perspective(30.0f, 260/640.0f, 2.0f, 20.0f)));
	glViewport(640, 0, 260, 640);
	
	sb.draw();

	white->draw();
	black->draw();

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glUseProgram(0); //TODO: Use ShaderProgram
	checkOpenGLError("ERROR: Could not draw scene.");
}


/////////////////////////////////////////////////////////////////////// CALLBACKS


void saveScene(std::string fileName) {
	std::ofstream myfile;
	myfile.open (fileName);
	std::vector<SceneNode*>::iterator it;

	for (it = Scene->_nodes.begin(); it != Scene->_nodes.end(); ++it) {
		myfile << (*it)->_shape
			<< " " << (*it)->_position.x
			<< " " << (*it)->_position.y
			<< " " << (*it)->_position.z
			<< " " << (*it)->_color.r
			<< " " << (*it)->_color.g
			<< " " << (*it)->_color.b
			<< " " << (*it)->_color.a
			<< " " << (*it)->_angle << "\n";
	}
	myfile.close();
	std::cout << ">>>>> SCENE SAVED <<<<<" << std::endl;
}

void loadScene(std::string fileName) {
	std::cout << "<<<<< LOADING >>>>>" << std::endl;
	std::ifstream myfile(fileName);
	std::string line;
	
	if (myfile.is_open()) {
		ID = 1;
		Scene->deleteAllNodes();
		while (getline(myfile, line)) {
			std::stringstream linestream(line);
			int shape;
			glm::vec3 position;
			glm::vec4 color;
			GLfloat angle;
			linestream >> shape;
			linestream >> position.x;
			linestream >> position.y;
			linestream >> position.z;
			linestream >> color.r;
			linestream >> color.g;
			linestream >> color.b;
			linestream >> color.a;
			linestream >> angle;
			SceneNode* node = createMesh(shape);
			node->setPosition(position);
			node->setColor(color);
			node->setAngle(angle);
		}
		myfile.close();
		std::cout << ">>>>> SCENE LOADED <<<<<" << std::endl;
	} else {
		std::cout << "Unable to open file";
	}
}

void deleteSelected() {
	if (SelectedNode != NULL) {
		Scene->deleteNode(SelectedNode);
		SelectedNode = NULL;
	}
}

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
	std::string fileName;

	switch (key) {
	case 'q':
		glutDestroyWindow(WindowHandle);
		break;
	case 'c':
		createMesh(0);
		break;
	case 'v':
		createMesh(1);
		break;
	case 'b':
		createMesh(2);
		break;
	case 'n':
		createMesh(3);
		break;
	case 'm':
		createMesh(4);
		break;
	case 'd':
		Scene->deleteAllNodes();
		break;
	//backspace key has an ascii number
	case 8:
		deleteSelected();
		break;
	case 's':
		std::cout << "Save scene - Please enter the file name: ";
		std::cin >> fileName;
		saveScene(fileName);
		break;
	case 'l':
		std::cout << "Load scene - Please enter the file name: ";
		std::cin >> fileName;
		loadScene(fileName);
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
	case '0':
		SymMode = SymmetryMode::NONE;
		Scene->hideSolids(ReflectionX);
		Scene->hideSolids(ReflectionZ);
		Scene->hideSolids(ReflectionO);
		grid.highlightGrid(0);
		break;
	case '1':
		SymMode = SymmetryMode::XAXIS;
		Scene->hideSolids(ReflectionZ);
		Scene->hideSolids(ReflectionO);
		Scene->showSolids(ReflectionX);
		grid.highlightGrid(1);
		break;
	case '2':
		SymMode = SymmetryMode::ZAXIS;
		Scene->hideSolids(ReflectionX);
		Scene->hideSolids(ReflectionO);
		Scene->showSolids(ReflectionZ);
		grid.highlightGrid(2);
		break;
	case '3':
		SymMode = SymmetryMode::O;
		Scene->hideSolids(ReflectionX);
		Scene->hideSolids(ReflectionZ);
		Scene->showSolids(ReflectionO);
		grid.highlightGrid(3);
		break;

	case '4':
		SymMode = SymmetryMode::XZAXIS;
		Scene->showSolids(ReflectionX);
		Scene->showSolids(ReflectionZ);
		Scene->showSolids(ReflectionO);
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

glm::vec4 Color = glm::vec4(0.9, 0.9, 0.9, 1.0);

void changeSidebarMeshColor() {
	std::vector<SceneNode*> nodes = sb.getScene()._nodes;
	std::vector<SceneNode*>::iterator it;
	for (it = nodes.begin(); it != nodes.end(); it++) {
		(*it)->setColor(Color);
	}
}


void meshSelector(GLfloat data) {
	GLuint id = GLuint(data);

	switch (id) {
	case 240:
		std::cout << "Cube" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.25;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			canDrag=true;
		}
		createMesh(0);
		SelectedNode = Scene->findNode(ID - 1);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.25;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
		}
		break;
	case 241:
		std::cout << "Prism" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.25;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			canDrag=true;
		}
		createMesh(2);
		SelectedNode = Scene->findNode(ID - 1);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.25;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
		}
		break;
	case 242:
		std::cout << "Half cube" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.25;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			canDrag=true;
		}
		createMesh(1);
		SelectedNode = Scene->findNode(ID - 1);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.25;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
		}
		break;
	case 243:
		std::cout << "Quarter cube" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.25;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			canDrag=true;
		}
		createMesh(4);
		SelectedNode = Scene->findNode(ID - 1);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.25;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
		}
		break;
	case 244:
		std::cout << "Half prism" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.25;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			canDrag=true;
		}
		createMesh(3);
		SelectedNode = Scene->findNode(ID - 1);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.25;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
		}
		break;
	case 245:
		std::cout << "White" << std::endl;

		if (SelectedNode != NULL) {
			SelectedNode->setColor(glm::vec4(0.9, 0.9, 0.9, 1.0));
		} else {
			Color = glm::vec4(0.9, 0.9, 0.9, 1.0);
			changeSidebarMeshColor();
		}

		break;
	case 246:
		std::cout << "Black" << std::endl;

		if (SelectedNode != NULL) {
			SelectedNode->setColor(glm::vec4(0.1, 0.1, 0.1, 1.0));
		} else {
			Color = glm::vec4(0.1, 0.1, 0.1, 1.0);
			changeSidebarMeshColor();
		}
		break;
	}

}


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
					if (data == SelectedNode->_id) {
						glm::vec3 currentPosition = SelectedNode->_position;
						currentPosition.y -= 0.25;
						SelectedNode->setPosition(currentPosition);

						SelectedNode = NULL;
						canDrag=true;
					}
				} else {
					SelectedNode = Scene->findNode(GLint(data));
					if (SelectedNode != NULL) {
						glm::vec3 currentPosition = SelectedNode->_position;
						currentPosition.y += 0.25;
						SelectedNode->setPosition(currentPosition);
					}
				}


				meshSelector(data);

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

void wheel(int button, int dir, int x, int y) {
	myCamera->zoom(dir);
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
	glutMouseWheelFunc(wheel);
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
	initializeShapes();
	createShaderProgram();
	createBufferObjects();
	setupCallbacks();

	createSidebar();
}

int main(int argc, char* argv[])
{
	init(argc, argv);
	glutMainLoop();	
	exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////


