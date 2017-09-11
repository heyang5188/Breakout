#include "game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include <iostream>


SpriteRenderer  *Renderer;
Game::Game(GLuint width, GLuint height)
	:State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
	delete Renderer;
}

void Game::Init()
{
	// Load shaders
	ResourceManager::LoadShader("sprite.vs", "sprite.frag", nullptr, "sprite");
	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// Load textures
	ResourceManager::LoadTexture("awesomeface.png", GL_TRUE, "face");
	// Set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	//Load textures
	ResourceManager::LoadTexture("background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("block_solid.png", GL_FALSE, "block_solid");
	//¶ÁÈ¡µÈ¼¶
	GameLevel one; one.Load("one.lvl", this->Width, this->Height*0.5);
	GameLevel two; two.Load("two.lvl", this->Width, this->Height*0.5);
	GameLevel three; three.Load("three.lvl", this->Width, this->Height*0.5);
	GameLevel four; four.Load("four.lvl", this->Width, this->Height*0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 1;

}

void Game::ProcessInput(GLfloat dt)
{
}

void Game::Update(GLfloat dt)
{
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		// Draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		// Draw level
		
		this->Levels[this->Level].Draw(*Renderer);
	}
}
