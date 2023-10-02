#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

constexpr int DISPLAY_WIDTH{ 800 };
constexpr int DISPLAY_HEIGHT{ 600 };
constexpr int DISPLAY_SCALE{ 1 };

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
	Play::CreateGameObject(TYPE_POD, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, 20, "pod_stand_right");
	Play::CentreAllSpriteOrigins();
	Play::MoveSpriteOrigin("pod_stand_right", 0, 10);
	Play::MoveSpriteOrigin("pod_stand_left", 0, 10);
	Play::MoveSpriteOrigin("pod_walk_left", 0, 10);
	Play::MoveSpriteOrigin("pod_walk_right", 0, 10);

	
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
			WallCollision();
			PansyCollision();
			GoldCollision();

			break;

		case STATE_PAUSE:

			break;

		case STATE_WIN:

			break;

		case STATE_GAMEOVER:

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

	Play::PresentDrawingBuffer();
}

void UpdateFishPod()
{
	switch (gameState.fishState)
	{
		case STATE_APPEAR:

			break;

		case STATE_MOVE:
			FishPodGroundControls();
			break;

		case STATE_JUMP:
			FishPodAirControls();
			break;

		case STATE_DEAD:

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
		
		//gameState.fishState = STATE_JUMP;
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

void WallCollision()
{
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);

	if (Play::IsLeavingDisplayArea(obj_pod))
	{
		Play::DestroyGameObject(TYPE_POD);
		gameState.playState = STATE_GAMEOVER;
	}

}

void PansyCollision()
{

}

void GoldCollision()
{

}