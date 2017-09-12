#include "game.h"

#include <iostream>




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
	ResourceManager::LoadTexture("paddle.png", GL_TRUE, "paddle");
	//读取等级
	GameLevel one; one.Load("one", this->Width, this->Height*0.5);
	GameLevel two; two.Load("two", this->Width, this->Height*0.5);
	GameLevel three; three.Load("three", this->Width, this->Height*0.5);
	GameLevel four; four.Load("four", this->Width, this->Height*0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 1;

	//读取小球
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		//move
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}

void Game::Update(GLfloat dt)
{
	//更新对象
	Ball->Move(dt, this->Width);
	// 检测碰撞
	this->DoCollisions();

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
		// Draw player
		Player->Draw(*Renderer);
		//Draw ball
		Ball->Draw(*Renderer);
	}
}

void Game::DoCollisions()
{
	for (auto &box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			if (CheckCollision(*Ball, box))
			{
				if (!box.IsSolid)
					box.Destroyed = GL_TRUE;
			}
		}
	}
}
/*首先计算球心，然后是AABB的半边长及中心。
使用这些碰撞外形的参数，
计算出differenceD¯D¯然后得到限制后的值clamped，
并与AABB中心相加得到closestP¯P¯。
然后计算出center和closest的矢量差D′¯D′¯
并返回两个外形是否碰撞。*/
GLboolean Game::CheckCollision(BallObject & one, GameObject & two)
{
	// 获取圆的中心 
	glm::vec2 center(one.Position + one.Radius);
	// 计算AABB的信息（中心、半边长）
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y
	);
	//获取两个中心的差矢量值
	glm::vec2 difference = center - aabb_center;
	//限制两个中心的差矢量值在半边长以内
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
	glm::vec2 closest = aabb_center + clamped;
	//获得圆心center和最近点clouset的矢量并且判断是否 length <= radius
	difference = closest - center;
	
	return glm::length(difference)<one.Radius;
}
GLboolean Game::CheckCollision(GameObject &one, GameObject &two)
{
	// x轴方向碰撞？
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// y轴方向碰撞？
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// 只有两个轴向都有碰撞时才碰撞
	return collisionX && collisionY;
}



