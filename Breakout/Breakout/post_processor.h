#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "texture.h"
#include "SpriteRenderer.h"
#include "shader.h"

class PostProcessor {
public:
	Shader PostProcessingShader;
	Texture2D Texture;
	GLuint Width, Height;
	//options
	GLboolean Confuse, Chaos, Shake;
	//¹¹ÔìÆ÷
	PostProcessor(Shader shader, GLuint width, GLuint height);

	void BeginRender();

	void EndRender();

	void Render(GLfloat);
private:
	GLuint MSFBO, FBO;
	GLuint RBO;
	GLuint VAO;

	void initRenderData();
};
#endif // !POST_PRICESSOR_H
