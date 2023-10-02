#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

constexpr int DISPLAY_WIDTH{ 800 };
constexpr int DISPLAY_HEIGHT{ 600 };
constexpr int DISPLAY_SCALE{ 1 };

void Draw();


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
	STATE_GAMEOVER,
};

struct GameState
{
	float time{ 0 };
	PlayState playState = STATE_START;
	FishPodState fishState = STATE_APPEAR;
};

GameState gState;

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


}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
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

	Play::DrawObjectRotated(Play::GetGameObjectByType(TYPE_POD));
	GameObject& obj_pod = Play::GetGameObjectByType(TYPE_POD);

	Play::PresentDrawingBuffer();
}