#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#include <vector>

#include "Mesh.h"
#include "Shader.h"

struct SceneNode {

	GLuint VaoId, VboId[3];
	std::vector<SceneNode*> _copies;
	Mesh* _mesh;
	ShaderProgram* _shader;
	int _id;
	int _shape;
	glm::vec3 _position;
	glm::vec4 _color;
	GLfloat _angle;
	glm::vec3 _scale;
	bool _canDraw, _textureLoaded, _toRevert, _isSelected;
	GLuint _textureId[2];

	SceneNode();
	SceneNode(int, int, Mesh*, ShaderProgram*);
	SceneNode(int, int, Mesh*, ShaderProgram*, GLuint*);
	SceneNode(SceneNode*, ShaderProgram*);
	~SceneNode();
	void setColor(glm::vec4);
	void setPosition(glm::vec3);
	void setAngle(GLfloat);
	void draw();
	void drawTransparencies();
	void createBufferObjects();
	void addCopy(SceneNode*);
	void hideCopies(ShaderProgram*);
	void showCopies(ShaderProgram*);

};

struct SceneGraph {

	std::vector<SceneNode*> _nodes;

	SceneGraph();
	~SceneGraph();
	void draw();
	void drawTransparencies();
	void addNode(SceneNode*);
	void hideSolids(ShaderProgram*);
	void showSolids(ShaderProgram*);
	void deleteAllNodes();
	void deleteNode(SceneNode*);
	SceneNode* findNode(int);
};

#endif
