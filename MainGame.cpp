#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <algorithm>
#include <iostream>


constexpr int DISPLAY_WIDTH{ 800 };
constexpr int DISPLAY_HEIGHT{ 600 };
constexpr int DISPLAY_SCALE{ 1 };

const Point2f POD_START_POS = { DISPLAY_WIDTH / 2, 500 };
const int facingLeft = 1;
const int facingRight = 2;
int direction = facingLeft;

//const Vector2D PANSY_AABB{ 48.f, 48.f };
//const Vector2D GOLD_AABB{ 48.f, 48.f };

void Draw();
void UpdateFishPod();
void FishPodGroundControls();
void FishPodAirControls();
void FishPodStand();
void RockPlatformCollision();
void WallCollision();
void PansyCollision();
void GoldCollision();


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
	TYPE_PLATFORM,
	TYPE_GOLD,
	TYPE_PANSY,
};


struct Tile
{
	const int TILE_HEIGHT = 32;
	const int TILE_WIDTH = 32;
	int tileSpacingWidth = 32;
	int tileSpacingHeight = 32;
	const Vector2D TILE_AABB{ 32.f, 32.f };
};

Tile obj_tile;


struct FishPod
{
	const Vector2D POD_AABB{ 10.f, 20.f };
	Point2f podPos{ POD_START_POS };
};

FishPod fishpod;


struct GameState
{
	float time{ 0 };
	PlayState playState = STATE_START;
	FishPodState fishState = STATE_STAND;
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
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
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
				Play::CreateGameObject(TYPE_PLATFORM, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 20, "rock_begin");
				break;

			case 2:
				Play::CreateGameObject(TYPE_PLATFORM, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 20, "rock_middle");
				break;

			case 3:
				Play::CreateGameObject(TYPE_PLATFORM, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 20, "rock_end");
				break;

			case 4:
				Play::CreateGameObject(TYPE_PLATFORM, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 20, "lava_begin");
				break;

			case 5:
				Play::CreateGameObject(TYPE_PLATFORM, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 20, "lava_middle");
				break;

			case 6:
				Play::CreateGameObject(TYPE_PLATFORM, { (x * obj_tile.TILE_WIDTH) + 16, (y * obj_tile.TILE_HEIGHT) + 16 }, 20, "lava_end");
				break;

			default:

				break;
			}
		}
	}

	Play::CreateGameObject(TYPE_POD, { fishpod.podPos }, 20, "pod_stand_right");
	Play::CentreAllSpriteOrigins();
	Play::MoveSpriteOrigin("pod_stand_right", 0, 5);
	Play::MoveSpriteOrigin("pod_stand_left", 0, 5);
	Play::MoveSpriteOrigin("pod_walk_left", 0, 5);
	Play::MoveSpriteOrigin("pod_walk_right", 0, 5);

}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	gameState.time += elapsedTime;
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);


	switch (gameState.playState)
	{
		case STATE_START:
			if (Play::KeyPressed('P') == true)
			{
				obj_pod.pos = { POD_START_POS };
				gameState.playState = STATE_PLAY;
				gameState.fishState = STATE_MOVE;
				Play::StartAudioLoop("thunder");
			}

			break;

		case STATE_PLAY:
			UpdateFishPod();
			RockPlatformCollision();
			PansyCollision();
			GoldCollision();

			break;

		case STATE_PAUSE:

			break;

		case STATE_WIN:

			break;

		case STATE_GAMEOVER:
			UpdateFishPod();

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
	std::vector<int> platform_id = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int i : platform_id)
	{
		GameObject& platform = Play::GetGameObject(i);
		Play::DrawObject(platform);
	}

	Play::DrawObject(Play::GetGameObjectByType(TYPE_POD));
	GameObject& FishPod{ Play::GetGameObjectByType(TYPE_POD) };

	//Play::DrawRect(fishpod.podPos - fishpod.POD_AABB, fishpod.podPos + fishpod.POD_AABB, Play::cWhite); // bounding box visual
	


	if (gameState.playState == STATE_START)
	{
		Play::DrawFontText("64px", "COLLECT THE GOLD AND AVOID THE PANSIES",
			{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
		Play::DrawFontText("64px", "PRESS P TO PLAY",
			{ DISPLAY_WIDTH / 2,  50 }, Play::CENTRE);
	}

	if (gameState.playState == STATE_PLAY)
	{
		Play::DrawFontText("64px", "LEFT AND RIGHT TO MOVE, SPACE TO JUMP",
			{ DISPLAY_WIDTH / 2,  50 }, Play::CENTRE);
	}

	if (gameState.playState == STATE_WIN)
	{
		Play::DrawFontText("64px", "YOU WON! PLAY AGAIN? PRESS P TO PLAY AGAIN", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
	}

	if (gameState.playState == STATE_GAMEOVER)
	{
		Play::DrawFontText("105px", "GAME OVER :(", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
		Play::DrawFontText("64px", "PLAY AGAIN? PRESS R TO RESTART", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50 }, Play::CENTRE);
	}


	Play::PresentDrawingBuffer();
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
			FishPodGroundControls();
			WallCollision();

			break;

		case STATE_JUMP:
			FishPodAirControls();
			WallCollision();

			break;

		case STATE_FALL:

			break;

		case STATE_DEAD:

			gameState.playState = STATE_GAMEOVER;
			Play::DestroyGameObjectsByType(TYPE_POD);

			break;
	}

	Play::UpdateGameObject(obj_pod);
}

void FishPodGroundControls()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);
	
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

	/*if (Play::IsAnimationComplete(obj_pod))
	{
		Play::PlayAudio("walk");
	}*/


	if (Play::KeyDown(VK_SPACE))
	{
		gameState.fishState = STATE_JUMP;
	}

	if (!Play::KeyDown(VK_LEFT) && !Play::KeyDown(VK_RIGHT))
	{
		gameState.fishState = STATE_STAND;
	}
	 
	
}

void FishPodAirControls()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);

	if (Play::KeyDown(VK_LEFT))
	{
		obj_pod.pos.x -= 3;
		Play::SetSprite(obj_pod, "pod_jump_left", 0.25f);
	}

	else if (Play::KeyDown(VK_RIGHT))
	{
		obj_pod.pos.x += 3;
		Play::SetSprite(obj_pod, "pod_jump_right", 0.25f);

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
		Play::SetSprite(obj_pod, "pod_stand_left", 0.25f);
	}
	if (direction == facingRight)
	{
		Play::SetSprite(obj_pod, "pod_stand_right", 0.25f);
	}

	if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT))
	{
		gameState.fishState = STATE_MOVE;
	}
}

void RockPlatformCollision()
{
	GameObject& obj_pod(Play::GetGameObjectByType(TYPE_POD));
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);

	for (int i : vPlatforms)
	{
		GameObject& obj_platform = Play::GetGameObject(i);

		bool Collision = false;

		if (obj_pod.pos.y + fishpod.POD_AABB.y > obj_platform.pos.y - obj_tile.TILE_AABB.y
			&& obj_pod.pos.y - fishpod.POD_AABB.y < obj_platform.pos.y + obj_tile.TILE_AABB.y)
		{
			if (obj_pod.pos.x + fishpod.POD_AABB.x > obj_platform.pos.x - obj_tile.TILE_AABB.x
				&& obj_pod.pos.x - fishpod.POD_AABB.x < obj_platform.pos.x + obj_tile.TILE_AABB.y)
			{
				if (obj_pod.oldPos.x < obj_platform.pos.x - obj_tile.TILE_AABB.x || obj_pod.oldPos.x > obj_platform.pos.x + obj_tile.TILE_AABB.x)
				{

				}

				else
				{

				}

				Collision = true;
			}

			//Play::UpdateGameObject(obj_pod);
		}
	}
	
}

void PansyCollision()
{

}

void GoldCollision()
{

}

void WallCollision()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);
	
	if (obj_pod.pos.x < 0 || obj_pod.pos.x > DISPLAY_WIDTH)
	{
		obj_pod.pos.x = std::clamp(obj_pod.pos.x, 0.f, (float) DISPLAY_WIDTH);
		gameState.fishState = STATE_DEAD;
	}

	if (obj_pod.pos.y < 0 || obj_pod.pos.y > DISPLAY_HEIGHT)
	{
		obj_pod.pos.y = std::clamp(obj_pod.pos.y, 0.f, (float) DISPLAY_HEIGHT);
		gameState.fishState = STATE_DEAD;

	}
}