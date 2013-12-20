#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include <stack>

#include "Engine.h"
#include "Shader.h"
#include "Grid.h"
#include "Sidebar.h"

#define CAPTION "Pedras e Calcada"

#define VERTEX_SHADER_FILE "../src/shaders/VertexShader.glsl"
#define FRAGMENT_SHADER_FILE "../src/shaders/FragmentShader.glsl"
#define MESH_PATH "../src/meshes/"
#define TEXTURE_PATH "../src/meshes/basalt2.png"
#define NOISE_TEXTURE_PATH "../src/meshes/PerlinNoise.jpg"

int WinX = 900, WinY = 640;
int WindowHandle = 0;
unsigned int FrameCount = 0;
bool canDrag = true;

GLuint VaoId, VboId[4], TextureId[2];
GLint UboId, UniformId;
const GLuint UBO_BP = 0;

ShaderProgram *Shader; 
ShaderProgram *ReflectionX;
ShaderProgram *ReflectionZ;
ShaderProgram *ReflectionO;
ShaderProgram *SimpleShader;

MeshManager *Manager = new MeshManager();

std::stack<int> AvailableIds;

Grid *DrawingZone = new Grid(40);
Sidebar *EditZone = new Sidebar();


int ID = 1;
int CurrentID = 0;
int lastMx = 0, lastMy = 0;
SceneNode* SelectedNode;

enum SymmetryMode {
	NONE,
	XAXIS,
	ZAXIS,
	XZAXIS,
	O
};

int SymMode = SymmetryMode::NONE;


void initializeMeshes() {
	Manager->addMesh(0, new Mesh("../src/meshes/cube.obj"));
	Manager->addMesh(1, new Mesh("../src/meshes/halfCube.obj"));
	Manager->addMesh(2, new Mesh("../src/meshes/prism.obj"));
	Manager->addMesh(3, new Mesh("../src/meshes/halfPrism.obj"));
	Manager->addMesh(4, new Mesh("../src/meshes/quarterCube.obj"));
	Manager->addMesh(5, new Mesh("../src/meshes/smooth_sphere.obj"));
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

void checkOpenGLError(std::string error) {
	if (isOpenGLError()) {
		std::cerr << error << std::endl;
		exit(EXIT_FAILURE);
	}
}

/////////////////////////////////////////////////////////////////////// SHADERs

SceneNode* addNode(int shape) {
	// Original solid
	Mesh* m = Manager->getMesh(shape);
	int id;

	if (AvailableIds.empty()) {
		id = ID++;
	} else {
		id = AvailableIds.top();
		AvailableIds.pop();
	}

	CurrentID = id;

	SceneNode* n = new SceneNode(id, shape, m, Shader, TextureId);
	n->createBufferObjects();
	n->_position = glm::vec3(0.125, 0.125, 0.125);

	// X reflection solid
	SceneNode* nX = new SceneNode(n, ReflectionX);
	nX->_toRevert = true;
	nX->createBufferObjects();

	// Z reflection solid
	SceneNode* nZ = new SceneNode(n, ReflectionZ);
	nZ->_toRevert = true;
	nZ->createBufferObjects();

	// Origin reflection solid
	SceneNode* nO = new SceneNode(n, ReflectionO);
	nO->createBufferObjects();

	n->addCopy(nX);
	n->addCopy(nZ);
	n->addCopy(nO);
	DrawingZone->addNode(n);

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


void createShaderProgram() {
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

	SimpleShader = new ShaderProgram();

	SimpleShader->setProgramId();
	SimpleShader->addShaderFromFile("../src/shaders/SimpleVertexShader.glsl", GL_VERTEX_SHADER);
	SimpleShader->addShaderFromFile("../src/shaders/SimpleFragmentShader.glsl", GL_FRAGMENT_SHADER);
	SimpleShader->bindAttribLocation(VERTICES, "in_Position");
	SimpleShader->bindAttribLocation(COLORS, "in_Color");
	SimpleShader->linkShaderProgram();

	UboId = glGetUniformBlockIndex(SimpleShader->getProgramId(), "SharedMatrices"); //TODO: Use ShaderProgram
	glUniformBlockBinding(SimpleShader->getProgramId(), UboId, UBO_BP);
	
	checkOpenGLError("ERROR: Could not create shaders.");
}

void destroyShaderProgram() {
	glUseProgram(0);

	delete(Shader);
	delete(ReflectionX);
	delete(ReflectionZ);
	delete(ReflectionO);
	delete(SimpleShader);

	checkOpenGLError("ERROR: Could not destroy shaders.");
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

SceneNode *white;
SceneNode *black;

void createSidebar() {
	int id = 240;

	Mesh *sq = new Mesh("../src/meshes/sidebar/cube.obj");
	SceneNode *sqn = new SceneNode(id++, 0, sq, Shader, TextureId);
	sqn->createBufferObjects();
	sqn->_position = glm::vec3(-0.15f, 1.2f, 0.0f);
	sqn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	sqn->_angle = 45.0f;
	sqn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *pr = new Mesh("../src/meshes/sidebar/prism.obj");
	SceneNode *prn = new SceneNode(id++, 2, pr, Shader, TextureId);
	prn->createBufferObjects();
	prn->_position = glm::vec3(0.15f, 1.2f, 0.0f);
	prn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	prn->_angle = 45.0f;
	prn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *hs = new Mesh("../src/meshes/sidebar/halfCube.obj");
	SceneNode *hsn = new SceneNode(id++, 1, hs, Shader, TextureId);
	hsn->createBufferObjects();
	hsn->_position = glm::vec3(-0.15f, 0.6f, 0.0f);
	hsn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	hsn->_angle = 45.0f;
	hsn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *qs = new Mesh("../src/meshes/sidebar/quarterCube.obj");
	SceneNode *qsn = new SceneNode(id++, 4, qs, Shader, TextureId);
	qsn->createBufferObjects();
	qsn->_position = glm::vec3(-0.15f, 0.0f, 0.0f);
	qsn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	qsn->_angle = 45.0f;
	qsn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *lpr = new Mesh("../src/meshes/sidebar/halfPrism.obj");
	SceneNode *lprn = new SceneNode(id++, 3, lpr, Shader, TextureId);
	lprn->createBufferObjects();
	lprn->_position = glm::vec3(0.15f, 0.6f, 0.0f);
	lprn->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	lprn->_angle = 45.0f;
	lprn->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *cw = new Mesh("../src/meshes/sidebar/cube.obj");
	white = new SceneNode(id++, 0, sq, Shader, TextureId);
	white->createBufferObjects();
	white->_position = glm::vec3(-0.15f, -0.6f, 0.0f);
	white->_color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	white->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	Mesh *cb = new Mesh("../src/meshes/sidebar/cube.obj");
	black = new SceneNode(id++, 0, sq, Shader, TextureId);
	black->createBufferObjects();
	black->_position = glm::vec3(0.15f, -0.6f, 0.0f);
	black->_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	black->_scale = glm::vec3(0.5f, 0.5f, 0.5f);

	EditZone->addNode(sqn);
	EditZone->addNode(prn);
	EditZone->addNode(hsn);
	EditZone->addNode(qsn);
	EditZone->addNode(lprn);
}

void createTextures() {
	int width1,height1,width2,height2;
	
	unsigned char* img1 = SOIL_load_image(TEXTURE_PATH, &width1, &height1, NULL, SOIL_LOAD_RGB);
	unsigned char* img2 = SOIL_load_image(NOISE_TEXTURE_PATH, &width2, &height2, NULL, SOIL_LOAD_RGB);
	
	glGenTextures(2, TextureId);
	
	glBindTexture(GL_TEXTURE_2D, TextureId[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, TextureId[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void createBufferObjects() {
	glGenBuffers(1, VboId);

	DrawingZone->createBufferObjects();
	EditZone->createBufferObjects();

	glBindBuffer(GL_UNIFORM_BUFFER, VboId[0]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, 0, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, UBO_BP, VboId[0]);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void destroyBufferObjects() {
	glDisableVertexAttribArray(VERTICES);
	glDisableVertexAttribArray(COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(4, VboId);
	glDeleteVertexArrays(1, &VaoId);
	checkOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
}

/////////////////////////////////////////////////////////////////////// SCENE

void drawScene() {

	glBindBuffer(GL_UNIFORM_BUFFER, VboId[0]);


	glViewport(0, 0, 640, 640);
	SimpleShader->useShaderProgram();
	DrawingZone->draw();

	glViewport(640, 0, 260, 640);
	SimpleShader->useShaderProgram();
	EditZone->draw();

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

	for (it = DrawingZone->getScene()._nodes.begin(); it != DrawingZone->getScene()._nodes.end(); ++it) {
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
	std::cout << "This scene has been saved." << std::endl;
}

void loadScene(std::string fileName) {
	std::cout << "Loading scene..." << std::endl;
	std::ifstream myfile(fileName);
	std::string line;
	
	if (myfile.is_open()) {
		//TODO deveriamos fazer algum metodo que reiniciasse tudo?!
		ID = 1;
		SelectedNode = NULL;
		DrawingZone->getScene().deleteAllNodes();
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
			SceneNode* node = addNode(shape);
			node->setPosition(position);
			node->setColor(color);
			node->setAngle(angle);
		}
		myfile.close();
		std::cout << "This scene has been loaded." << std::endl;
	} else {
		std::cout << "Unable to open file";
	}
}

void deleteSelected() {
	if (SelectedNode != NULL) {
		AvailableIds.push(SelectedNode->_id);
		DrawingZone->getScene().deleteNode(SelectedNode);
		SelectedNode = NULL;
	}
}

void cleanup() {
	destroyShaderProgram();
	destroyBufferObjects();
}

void display() {
	++FrameCount;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearStencil(0);
	drawScene();
	glutSwapBuffers();
}

void idle() {
	glutPostRedisplay();
}

void reshape(int w, int h) {
	WinX = w;
	WinY = h;
	glViewport(0, 0, WinX, WinY);
}

void timer(int value) {
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
		addNode(0);
		break;
	case 'v':
		addNode(1);
		break;
	case 'b':
		addNode(2);
		break;
	case 'n':
		addNode(3);
		break;
	case 'm':
		addNode(4);
		break;
	case 'k':
		addNode(5);
		break;
	case 'd':
		DrawingZone->getScene().deleteAllNodes();
		ID = 1;
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
		DrawingZone->getScene().hideSolids(ReflectionX);
		DrawingZone->getScene().hideSolids(ReflectionZ);
		DrawingZone->getScene().hideSolids(ReflectionO);
		DrawingZone->highlightGrid(0);
		break;
	case '1':
		SymMode = SymmetryMode::XAXIS;
		DrawingZone->getScene().hideSolids(ReflectionZ);
		DrawingZone->getScene().hideSolids(ReflectionO);
		DrawingZone->getScene().showSolids(ReflectionX);
		DrawingZone->highlightGrid(1);
		break;
	case '2':
		SymMode = SymmetryMode::ZAXIS;
		DrawingZone->getScene().hideSolids(ReflectionX);
		DrawingZone->getScene().hideSolids(ReflectionO);
		DrawingZone->getScene().showSolids(ReflectionZ);
		DrawingZone->highlightGrid(2);
		break;
	case '3':
		SymMode = SymmetryMode::O;
		DrawingZone->getScene().hideSolids(ReflectionX);
		DrawingZone->getScene().hideSolids(ReflectionZ);
		DrawingZone->getScene().showSolids(ReflectionO);
		DrawingZone->highlightGrid(3);
		break;

	case '4':
		SymMode = SymmetryMode::XZAXIS;
		DrawingZone->getScene().showSolids(ReflectionX);
		DrawingZone->getScene().showSolids(ReflectionZ);
		DrawingZone->getScene().showSolids(ReflectionO);
		DrawingZone->highlightGrid(4);
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

int DragX = 0;
int DragY = 0;

glm::vec4 Color = glm::vec4(0.9, 0.9, 0.9, 1.0);

void changeSidebarMeshColor() {
	std::vector<SceneNode*> nodes = EditZone->getScene()._nodes;
	std::vector<SceneNode*>::iterator it;
	for (it = nodes.begin(); it != nodes.end(); it++) {
		(*it)->setColor(Color);
	}
}


void nodeSelector(GLfloat data) {
	GLuint id = GLuint(data);

	switch (id) {
	case 240:
		std::cout << "Cube" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.35f;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			//canDrag = true;
		}
		SelectedNode = addNode(0);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.35f;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
			SelectedNode->_isSelected = true;

			canDrag = false;
		}
		break;
	case 241:
		std::cout << "Prism" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.35f;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			//canDrag = true;
		}
		SelectedNode = addNode(2);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.35f;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
			SelectedNode->_isSelected = true;
			canDrag = false;
		}
		break;
	case 242:
		std::cout << "Half cube" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.35f;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			//canDrag = true;
		}
		SelectedNode = addNode(1);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.35f;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
			SelectedNode->_isSelected = true;
			canDrag = false;
		}
		break;
	case 243:
		std::cout << "Quarter cube" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.35f;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			//canDrag = true;
		}
		SelectedNode = addNode(4);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.35f;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
			SelectedNode->_isSelected = true;
			canDrag = false;
		}
		break;
	case 244:
		std::cout << "Half prism" << std::endl;
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y -= 0.35f;
			SelectedNode->setPosition(currentPosition);

			SelectedNode = NULL;
			//canDrag = true;
		}
		SelectedNode = addNode(3);
		if (SelectedNode != NULL) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.y += 0.35f;
			SelectedNode->setPosition(currentPosition);
			SelectedNode->setColor(Color);
			SelectedNode->_isSelected = true;
			canDrag = false;
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
			SelectedNode->setColor(glm::vec4(0.3, 0.3, 0.4, 1.0));
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
			lastMx = x;
			lastMy = y;
			DragX = x;
			DragY = y;

			GLfloat data;
			glReadPixels(MouseX, WinY - MouseY - 1, 1, 1, GL_STENCIL_INDEX, GL_FLOAT, &data);

			std::cout << "Stencil data: " << data << std::endl;
			if (data == 0) {
				if (SelectedNode != NULL) {
					glm::vec3 currentPosition = SelectedNode->_position;
					currentPosition.y -= 0.35f;
					SelectedNode->setPosition(currentPosition);
					SelectedNode->_isSelected = false;
				}

				SelectedNode = NULL;
				canDrag = true;
			} else {
				if (SelectedNode != NULL) {
					if (data != SelectedNode->_id) {
						SelectedNode->_isSelected = false;
						SceneNode* nextNode = DrawingZone->getScene().findNode(GLint(data));
						if (nextNode != NULL) {
							glm::vec3 currentPosition = SelectedNode->_position;
							currentPosition.y -= 0.35f;
							SelectedNode->setPosition(currentPosition);
							
							SelectedNode = nextNode;
							 currentPosition = SelectedNode->_position;
							currentPosition.y += 0.35f;
							SelectedNode->setPosition(currentPosition);
							SelectedNode->_isSelected = true;

							canDrag = false;
						}
					}
				} else {
					SelectedNode = DrawingZone->getScene().findNode(GLint(data));
					if (SelectedNode != NULL) {
						glm::vec3 currentPosition = SelectedNode->_position;
						currentPosition.y += 0.35f;
						SelectedNode->setPosition(currentPosition);
						SelectedNode->_isSelected = true;

						canDrag = false;
					}
				}
				
				nodeSelector(data);
			}
		}
		if (state == GLUT_UP){
			DrawingZone->getCamera()->update(0.0f, 0.0f);
			canDrag = false;
		}
		break;
	}
}


void mouseMotion(int x, int y) {


	if (canDrag){
		DrawingZone->getCamera()->update((GLfloat)(x - lastMx), (GLfloat)(y - lastMy));
		lastMx = x;
		lastMy = y; 
	}

	if (SelectedNode != NULL) {
		int diffX = x - DragX;
		int diffY = y - DragY;

		if (diffX > 20) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.x += 0.125;
			SelectedNode->setPosition(currentPosition);
			DragX = x;

		} else if (diffX < -20) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.x -= 0.125;
			SelectedNode->setPosition(currentPosition);
			DragX = x;
		}

		if (diffY > 20) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.z += 0.125;
			SelectedNode->setPosition(currentPosition);
			DragY = y;

		} else if (diffY < -20) {
			glm::vec3 currentPosition = SelectedNode->_position;
			currentPosition.z -= 0.125;
			SelectedNode->setPosition(currentPosition);
			DragY = y;
		}
	}

}

void wheel(int button, int dir, int x, int y) {

	if (SelectedNode != NULL) {
		GLfloat newAngle;
		if (dir > 0) {
			newAngle = SelectedNode->_angle + 90.0f;
			SelectedNode->setAngle(newAngle);
		} else {
			newAngle = SelectedNode->_angle - 90.0f;
			SelectedNode->setAngle(newAngle);
		}
	} else {
		DrawingZone->getCamera()->zoom(dir);
	}

}

/////////////////////////////////////////////////////////////////////// SETUP

void setupCallbacks() {
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

void setupGLUT(int argc, char* argv[]) {
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

void init(int argc, char* argv[]) {
	setupGLUT(argc, argv);
	setupGLEW();
	setupOpenGL();
	initializeMeshes();
	createShaderProgram();
	createBufferObjects();
	setupCallbacks();

	createTextures();
	createSidebar();
}

int main(int argc, char* argv[]) {
	init(argc, argv);
	glutMainLoop();
	exit(EXIT_SUCCESS);
}

