#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <glad/glad.h>
#include<vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

class GameLevel {
public:
	std::vector<GameObject> Bricks;
	GameLevel() {}
	void Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);

	void Draw(SpriteRenderer &renderer);

	GLboolean IsComplete();
private:
	void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHight);
};
#endif // !GAMELEVEL_H
