#ifndef BALLOBJECT_H
#define BALLOBJECT_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GameObject.h"
#include "texture.h"

class BallObject :public GameObject {
public:
	//ÇòµÄ×´Ì¬
	GLfloat Radius;
	GLboolean Stuck;

	BallObject();
	BallObject(glm::vec2 pos,GLfloat radius,glm::vec2 velocity,Texture2D& sprite);

	glm::vec2 Move(GLfloat radius, GLuint window_width);
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif // !BALLOBJECT_H
