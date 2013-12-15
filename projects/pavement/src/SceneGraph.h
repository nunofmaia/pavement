#include <vector>

#include "Mesh.h"
#include "Shader.h"

struct SceneNode {

	std::vector<SceneNode*> _copies;
	Mesh* _mesh;
	ShaderProgram* _shader;
	int _id;
	glm::vec3 _position;
	glm::vec4 _color;
	GLfloat _angle;
	glm::vec3 _scale;
	bool _canDraw;

	SceneNode();
	SceneNode(int, Mesh*, ShaderProgram*);
	SceneNode(SceneNode*, ShaderProgram*);
	~SceneNode();
	void setColor(glm::vec4);
	void setPosition(glm::vec3);
	void setAngle(GLfloat);
	void draw();
	void addCopy(SceneNode*);
	void hideCopies(ShaderProgram*);
	void showCopies(ShaderProgram*);

};

struct SceneGraph {
//private:
	std::vector<SceneNode*> _nodes;
public:
	SceneGraph();
	~SceneGraph();
	void draw();
	void addNode(SceneNode*);
	void hideSolids(ShaderProgram*);
	void showSolids(ShaderProgram*);
	void deleteAllNodes();
	SceneNode* findNode(int);
};