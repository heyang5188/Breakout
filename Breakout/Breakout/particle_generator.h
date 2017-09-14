#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
#include "GameObject.h"

struct Particle {
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particle() :Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleGenerator
{
public:
	//Constructor 栗子构造器
	ParticleGenerator(Shader shader,Texture2D texture,GLuint amount);
	//栗子更新函数（延迟，对象，数量，偏移量）
	void Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	//rander all particles
	void Draw();
private:
	//粒子集合
	std::vector<Particle> particles;
	GLuint amount;
	Shader shader;
	Texture2D texture;
	//顶点数组对象
	GLuint VAO;
	void init();
	/*找到第一个消亡的粒子然后用一个新产生的粒子来更新它.
	函数`FirstUnuseParticle`就是试图找到第一个消亡的粒子并且返回它的索引值给
	调用者.*/
	//第一个无效的粒子
	GLuint firstUnusedParticle();
	//
	void respawParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

};
#endif // !PARTICLE_GENERATOR_H

