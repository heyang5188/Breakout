#ifndef GAME_H
#define GAME_H
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameLevel.h"
#include "power_up.h"


enum GameState {
	GAME_MENU,
	GAME_ACTIVE,
	GAME_WIN
};


enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};
// Defines a Collision typedef that represents collision data
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>

															   // Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(888.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -500.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;


class Game
{
public:
	// Game state
	GLuint Lives;
	GameState              State;
	GLboolean              Keys[1024];
	GLboolean			KeysProcessed[1024];
	GLuint                 Width, Height;
	std::vector<GameLevel> Levels;
	GLuint                 Level;
	std::vector<PowerUp>  PowerUps;
	// Constructor/Destructor
	Game(GLuint width, GLuint height);
	~Game();

	void Init();
	// GameLoop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
	void DoCollisions();
	// Reset
	void ResetLevel();
	void ResetPlayer();
	//µÀ¾ß
	void SpawnPowerUps(GameObject &block);
	void UpdatePowerUps(GLfloat dt);
};

#endif

