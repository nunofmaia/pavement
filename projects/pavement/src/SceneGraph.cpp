#include "SceneGraph.h"

SceneNode::SceneNode() {
	_mesh = NULL;
	_shader = NULL;
}

SceneNode::SceneNode(int id, int shape, Mesh* mesh, ShaderProgram* shader) {
	_mesh = mesh;
	_mesh->loadTextureFile(shader->getProgramId());
	_shader = shader;
	_id = id;
	_shape = shape;
	_canDraw = true;
	_color = glm::vec4(1.0, 0.98, 0.92, 1.0);
	_angle = 0.0f;
	_scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

SceneNode::SceneNode(SceneNode* node, ShaderProgram* p) {
	_id = node->_id;
	_position = node->_position;
	_mesh = new Mesh(node->_mesh);
	_mesh->loadTextureFile(p->getProgramId());
	_color = node->_color;
	_angle = node->_angle;
	_shader = p;
	_scale = node->_scale;
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


void SceneNode::draw() {

	if(_canDraw) {
		if (_shader != NULL) {
			_shader->useShaderProgram();
			_shader->setUniform("ModelMatrix", glm::scale(glm::translate(glm::mat4(1.0), _position), _scale));
			_shader->setUniform("DefaultColor", _color);
			_shader->setUniform("Angle", _angle);
		}
	
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, _id, 0xFF);

		_mesh->draw();

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

