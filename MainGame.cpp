#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <algorithm>
#include <iostream>


constexpr int DISPLAY_WIDTH{ 800 };
constexpr int DISPLAY_HEIGHT{ 600 };
constexpr int DISPLAY_SCALE{ 1 };

const Point2f POD_START_POS{ 150, 400 };
const int facingLeft{ 1 };
const int facingRight{ 2 };
int direction{ facingLeft };
const float gravity{ 0.5f };
const Vector2D FISHPOD_JUMP_LEFT_VELOCITY{ -3, -12 };
const Vector2D FISHPOD_JUMP_RIGHT_VELOCITY{ 3, -12 };
const Vector2D TILE_AABB{ 16.f, 10.f };
const Vector2D TILE_BEGIN_AABB{ 5.f, 10.f };
const Vector2D TILE_END_AABB{ 5.f, 10.f };
const Vector2D AABB_ORIGIN{ 10.f,0.f };
const Vector2D POD_AABB{ 10.f, 16.f };
const Vector2D PANSY_AABB{ 10.f, 15.f };
const Vector2D GOLD_AABB{ 14.f, 14.f };

void Draw();
void DrawUI();
void UpdateFishPod();
void FishPodMove();
void FishPodStand();
void FishPodJump();
void FishPodFall();
bool RockPlatformCollision();
void WallCollision();
bool PansyCollision();
void GoldUpdate();
bool GoldCollision();
bool LavaCollision();
void UpdateDestroyed();


enum FishPodState
{
	STATE_STAND,
	STATE_MOVE,
	STATE_JUMP,
	STATE_FALL,
	STATE_DEAD,
};

enum PlayState
{
	STATE_START,
	STATE_PLAY,
	STATE_WIN,
	STATE_GAMEOVER,
};

enum GameObjectType
{
	TYPE_POD,
	TYPE_PLATFORM_BEGIN,
	TYPE_PLATFORM,
	TYPE_PLATFORM_END,
	TYPE_LAVA,
	TYPE_GOLD,
	TYPE_PANSY,
	TYPE_DESTROYED,
};


struct Tile
{
	const int TILE_HEIGHT = 32;
	const int TILE_WIDTH = 32;
};

Tile obj_tile;


struct FishPod
{
	Point2f podPos{ POD_START_POS };
};

FishPod fishpod;


struct GameState
{
	float time{ 0 };
	float gold{ 4 };
	bool collision{ false };
	PlayState playState{ STATE_START };
	FishPodState fishState{ STATE_STAND };
};

GameState gameState;

void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::LoadBackground("Data\\Backgrounds\\background.png");

	//setting up a 1D array for tiles
	int tileId;
	const int width{ 26 };
	const int height{ 19 };
	const int size = width * height;
	int tileMap[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,7,0,8,0,0,7,0,0,0,0,0,0,0,0,0,0,
		0,0,1,2,2,3,0,0,0,1,2,2,2,2,2,2,2,3,0,0,0,0,0,7,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,3,0,
		0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,1,2,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,3,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,2,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,1,2,2,2,2,2,2,3,0,0,0,0,8,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		4,5,6,2,2,2,2,2,2,2,2,4,5,5,5,6,2,2,2,2,2,2,2,2,2,3,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			tileId = tileMap[(y * width) + x];
			switch (tileId)
			{
			case 1:
				Play::CreateGameObject(TYPE_PLATFORM_BEGIN, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "rock_begin");
				break;

			case 2:
				Play::CreateGameObject(TYPE_PLATFORM, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "rock_middle");
				break;

			case 3:
				Play::CreateGameObject(TYPE_PLATFORM_END, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "rock_end");
				break;

			case 4:
				Play::CreateGameObject(TYPE_LAVA, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "lava_begin");
				break;

			case 5:
				Play::CreateGameObject(TYPE_LAVA, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "lava_middle");
				break;

			case 6:
				Play::CreateGameObject(TYPE_LAVA, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "lava_end");
				break;

			case 7:
				Play::CreateGameObject(TYPE_PANSY, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "pansy");
				break;

			case 8:
				Play::CreateGameObject(TYPE_GOLD, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "gold");
				break;

			default:

				break;
			}
		}
	}

	Play::CreateGameObject(TYPE_POD, { fishpod.podPos }, 20, "pod_stand_left");
	Play::CentreAllSpriteOrigins();
	Play::MoveSpriteOrigin("pod_stand_right", 0, 15);
	Play::MoveSpriteOrigin("pod_stand_left", 0, 15);
	Play::MoveSpriteOrigin("pod_walk_left", 0, 15);
	Play::MoveSpriteOrigin("pod_walk_right", 0, 15);
	

}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	gameState.time += elapsedTime;
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);

	switch (gameState.playState)
	{
		case STATE_START:
			RockPlatformCollision();
			if (Play::KeyPressed('P') == true)
			{
				gameState.playState = STATE_PLAY;
				gameState.fishState = STATE_FALL;
				obj_pod.velocity = { 0,0 };
				Play::StartAudioLoop("thunder");
			}

			break;

		case STATE_PLAY:
			UpdateFishPod();
			PansyCollision();
			GoldUpdate();
			GoldCollision();
			UpdateDestroyed();
			break;

		case STATE_WIN:
			Play::StopAudioLoop("thunder");
			break;

		case STATE_GAMEOVER:
			//UpdateFishPod();
			Play::StopAudioLoop("thunder");
			if (Play::KeyPressed('R') == true)
			{
				obj_pod.pos = { POD_START_POS };
				gameState.playState = STATE_START;
			}

			break;

	}

	Draw();

	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

void Draw()
{
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	//draw platforms

	std::vector<int> vPlatformsBegin = Play::CollectGameObjectIDsByType(TYPE_PLATFORM_BEGIN);
	for (int i : vPlatformsBegin)
	{
		GameObject& platformBegin = Play::GetGameObject(i);
		Play::DrawObject(platformBegin);
		//Play::DrawRect(platformBegin.pos - TILE_BEGIN_AABB + AABB_ORIGIN, platformBegin.pos + TILE_BEGIN_AABB + AABB_ORIGIN, Play::cOrange);
	}

	std::vector<int> vPlatformsEnd = Play::CollectGameObjectIDsByType(TYPE_PLATFORM_END);
	for (int i : vPlatformsEnd)
	{
		GameObject& platformEnd = Play::GetGameObject(i);
		Play::DrawObject(platformEnd);
		//Play::DrawRect(platformEnd.pos - TILE_END_AABB - AABB_ORIGIN, platformEnd.pos + TILE_END_AABB - AABB_ORIGIN, Play::cOrange);
	}

	std::vector<int> vPlatformsMid = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int i : vPlatformsMid)
	{
		GameObject& platformMid = Play::GetGameObject(i);
		Play::DrawObject(platformMid);
		//Play::DrawRect(platformMid.pos - TILE_AABB, platformMid.pos + TILE_AABB, Play::cGreen);
	}

	//draw lava
	std::vector<int> vLavaMid = Play::CollectGameObjectIDsByType(TYPE_LAVA);
	for (int i : vLavaMid)
	{
		GameObject& lavaMid = Play::GetGameObject(i);
		Play::DrawObject(lavaMid);
		//Play::DrawRect(lavaMid.pos - TILE_AABB, lavaMid.pos + TILE_AABB, Play::cGreen);
	}

	//draw other objs
	std::vector<int> pansy_id = Play::CollectGameObjectIDsByType(TYPE_PANSY);
	for (int j : pansy_id)
	{
		GameObject& pansy = Play::GetGameObject(j);
		Play::DrawObject(pansy);
	}

	std::vector<int> gold_id = Play::CollectGameObjectIDsByType(TYPE_GOLD);
	for (int k : gold_id)
	{
		GameObject& gold = Play::GetGameObject(k);
		Play::DrawObject(gold);
	}

	//draw pod
	Play::DrawObject(Play::GetGameObjectByType(TYPE_POD));
	GameObject& obj_pod{ Play::GetGameObjectByType(TYPE_POD) };
	//Play::DrawRect(obj_pod.pos - POD_AABB, obj_pod.pos + POD_AABB, Play::cWhite); // bounding box visual

	DrawUI();

	Play::PresentDrawingBuffer();
}

void DrawUI()
{
	switch (gameState.playState)
	{
		case STATE_START:
			Play::DrawFontText("64px", "COLLECT THE GOLD AND AVOID THE PANSIES AND WALLS",
				{ DISPLAY_WIDTH / 2, 400 }, Play::CENTRE);
			Play::DrawFontText("64px", "PRESS P TO PLAY",
				{ DISPLAY_WIDTH / 2,  350 }, Play::CENTRE);
			break;

		case STATE_PLAY:
			Play::DrawFontText("64px", "LEFT AND RIGHT TO MOVE, SPACE TO JUMP",
					{ DISPLAY_WIDTH / 2,  50 }, Play::CENTRE);
			break;

		case STATE_WIN:
			Play::DrawFontText("64px", "YOU WON! PLAY AGAIN? PRESS P TO PLAY AGAIN",
				{ DISPLAY_WIDTH / 2, 50 }, Play::CENTRE);
			break;

		case STATE_GAMEOVER:
			Play::DrawFontText("105px", "GAME OVER :(", 
				{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
			Play::DrawFontText("64px", "PLAY AGAIN? PRESS R TO RESTART", 
				{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50 }, Play::CENTRE);
			break;
	}
}

void UpdateFishPod()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);

	switch (gameState.fishState)
	{
		case STATE_STAND:
			FishPodStand();
			WallCollision();

			break;

		case STATE_MOVE:

			if (RockPlatformCollision() == false)
			{
				gameState.fishState = STATE_FALL;
			}

			FishPodMove();
			LavaCollision();
			WallCollision();
			GoldCollision();

			break;

		case STATE_JUMP:

			obj_pod.acceleration.y = gravity;
			LavaCollision();
			RockPlatformCollision();
			FishPodJump();
			WallCollision();
			GoldCollision();

			break;


		case STATE_FALL:
			FishPodFall();
			RockPlatformCollision();
			LavaCollision();
			WallCollision();
			GoldCollision();

			break;

		case STATE_DEAD:
			LavaCollision();
			gameState.playState = STATE_GAMEOVER;

			break;
	}

	Play::UpdateGameObject(obj_pod);
}

void FishPodMove()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);
	obj_pod.velocity = { 0, 0 };
	obj_pod.acceleration.y = 0;


	if (Play::KeyDown(VK_LEFT))
	{
		obj_pod.pos.x -= 3;
		Play::SetSprite(obj_pod, "pod_walk_left", 0.75f);
	}

	if (Play::KeyDown(VK_RIGHT))
	{
		obj_pod.pos.x += 3;
		Play::SetSprite(obj_pod, "pod_walk_right", 0.75f);

	}

	if (!Play::KeyDown(VK_LEFT) && !Play::KeyDown(VK_RIGHT))
	{
		gameState.fishState = STATE_STAND;
	}
	 
	
}

void FishPodJump()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);

	if (direction == facingLeft)
	{
		Play::SetSprite(obj_pod, "pod_jump_left", 0.75f);
	}

	if (direction == facingRight)
	{
		Play::SetSprite(obj_pod, "pod_jump_right", 0.75f);
	}

}

void FishPodFall()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);
	obj_pod.acceleration.y = gravity;

	if (direction == facingLeft)
	{
		Play::SetSprite(obj_pod, "pod_fall_left", 0.75f);
	}

	if (direction == facingRight)
	{
		Play::SetSprite(obj_pod, "pod_fall_right", 0.75f);
	}
}

void FishPodStand()
{
	
	GameObject& obj_pod { Play::GetGameObjectByType(TYPE_POD) };

	if (Play::KeyPressed(VK_LEFT))
	{
		direction = facingLeft;
	}
	if (Play::KeyPressed(VK_RIGHT))
	{
		direction = facingRight;
	}

	if (direction == facingLeft)
	{
		obj_pod.velocity.x = 0;
		Play::SetSprite(obj_pod, "pod_stand_left", 0.25f);
	}
	if (direction == facingRight)
	{
		obj_pod.velocity.x = 0;
		Play::SetSprite(obj_pod, "pod_stand_right", 0.25f);
	}

	if (direction == facingRight && Play::KeyPressed(VK_SPACE))
	{
		gameState.fishState = STATE_JUMP;
		obj_pod.acceleration.y = gravity;
		obj_pod.velocity = { FISHPOD_JUMP_RIGHT_VELOCITY };
	}

	if (direction == facingLeft && Play::KeyPressed(VK_SPACE))
	{
		gameState.fishState = STATE_JUMP;
		obj_pod.acceleration.y = gravity;
		obj_pod.velocity = { FISHPOD_JUMP_LEFT_VELOCITY };
	}

	if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT))
	{
		gameState.fishState = STATE_MOVE;
	}
}

bool RockPlatformCollision()
{
	GameObject& obj_pod(Play::GetGameObjectByType(TYPE_POD));
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	std::vector<int> vPlatformsBegin = Play::CollectGameObjectIDsByType(TYPE_PLATFORM_BEGIN);
	std::vector<int> vPlatformsEnd = Play::CollectGameObjectIDsByType(TYPE_PLATFORM_END);

	for (int i : vPlatforms)
	{
		GameObject& obj_platform = Play::GetGameObject(i);
			
		if (obj_pod.pos.y + POD_AABB.y > obj_platform.pos.y - TILE_AABB.y
			&& obj_pod.pos.y - POD_AABB.y < obj_platform.pos.y + TILE_AABB.y)
		{

			if (obj_pod.pos.x - POD_AABB.x < obj_platform.pos.x + TILE_AABB.x
				&& obj_pod.pos.x + POD_AABB.x > obj_platform.pos.x - TILE_AABB.x)
			{
				if (obj_pod.pos.y < obj_platform.pos.y == true)
				{
					gameState.fishState = STATE_STAND;
					obj_pod.acceleration.y = 0;
					obj_pod.velocity = { 0,0 };
					return true;
				}

				if (obj_pod.oldPos.y > obj_platform.pos.y == true)
				{
					obj_pod.velocity = { 0,0 };
					gameState.fishState = STATE_FALL;
				}
			}
			
		}

	}

	for (int j : vPlatformsBegin)
	{
		GameObject& obj_platform_begin = Play::GetGameObject(j);

		if (obj_pod.pos.y + POD_AABB.y > obj_platform_begin.pos.y - TILE_AABB.y + AABB_ORIGIN.y
			&& obj_pod.pos.y - POD_AABB.y < obj_platform_begin.pos.y + TILE_AABB.y + AABB_ORIGIN.y)
		{

			if (obj_pod.pos.x - POD_AABB.x < obj_platform_begin.pos.x + TILE_AABB.x + AABB_ORIGIN.x
				&& obj_pod.pos.x + POD_AABB.x > obj_platform_begin.pos.x - TILE_AABB.x + AABB_ORIGIN.x)
			{
				if (obj_pod.pos.y < obj_platform_begin.pos.y == true)
				{
					gameState.fishState = STATE_STAND;
					obj_pod.acceleration.y = 0;
					obj_pod.velocity = { 0,0 };
					return true;
				}

				if (obj_pod.oldPos.y > obj_platform_begin.pos.y == true)
				{
					obj_pod.velocity = { 0,0 };
					gameState.fishState = STATE_FALL;
				}
			}

		}

	}


	for (int k : vPlatformsEnd)
	{
		GameObject& obj_platform_end = Play::GetGameObject(k);

		if (obj_pod.pos.y + POD_AABB.y > obj_platform_end.pos.y - TILE_AABB.y - AABB_ORIGIN.y
			&& obj_pod.pos.y - POD_AABB.y < obj_platform_end.pos.y + TILE_AABB.y - AABB_ORIGIN.y)
		{

			if (obj_pod.pos.x - POD_AABB.x < obj_platform_end.pos.x + TILE_AABB.x - AABB_ORIGIN.x
				&& obj_pod.pos.x + POD_AABB.x > obj_platform_end.pos.x - TILE_AABB.x - AABB_ORIGIN.x)
			{
				if (obj_pod.pos.y < obj_platform_end.pos.y == true)
				{
					gameState.fishState = STATE_STAND;
					obj_pod.acceleration.y = 0;
					obj_pod.velocity = { 0,0 };
					return true;
				}

				if (obj_pod.oldPos.y > obj_platform_end.pos.y == true)
				{
					obj_pod.velocity = { 0,0 };
					gameState.fishState = STATE_FALL;
				}

			}

		}

	}
	
	return false;
}

bool LavaCollision()
{
	GameObject& obj_pod(Play::GetGameObjectByType(TYPE_POD));
	std::vector<int> vLava = Play::CollectGameObjectIDsByType(TYPE_LAVA);

	for (int i : vLava)
	{
		GameObject& obj_lava = Play::GetGameObject(i);

		if (obj_pod.pos.y + POD_AABB.y > obj_lava.pos.y - TILE_AABB.y
			&& obj_pod.pos.y - POD_AABB.y < obj_lava.pos.y + TILE_AABB.y)
		{

			if (obj_pod.pos.x - POD_AABB.x < obj_lava.pos.x + TILE_AABB.x
				&& obj_pod.pos.x + POD_AABB.x > obj_lava.pos.x - TILE_AABB.x)
			{
				obj_pod.acceleration.y = 0;
				obj_pod.velocity = { 0,0 };
				Play::PlayAudio("die");
				gameState.fishState = STATE_DEAD;
				return true;
			}

		}

	}

	return false;
}

bool PansyCollision()
{
	GameObject& obj_pod(Play::GetGameObjectByType(TYPE_POD));
	std::vector<int> vPansy = Play::CollectGameObjectIDsByType(TYPE_PANSY);

	for (int i : vPansy)
	{
		GameObject& obj_pansy = Play::GetGameObject(i);

		if (obj_pod.pos.y + POD_AABB.y > obj_pansy.pos.y - PANSY_AABB.y
			&& obj_pod.pos.y - POD_AABB.y < obj_pansy.pos.y + PANSY_AABB.y)
		{
			if (obj_pod.pos.x + POD_AABB.x > obj_pansy.pos.x - PANSY_AABB.x
				&& obj_pod.pos.x - POD_AABB.x < obj_pansy.pos.x + PANSY_AABB.x)
			{
				gameState.fishState = STATE_DEAD;
				Play::PlayAudio("die");
				return true;
				
			}

		}

	}

	return false;

}

void GoldUpdate()
{
	std::vector<int> vGold = Play::CollectGameObjectIDsByType(TYPE_GOLD);

	for (int i : vGold)
	{
		GameObject& obj_gold = Play::GetGameObject(i);
		Play::SetSprite(obj_gold, "gold", 0.25f);
		Play::UpdateGameObject(obj_gold);
	}
}

bool GoldCollision()
{
	GameObject& obj_pod(Play::GetGameObjectByType(TYPE_POD));
	std::vector<int> vGold = Play::CollectGameObjectIDsByType(TYPE_GOLD);

	bool Collision = false;

	for (int i : vGold)
	{
		GameObject& obj_gold = Play::GetGameObject(i);


		if (obj_pod.pos.y + POD_AABB.y > obj_gold.pos.y - GOLD_AABB.y 
			&& obj_pod.pos.y - POD_AABB.y < obj_gold.pos.y + GOLD_AABB.y)
		{
			if (obj_pod.pos.x + POD_AABB.x > obj_gold.pos.x - GOLD_AABB.x 
				&& obj_pod.pos.x - POD_AABB.x < obj_gold.pos.x + GOLD_AABB.x)
			{
					obj_gold.type = TYPE_DESTROYED;
					gameState.gold -= 1;
					Play::PlayAudio("gold");
					Collision = true;

					if (gameState.gold <= 0)
					{
						gameState.playState = STATE_WIN;
					}
			}

		}

		Play::UpdateGameObject(Play::GetGameObject(i));

	}

		return Collision;

}

void WallCollision()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);
	
	if (obj_pod.pos.x < 0 || obj_pod.pos.x > DISPLAY_WIDTH)
	{
		obj_pod.pos.x = std::clamp(obj_pod.pos.x, 0.f, (float) DISPLAY_WIDTH);
		Play::PlayAudio("die");
		gameState.fishState = STATE_DEAD;
	}

	if (obj_pod.pos.y > DISPLAY_HEIGHT)
	{
		obj_pod.pos.y = std::clamp(obj_pod.pos.y, 0.f, (float) DISPLAY_HEIGHT);
		Play::PlayAudio("die");
		gameState.fishState = STATE_DEAD;

	}
}

void UpdateDestroyed()
{
	std::vector<int> vDead = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);

	for (int id_dead : vDead)
	{
		GameObject& obj_dead = Play::GetGameObject(id_dead);
		obj_dead.animSpeed = 0.2f;
		Play::UpdateGameObject(obj_dead);

		if (obj_dead.frame % 2)
			Play::DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);

		if (!Play::IsVisible(obj_dead) || obj_dead.frame >= 10)
			Play::DestroyGameObject(id_dead);
	}
}