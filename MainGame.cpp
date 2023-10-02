#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <algorithm>
#include <iostream>


constexpr int DISPLAY_WIDTH{ 800 };
constexpr int DISPLAY_HEIGHT{ 600 };
constexpr int DISPLAY_SCALE{ 1 };

const Vector2D FISHPOD_AABB{ 10.f, 20.f };
//const Vector2D PLATFORM_AABB{ 48.f, 48.f };
//const Vector2D PANSY_AABB{ 48.f, 48.f };
//const Vector2D GOLD_AABB{ 48.f, 48.f };

void Draw();
void UpdateFishPod();
void FishPodGroundControls();
void FishPodAirControls();
void PlatformCollision();
void WallCollision();
void PansyCollision();
void GoldCollision();
void CreatePlatforms();


enum FishPodState
{
	STATE_APPEAR,
	STATE_MOVE,
	STATE_JUMP,
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

struct GameState
{
	float time{ 0 };
	PlayState playState = STATE_START;
	FishPodState fishState = STATE_APPEAR;
};

GameState gameState;

enum GameObjectType
{
	TYPE_POD,
	TYPE_GOLD,
	TYPE_PANSY,
	TYPE_ROCK,
	TYPE_LAVA,
};

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::CreateGameObject(TYPE_POD, { DISPLAY_WIDTH / 2, 500 }, 20, "pod_stand_right");
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

	switch (gameState.playState)
	{
		case STATE_START:
			
			if (Play::KeyPressed('P') == true)
			{
				gameState.playState = STATE_PLAY;
				gameState.fishState = STATE_MOVE;
			}

			break;

		case STATE_PLAY:
			UpdateFishPod();
			CreatePlatforms();
			PlatformCollision();
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

	Play::DrawObject(Play::GetGameObjectByType(TYPE_POD));
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);

	Play::DrawRect(obj_pod.pos - FISHPOD_AABB, obj_pod.pos + FISHPOD_AABB, Play::cWhite); // bounding box visual

	if (gameState.playState == STATE_START)
	{
		Play::DrawFontText("64px", "COLLECT THE GOLD AND AVOID THE PANSIES",
			{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50 }, Play::CENTRE);
		Play::DrawFontText("64px", "PRESS P TO PLAY",
			{ DISPLAY_WIDTH / 2,  50 }, Play::CENTRE);
	}

	if (gameState.playState == STATE_PLAY)
	{
		Play::DrawFontText("64px", "LEFT AND RIGHT TO MOVE, SPACE TO JUMP",
			{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50 }, Play::CENTRE);
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
		case STATE_APPEAR:

			break;

		case STATE_MOVE:
			FishPodGroundControls();
			WallCollision();

			break;

		case STATE_JUMP:
			FishPodAirControls();
			WallCollision();

			break;

		case STATE_DEAD:

			gameState.playState = STATE_GAMEOVER;
			Play::DestroyGameObjectsByType(TYPE_POD);

			break;
	}
}

void FishPodGroundControls()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);
	
	if (Play::KeyDown(VK_LEFT))
	{
		obj_pod.pos.x -= 3;
		Play::SetSprite(obj_pod, "pod_walk_left", 0.25f);
	}

	else if (Play::KeyDown(VK_RIGHT))
	{
		obj_pod.pos.x += 3;
		Play::SetSprite(obj_pod, "pod_walk_right", 0.25f);

	}

	else
	{
		Play::SetSprite(obj_pod, "pod_stand_right", 0);
		obj_pod.animSpeed = 0.f;
	}


	if (Play::KeyDown(VK_SPACE))
	{
		
		gameState.fishState = STATE_JUMP;
	}
	 

}

void FishPodAirControls()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);

	if (Play::KeyDown(VK_LEFT))
	{
		obj_pod.pos.x -= 3; 
		Play::SetSprite(obj_pod, "pod_jump_left", 0);
		obj_pod.animSpeed = 0.25f;
	}

	else if (Play::KeyDown(VK_RIGHT))
	{
		obj_pod.pos.x += 3;
		Play::SetSprite(obj_pod, "pod_jump_right", 0);
		obj_pod.animSpeed = 0.25f;

	}



}


void CreatePlatforms()
{

}

void PlatformCollision()
{

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