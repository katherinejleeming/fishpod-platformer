#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <algorithm>
#include <iostream>


constexpr int DISPLAY_WIDTH{ 800 };
constexpr int DISPLAY_HEIGHT{ 600 };
constexpr int DISPLAY_SCALE{ 1 };

const Point2f POD_START_POS{ 150, 530 };
const int facingLeft{ 1 };
const int facingRight{ 2 };
int direction{ facingLeft };
const float gravity{ 0.5f };
const Vector2D FISHPOD_JUMP_LEFT_VELOCITY{ -3, -12 };
const Vector2D FISHPOD_JUMP_RIGHT_VELOCITY{ 3, -12 };
const Vector2D TILE_AABB{ 16.f, 10.f };
const Vector2D TILE_BEGIN_AABB{ 16.f, 10.f };
const Vector2D TILE_END_AABB{ 16.f, 10.f };
const Vector2D POD_AABB{ 10.f, 20.f };
const Vector2D PANSY_AABB{ 10.f, 15.f };
const Vector2D GOLD_AABB{ 14.f, 14.f };

void Draw();
void DrawUI();
void UpdateFishPod();
void FishPodGroundControls();
void FishPodStand();
void FishPodJump();
bool RockPlatformCollision();
void WallCollision();
bool PansyCollision();
void GoldUpdate();
bool GoldCollision();


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
	STATE_PAUSE,
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
	TYPE_LAVA_BEGIN,
	TYPE_LAVA,
	TYPE_LAVA_END,
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
	bool collision{ false };
	PlayState playState{ STATE_START };
	FishPodState fishState{ STATE_STAND };
};

GameState gameState;


// The entry point for a PlayBuffer program
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
				Play::CreateGameObject(TYPE_LAVA_BEGIN, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "lava_begin");
				break;

			case 5:
				Play::CreateGameObject(TYPE_LAVA, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "lava_middle");
				break;

			case 6:
				Play::CreateGameObject(TYPE_LAVA_END, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 10, "lava_end");
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
				gameState.fishState = STATE_MOVE;
				obj_pod.pos = { POD_START_POS };
				Play::StartAudioLoop("thunder");
			}

			break;

		case STATE_PLAY:
			UpdateFishPod();
			PansyCollision();
			GoldUpdate();
			GoldCollision();

			break;

		case STATE_PAUSE:

			break;

		case STATE_WIN:

			break;

		case STATE_GAMEOVER:
			UpdateFishPod();
			Play::StopAudioLoop("thunder");
			if (Play::KeyPressed('R') == true)
			{
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
	std::vector<int> vPlatformsMid = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int i : vPlatformsMid)
	{
		GameObject& platformMid = Play::GetGameObject(i);
		Play::DrawObject(platformMid);
		Play::DrawRect(platformMid.pos - TILE_AABB, platformMid.pos + TILE_AABB, Play::cGreen);
	}

	std::vector<int> vPlatformsBegin = Play::CollectGameObjectIDsByType(TYPE_PLATFORM_BEGIN);
	for (int i : vPlatformsBegin)
	{
		GameObject& platformBegin = Play::GetGameObject(i);
		Play::DrawObject(platformBegin);
		Play::DrawRect(platformBegin.pos - TILE_BEGIN_AABB, platformBegin.pos + TILE_BEGIN_AABB, Play::cOrange);
	}

	std::vector<int> vPlatformsEnd = Play::CollectGameObjectIDsByType(TYPE_PLATFORM_END);
	for (int i : vPlatformsEnd)
	{
		GameObject& platformEnd = Play::GetGameObject(i);
		Play::DrawObject(platformEnd);
		Play::DrawRect(platformEnd.pos - TILE_END_AABB, platformEnd.pos + TILE_END_AABB, Play::cOrange);
	}


	//draw lava
	std::vector<int> vLavaMid = Play::CollectGameObjectIDsByType(TYPE_LAVA);
	for (int i : vLavaMid)
	{
		GameObject& lavaMid = Play::GetGameObject(i);
		Play::DrawObject(lavaMid);
		Play::DrawRect(lavaMid.pos - TILE_AABB, lavaMid.pos + TILE_AABB, Play::cGreen);
	}

	std::vector<int> vlavaBegin = Play::CollectGameObjectIDsByType(TYPE_LAVA_BEGIN);
	for (int i : vlavaBegin)
	{
		GameObject& lavaBegin = Play::GetGameObject(i);
		Play::DrawObject(lavaBegin);
		Play::DrawRect(lavaBegin.pos - TILE_BEGIN_AABB, lavaBegin.pos + TILE_BEGIN_AABB, Play::cOrange);
	}

	std::vector<int> vlavaEnd = Play::CollectGameObjectIDsByType(TYPE_LAVA_END);
	for (int i : vlavaEnd)
	{
		GameObject& lavaEnd = Play::GetGameObject(i);
		Play::DrawObject(lavaEnd);
		Play::DrawRect(lavaEnd.pos - TILE_END_AABB, lavaEnd.pos + TILE_END_AABB, Play::cOrange);
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
	Play::DrawRect(obj_pod.pos - POD_AABB, obj_pod.pos + POD_AABB, Play::cWhite); // bounding box visual

	DrawUI();

	Play::PresentDrawingBuffer();
}

void DrawUI()
{
	switch (gameState.playState)
	{
		case STATE_START:
			Play::DrawFontText("64px", "COLLECT THE GOLD AND AVOID THE PANSIES",
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
				{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
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

			FishPodGroundControls();
			WallCollision();
			GoldCollision();

			break;

		case STATE_JUMP:

			obj_pod.acceleration.y = gravity;
			RockPlatformCollision();
			FishPodJump();
			WallCollision();
			GoldCollision();

			break;


		case STATE_FALL:
			obj_pod.acceleration.y = gravity;
			RockPlatformCollision();
			WallCollision();
			GoldCollision();

			break;

		case STATE_DEAD:

			gameState.playState = STATE_GAMEOVER;

			break;
	}

	Play::UpdateGameObject(obj_pod);
}

void FishPodGroundControls()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);
	obj_pod.velocity = { 0, 0 };
	obj_pod.acceleration.y = 0;

	if (Play::KeyDown(VK_LEFT))
	{
		obj_pod.pos.x -= 1;
		Play::SetSprite(obj_pod, "pod_walk_left", 0.75f);
	}

	if (Play::KeyDown(VK_RIGHT))
	{
		obj_pod.pos.x += 1;
		Play::SetSprite(obj_pod, "pod_walk_right", 0.75f);

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

	if (obj_pod.velocity.y > 0)
	{
		
		gameState.fishState = STATE_FALL;
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

	if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT))
	{
		gameState.fishState = STATE_MOVE;
	}
}

bool RockPlatformCollision()
{
	GameObject& obj_pod(Play::GetGameObjectByType(TYPE_POD));
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);

	for (int i : vPlatforms)
	{
		GameObject& obj_platform = Play::GetGameObject(i);
			
		if (obj_pod.pos.y + POD_AABB.y > obj_platform.pos.y - TILE_AABB.y
			&& obj_pod.pos.y - POD_AABB.y < obj_platform.pos.y + TILE_AABB.y)
		{

			if (obj_pod.pos.x - POD_AABB.x < obj_platform.pos.x + TILE_AABB.x
				&& obj_pod.pos.x + POD_AABB.x > obj_platform.pos.x - TILE_AABB.x)
			{
				gameState.fishState = STATE_STAND;
				obj_pod.acceleration.y = 0;
				obj_pod.velocity = { 0,0 };
				return true;
			}
			
		}

	}
	
	return false;
}

bool LavaPlatformCollision()
{
	GameObject& obj_pod(Play::GetGameObjectByType(TYPE_POD));
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_LAVA);

	for (int i : vPlatforms)
	{
		GameObject& obj_platform = Play::GetGameObject(i);

		if (obj_pod.pos.y + POD_AABB.y > obj_platform.pos.y - TILE_AABB.y
			&& obj_pod.pos.y - POD_AABB.y < obj_platform.pos.y + TILE_AABB.y)
		{

			if (obj_pod.pos.x - POD_AABB.x < obj_platform.pos.x + TILE_AABB.x
				&& obj_pod.pos.x + POD_AABB.x > obj_platform.pos.x - TILE_AABB.x)
			{
				gameState.fishState = STATE_DEAD;
				Play::PlayAudio("die");
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
					Play::PlayAudio("gold");
					Collision = true;
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