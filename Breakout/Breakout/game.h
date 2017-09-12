#ifndef GAME_H
#define GAME_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GameLevel.h"
#include <vector>
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "BallObject.h"
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

class Game
{
public:
	// Game state
	GameState              State;
	GLboolean              Keys[1024];
	GLuint                 Width, Height;
	std::vector<GameLevel> Levels;
	GLuint  Level;
	// Constructor/Destructor
	Game(GLuint width, GLuint height);
	~Game();
	
	// Initialize game state (load all shaders/textures/levels)
	void Init();
	// GameLoop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
	//game check
	void DoCollisions();
	GLboolean CheckCollision(BallObject &, GameObject &);
	GLboolean CheckCollision(GameObject &, GameObject &);
private:
	GameObject *Player;
	BallObject *Ball;
	SpriteRenderer  *Renderer;
};
#endif // !GAME_H

