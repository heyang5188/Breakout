#include "game.h"
#include <algorithm>
#include <iostream>
#include <sstream>

#include "game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "BallObject.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"

#include <irrKlang/irrKlang.h>

// Game-related State data
SpriteRenderer  *Renderer;
GameObject      *Player;
BallObject      *Ball;
ParticleGenerator   *Particles;
PostProcessor  *Effects;
TextRenderer  *Text;
irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();
GLfloat   ShakeTime = 0.0f;

Game::Game(GLuint width, GLuint height)
	:State(GAME_MENU), Keys(), Width(width), Height(height), Level(0), Lives(3)
{
}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	delete Text;
	SoundEngine->drop();
}

void Game::Init()
{
	// Load shaders
	ResourceManager::LoadShader("sprite.vs", "sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("particle.vs", "particle.frag", nullptr, "particle");
	ResourceManager::LoadShader("post_processing.vs", "post_processing.frag", nullptr, "postprocessing");
	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
	// Load textures
	ResourceManager::LoadTexture("awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("particle.png", GL_TRUE, "particle");
	ResourceManager::LoadTexture("background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture("powerup_speed.png", GL_TRUE, "powerup_speed");
	ResourceManager::LoadTexture("powerup_sticky.png", GL_TRUE, "powerup_sticky");
	ResourceManager::LoadTexture("powerup_increase.png", GL_TRUE, "powerup_increase");
	ResourceManager::LoadTexture("powerup_confuse.png", GL_TRUE, "powerup_confuse");
	ResourceManager::LoadTexture("powerup_chaos.png", GL_TRUE, "powerup_chaos");
	ResourceManager::LoadTexture("powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
	// 创建指针
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
	Text = new TextRenderer(this->Width, this->Height);
	//读取等级
	GameLevel one; one.Load("one", this->Width, this->Height*0.5);
	GameLevel two; two.Load("two", this->Width, this->Height*0.5);
	GameLevel three; three.Load("three", this->Width, this->Height*0.5);
	GameLevel four; four.Load("four", this->Width, this->Height*0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	//读取小球
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
	//music
	SoundEngine->play2D("breakout.mp3", GL_TRUE);
	//ttf
	Text->Load("front.TTF", 24);
}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
		}
	}
	if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			Effects->Chaos = GL_FALSE;
			this->State = GAME_MENU;
		}
	}
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
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
			Ball->Stuck = GL_FALSE;
	}
}

void Game::Update(GLfloat dt)
{
	//更新对象
	Ball->Move(dt, this->Width);
	// 检测碰撞
	this->DoCollisions();
	
	//更新粒子
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));
	//update
	this->UpdatePowerUps(dt);
	//震动时间更新
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = false;
	}
	// 球是否接触底部边界？
	if (Ball->Position.y >= this->Height)
	{
		--this->Lives;
		// 玩家是否已失去所有生命值? : 游戏结束
		if (this->Lives == 0)
		{
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	//检测是否赢得胜利
	if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = GL_TRUE;
		this->State = GAME_WIN;
	}

	
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU||this->State == GAME_WIN)
	{
		Effects->BeginRender();
		// Draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		// Draw level
		this->Levels[this->Level].Draw(*Renderer);
		// Draw player
		Player->Draw(*Renderer);
		// Draw PowerUps
		for (PowerUp &powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		//Draw Particles
		Particles->Draw();
		//Draw ball
		Ball->Draw(*Renderer);

		Effects->EndRender();
		Effects->Render(glfwGetTime());

		std::stringstream ss; ss << this->Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", 250.0f, Height / 2, 1.0f);
		Text->RenderText("Press W or S to select level", 248, Height / 2 + 25.0f, 0.75f);
		Text->RenderText("www.heyang5188.site", 280, Height / 2 + 45.0f, 0.75f);
	}
	if (this->State == GAME_WIN)
	{
		// Draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		Text->RenderText(
			"You WON!!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
		);
		Text->RenderText(
			"Press ENTER to retry or ESC to quit", 130.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
		);
	}
}
void Game::ResetLevel()
{
	if (this->Level == 0)this->Levels[0].Load("one", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("two", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("three", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("four", this->Width, this->Height * 0.5f);

	this->Lives = 3;
}

void Game::ResetPlayer()
{
	// Reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
	// 重置道具
	Effects->Chaos = Effects->Confuse = GL_FALSE;
	Ball->PassThrough = Ball->Sticky = GL_FALSE;
	Player->Color = glm::vec3(1.0f);
	Ball->Color = glm::vec3(1.0f);
}
/*
出现增强道具的几率
*/
GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

void Game::UpdatePowerUps(GLfloat dt)
{
	for (PowerUp &powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				// Remove powerup from list (will later be removed)
				powerUp.Activated = GL_FALSE;
				// Deactivate effects
				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
					{	// Only reset if no other PowerUp of type sticky is active
						Ball->Sticky = GL_FALSE;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
					{	// Only reset if no other PowerUp of type pass-through is active
						Ball->PassThrough = GL_FALSE;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
					{	// Only reset if no other PowerUp of type confuse is active
						Effects->Confuse = GL_FALSE;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
					{	// Only reset if no other PowerUp of type chaos is active
						Effects->Chaos = GL_FALSE;
					}
				}
			}
		}
	}
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}
GLboolean ShouldSpawn(GLuint chance)
{
	GLuint random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps(GameObject & block)
{
	if (ShouldSpawn(75)) // 1 in 75 chance
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15)) // Negative powerups should spawn more often
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void ActivatePowerUp(PowerUp &powerUp)
{
	// Initiate a powerup based type of powerup
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = GL_TRUE;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = GL_TRUE;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = GL_TRUE; 
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = GL_TRUE;
	}
}
GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return GL_TRUE;
	}
	return GL_FALSE;
}
// Collision detection
GLboolean CheckCollision(GameObject &one, GameObject &two);
Collision CheckCollision(BallObject &one, GameObject &two);
Direction VectorDirection(glm::vec2 closest);
//碰撞检测
void Game::DoCollisions()
{
	for (GameObject &box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) // If collision is true
			{
				// Destroy block if not solid
				if (!box.IsSolid)
				{
					box.Destroyed = GL_TRUE;
					this->SpawnPowerUps(box);
					SoundEngine->play2D("bleep.mp3", GL_FALSE);
				}
				else
				{   // if block is solid, enable shake effect
					ShakeTime = 0.05f;
					Effects->Shake = GL_TRUE;
					SoundEngine->play2D("solid.wav", GL_FALSE);
				}
				// Collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->PassThrough && !box.IsSolid)) // don't do collision resolution on non-solid bricks if pass-through activated
				{
					if (dir == LEFT || dir == RIGHT) // Horizontal collision
					{
						Ball->Velocity.x = -Ball->Velocity.x; // Reverse horizontal velocity
															  // Relocate
						GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							Ball->Position.x += penetration; // Move ball to right
						else
							Ball->Position.x -= penetration; // Move ball to left;
					}
					else // Vertical collision
					{
						Ball->Velocity.y = -Ball->Velocity.y; // Reverse vertical velocity
															  // Relocate
						GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
							Ball->Position.y -= penetration; // Move ball bback up
						else
							Ball->Position.y += penetration; // Move ball back down
					}
				}
			}
		}
	}

	// Also check collisions on PowerUps and if so, activate them
	for (PowerUp &powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			// 首先判断工具的位置是不是超出范围
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = GL_TRUE;

			if (CheckCollision(*Player, powerUp))
			{	// Collided with player, now activate powerup
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = GL_TRUE;
				powerUp.Activated = GL_TRUE;
				SoundEngine->play2D("powerup.wav", GL_FALSE);
			}
		}
	}
	/*
	球和玩家之间的碰撞与我们之前讨论的碰撞稍有不同，
	因为这里应当基于撞击挡板的点与（挡板）中心的距离来改变球的水平速度。
	撞击点距离挡板的中心点越远，则水平方向的速度就会越大。
	*/
	// 检测球体和玩家的结果；
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// 检测碰到的位置和根据碰到的位置来改变速度；
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);
		// 根据速度移动
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		//将球的速度弄反
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); 
		/*
		玩家挡板以较高的速度移向球，导致球的中心进入玩家挡板。
		由于我们没有考虑球的中心在AABB内部的情况
		游戏会持续试图对所有的碰撞做出响应，当球最终脱离时
		已经对y向速度翻转了多次
		以至于无法确定球在脱离后是向上还是向下运动。
		*/
		//保证这个数字一定是负的
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);

		Ball->Stuck = Ball->Sticky;
		SoundEngine->play2D("bleep.wav", GL_FALSE);
	}
}

/*首先计算球心，然后是AABB的半边长及中心。
使用这些碰撞外形的参数，
计算出differenceD¯D¯然后得到限制后的值clamped，
并与AABB中心相加得到closestP¯P¯。
然后计算出center和closest的矢量差D′¯D′¯
并返回两个外形是否碰撞。*/
GLboolean CheckCollision(GameObject &one, GameObject &two) // 矩形 与 矩形的碰撞检测
{
	//判断X坐标是否相交
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// 判断Y坐标是否相交
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// 只有两个坐标都相交了才会判定成功
	return collisionX && collisionY;
}
/*
首先要获取球心C¯C¯与AABB中心B¯B¯的矢量差D¯D¯。
接下来用AABB的半边长(half-extents)ww和h¯h¯来限制(clamp)矢量D¯D¯。
长方形的半边长是指长方形的中心到它的边的距离；
简单的说就是它的尺寸除以2。
这一过程返回的是一个总是位于AABB的边上的位置矢量。
*/

Collision CheckCollision(BallObject &one, GameObject &two) // 矩形与原型的碰撞检测
{
	// 首先得到圆心
	glm::vec2 center(one.Position + one.Radius);
	//计算AABB的信息 半卞长和中心。
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
	// 得到两个中心的差矢量
	glm::vec2 difference = center - aabb_center;
	//将数据限制在负半边长到正的半边长之间，
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// 现在知道了限制的距离， 加上一个and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// Now retrieve vector between center circle and closest point AABB and check if length < radius
	difference = closest - center;

	if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}
/*
*
*此函数比较了target矢量和compass数组中各方向矢量。
compass数组中与target角度最接近的矢量，
即是返回给函数调用者的Direction。
这里的Direction是一个Game类的头文件中定义的枚举类型：
*/
/*
点乘可以得到两个正交化的矢量的夹角的cos值。
如果我们定义指向北、南、西和东的四个矢量，
然后计算它们和给定矢量的夹角会怎么样？
由这四个方向矢量和给定的矢量点乘积的结果中的最高值
（点乘积的最大值为1.0f，代表0度角）即是矢量的方向。
*/
Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// 上向量
		glm::vec2(1.0f, 0.0f),	// 右
		glm::vec2(0.0f, -1.0f),	// 下
		glm::vec2(-1.0f, 0.0f)	// 左
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	//数字强制转换成为枚举的类型
	return (Direction)best_match;
}


