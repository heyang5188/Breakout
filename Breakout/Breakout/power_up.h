#ifndef POWER_UP_H
#define POWER_UP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "GameObject.h"

const glm::vec2 POWERSIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);
class PowerUp :public GameObject
{
public:
	std::string Type;
	GLfloat Duration; //持续时间
	GLboolean Activated; //是否激活

	PowerUp(std::string type, glm::vec3 color, GLfloat duration,
		glm::vec2 position, Texture2D texture)
		: GameObject(position, POWERSIZE, texture, color, VELOCITY),
		Type(type), Duration(duration), Activated()
	{ }
};

#endif // !POWER_UP_H

