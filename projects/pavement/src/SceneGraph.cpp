#include "SceneGraph.h"

SceneNode::SceneNode() {
	_mesh = NULL;
	_shader = NULL;
}

SceneNode::SceneNode(int id, int shape, Mesh* mesh, ShaderProgram* shader) {
	_mesh = mesh;
	_shader = shader;
	_id = id;
	_shape = shape;
	_canDraw = true;
	_color = glm::vec4(1.0, 0.98, 0.92, 1.0);
	_angle = 0.0f;
	_scale = glm::vec3(1.0f, 1.0f, 1.0f);

	_textureLoaded = false;
	_toRevert = false;
}

SceneNode::SceneNode(int id, int shape, Mesh* mesh, ShaderProgram* shader, GLuint textureId[2]) {
	_mesh = mesh;
	_shader = shader;
	_id = id;
	_shape = shape;
	_canDraw = true;
	_color = glm::vec4(1.0, 0.98, 0.92, 1.0);
	_angle = 0.0f;
	_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	
	_textureLoaded = true;
	_textureId[0] = textureId[0];
	_textureId[1] = textureId[1];
	_toRevert = false;
}

SceneNode::SceneNode(SceneNode* node, ShaderProgram* p) {
	_id = node->_id;
	_position = node->_position;
	//_mesh = new Mesh(node->_mesh);
	_mesh = node->_mesh;
	_color = node->_color;
	_angle = node->_angle;
	_shader = p;
	_scale = node->_scale;

	_textureLoaded = node->_textureLoaded;
	_textureId[0] = node->_textureId[0];
	_textureId[1] = node->_textureId[1];
	_toRevert = false;
}


SceneNode::~SceneNode() {

}


void SceneNode::setColor(glm::vec4 color) {
	_color = color;
	std::vector<SceneNode*>::iterator it;
	for (it = _copies.begin(); it != _copies.end(); it++) {
		(*it)->_color = color;
	}
}

void SceneNode::setPosition(glm::vec3 pos) {
	_position = pos;
	std::vector<SceneNode*>::iterator it;
	for (it = _copies.begin(); it != _copies.end(); it++) {
		(*it)->setPosition(_position);
	}
}

void SceneNode::setAngle(GLfloat angle) {
	_angle = angle;
	std::vector<SceneNode*>::iterator it;
	for (it = _copies.begin(); it != _copies.end(); it++) {
		(*it)->_angle = angle;
	}
}

void SceneNode::createBufferObjects() {

	std::vector<glm::vec4> v = _mesh->_vertices;
	std::vector<glm::vec4> n = _mesh->_normals;
	std::vector<glm::vec2> t = _mesh->_textures;

	if (_toRevert) {
		std::reverse(v.begin(), v.end());
		std::reverse(n.begin(), n.end());
		std::reverse(t.begin(), t.end());
	} else {
		//std::reverse(t.begin(), t.end());
	}



	glGenVertexArrays(1, &VaoId);

	glBindVertexArray(VaoId);

	glGenBuffers(3, VboId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(glm::vec4), &v[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, t.size()*sizeof(glm::vec2), &t[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEXTURES);
	glVertexAttribPointer(TEXTURES, 2, GL_FLOAT, GL_FALSE, sizeof( glm::vec2 ), 0); 

	glBindBuffer(GL_ARRAY_BUFFER, VboId[2]);
	glBufferData(GL_ARRAY_BUFFER, n.size()*sizeof(glm::vec4), &n[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMALS);
	glVertexAttribPointer(NORMALS, 4, GL_FLOAT, GL_FALSE, sizeof( glm::vec4 ), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(VERTICES);
	glDisableVertexAttribArray(TEXTURES);
	glDisableVertexAttribArray(NORMALS);

	_toRevert = false;
	//checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}



void SceneNode::draw() {

	if(_canDraw) {

		if (_shader != NULL) {
			_shader->useShaderProgram();
			_shader->setUniform("ModelMatrix", glm::scale(glm::translate(glm::mat4(1.0), _position), _scale));
			_shader->setUniform("DefaultColor", _color);
			_shader->setUniform("Angle", _angle);
		}

		if (_textureLoaded) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _textureId[0]);
			GLint uniform_mytexture = glGetUniformLocation(_shader->getProgramId(), "texture_uniform");
			glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, _textureId[1]);
			uniform_mytexture = glGetUniformLocation(_shader->getProgramId(), "noise_texture_uniform");
			glUniform1i(uniform_mytexture, /*GL_TEXTURE*/1);
		}
		


		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, _id, 0xFF);


		glBindVertexArray(VaoId);
		_mesh->draw();
		glBindVertexArray(0);

		glDisable(GL_STENCIL_TEST);

		for (std::vector<SceneNode*>::iterator it = _copies.begin(); it != _copies.end(); ++it) {
			(*it)->draw();
		}
	}
}

void SceneNode::addCopy(SceneNode* copy) {
	_copies.push_back(copy);
}

void SceneNode::hideCopies(ShaderProgram* p) {
	std::vector<SceneNode*>::iterator it;
	for (it = _copies.begin(); it != _copies.end(); ++it) {
		if ((*it)->_shader == p) {
			(*it)->_canDraw = false;
		}
	}
}

void SceneNode::showCopies(ShaderProgram* p) {
	std::vector<SceneNode*>::iterator it;
	for (it = _copies.begin(); it != _copies.end(); ++it) {
		if ((*it)->_shader == p) {
			(*it)->_canDraw = true;
		}
	}
}

SceneGraph::SceneGraph() {

}

SceneGraph::~SceneGraph() {

}

void SceneGraph::draw() {
	for (std::vector<SceneNode*>::iterator it = _nodes.begin(); it != _nodes.end(); ++it) {
		(*it)->draw();
	}
}

void SceneGraph::addNode(SceneNode* node) {
	_nodes.push_back(node);
}

void SceneGraph::hideSolids(ShaderProgram* p) {
	std::vector<SceneNode*>::iterator it;
	for (it = _nodes.begin(); it != _nodes.end(); ++it) {
		(*it)->hideCopies(p);
	}
}

void SceneGraph::showSolids(ShaderProgram* p) {
	std::vector<SceneNode*>::iterator it;
	for (it = _nodes.begin(); it != _nodes.end(); ++it) {
		(*it)->showCopies(p);
	}
}

void SceneGraph::deleteAllNodes() {
	_nodes.clear();
}

void SceneGraph::deleteNode(SceneNode* n) {
	std::vector<SceneNode*>::iterator it;
	for (it = _nodes.begin(); it != _nodes.end(); ++it) {
		if ((*it)->_id == n->_id) {
			_nodes.erase(it);
			return;
		}
	}
}

SceneNode* SceneGraph::findNode(int id) {
	std::vector<SceneNode*>::iterator it;
	for (it = _nodes.begin(); it != _nodes.end(); ++it) {
		if ((*it)->_id == id) {
			return (*it);
		}
	}

	return NULL;
}

