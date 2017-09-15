#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include<glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"
#include <ft2build.h>
#include FT_FREETYPE_H  

struct Character
{
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing; //×ÖÌå¼ä¾à
	GLuint Advance;
};

class TextRenderer
{
public:
	std::map<GLchar, Character> Characters;
	//shader
	Shader TextShader;
	//constructor
	TextRenderer(GLuint width, GLuint height);

	void Load(std::string font, GLuint fontSize);
	//Renders a string
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));
private:
	GLuint VAO, VBO;
};
#endif // !TEXT_RENDERER_H

