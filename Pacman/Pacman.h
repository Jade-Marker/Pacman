#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"
#include "Maze.h"
#include "Direction.h"
#include "Enemy.h"
#include "Player.h"
#include "Menu.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
const int _enemyCount = 4;

class Pacman : public Game
{
private:
	Enemy* _enemies[_enemyCount];

	Player* _pacman;

	//Pacman constants
	const float _cPacmanSpeed;
	const float _cPacmanPosOffset;
	const int _cPacmanFrameTime;

	//Score constants
	const int _cPelletValue;
	const int _cPowerPelletValue;
	const int _cEnemyValue;
	const int _cCherryValue;

	//Time constants
	const int _cLevelEndDelay;
	const int _cLevelStartDelay;
	const int _cPoweredUpTime;
	const int _cDeathDelay;

	//Data for pellet animation
	int _currentPelletFrameTime;
	int _pelletFrame;
	const int _cPelletFrameTime;

	//Data for cherry
	bool _cherryGivenToPlayer;
	int _currentCherryFrameTime;
	int _cherryFrame;
	const int _cCherryX;
	const int _cCherryY;
	const float _cProportionOfPelletsRequired;
	const int _cCherryFrameTime;

	//Screenswrapping data
	float leftLimit;
	float rightLimit;

	//Level data
	int _pelletsCollected;
	int _level;
	bool _levelEnd;

	//Delay data
	int _delayInMilli;
	bool _delay;

	//Powered up data
	bool _poweredUp;
	int _powerTimer;

	Menu* _pauseMenu;
	Menu* _startMenu;
	Menu* _gameOverMenu;

	//Data for overlay
	Texture2D* _overlay;
	Rect* _overlayRect;

	//Data for maze
	Texture2D* _mazeTileset;
	Rect* _mazeTileRect;
	Vector2* _backgroundPos;
	int _noPelletsAvailable;
	mazeUnits _maze[cMazeHeight][cMazeWidth];
	Color* _backgroundColor;
	Vector2* _backgroundColorVector;

	//Data for sound/music
	SoundEffect* _intro;
	SoundEffect* _munch;

	//Functions
	void Input(int elapsedTime, Input::KeyboardState* state);
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckStart(Input::KeyboardState* state, Input::Keys startKey);
	void CheckGameOver(Input::KeyboardState* state, Input::Keys restartKey);
	void UpdatePacman(int elapsedTime, int frameTime);
	bool CollisionCheck(float pacmanX, float pacmanY, direction directionOfMovement);
	void LoadMaze(mazeUnits (&maze)[cMazeHeight][cMazeWidth], const mazeUnits(&mazeToCopy)[cMazeHeight][cMazeWidth]);
	int GetNoOfPellets(mazeUnits(&mazeToCheck)[cMazeHeight][cMazeWidth]);
	void PelletCollisionCheck();
	void CherryGiveCheck();
	void ScreenWrapCheck();
	void LevelWinCheck();
	void ResetLevel();
	void ResetMaze();
	void DelayCountdown(int elapsedTime);
	void CreateAndInitGhosts();
	void PacmanDeath();
	void UpdateGhostAndCheckCollisions(int elapsedTime);
	void HandlePowerTimer(int elapsedTime);
	void UpdatePelletAndCherry(int elapsedTime);
	void SetInitialPacmanPosition();

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};