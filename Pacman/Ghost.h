#pragma once

#include "S2D/S2D.h"
#include "Maze.h"
#include "Direction.h"
#include "Sprite.h"
#include <random>
using namespace S2D;

enum ghostMode { CHASE, SCATTER, EATEN, FRIGHTENED, INHOUSE };

class Ghost
{
private:
	enum ghostType { BLINKY, PINKY, INKY, CLYDE };

	Sprite* enemySprite;
	unsigned int totalElapsedTime = 0;

	//Data for maze
	mazeUnits(*maze)[cMazeHeight][cMazeWidth];

	//Data for random number generation
	random_device rd;
	mt19937 mt;

	//Screenwrapping data
	float leftLimit;
	float rightLimit;

	//Data for ghost state
	bool turnedAroundWhenFrightened = false;
	bool ableToLeaveHouse = false;
	ghostMode currMode = CHASE;
	ghostType currGhost;

	//Data for pathfinding
	bool reachedNewTile = true;
	int newTileX = 0;	int newTileY = 0;
	int targetX = 0;	int targetY = 0;

	//Constants
	const int _cFrameTime;
	const float _cSpeed;
	const int _cTimeToLeaveHouse;
	const float _cEatenSpeedMultiplier;
	const float _cFrightenedSpeedMultiplier;
	const int _cHouseX;
	const int _cHouseY;
	const int _cModeShortTime;
	const int _cModeMediumTime;
	const int _cModeLongTime;

	void CheckDirection(bool(&ableToMoveInDirections)[4], int currentX, int currentY);
	void CalculateDirection(int currentX, int currentY);
	int DistanceSquared(int x1, int x2, int y1, int y2);
	void Chase(int currentX, int currentY, int pacmanX, int pacmanY, direction pacmanDirection, Ghost* blinky);
	void Scatter(int currentX, int currentY, int pacmanX, int pacmanY);
	void Eaten(int currentX, int currentY);
	void Frightened(int currentX, int currentY);
	void InHouse(int currentX, int currentY, int level);
	void Move(int elapsedTime);
	void ScreenWrapCheck();
	bool PacmanCollision(float ghostX, float ghostY, float pacmanX, float pacmanY);
	void Animate(int elapsedTime);
	ghostMode GetMode(unsigned int totalElapsedTime, int level);
	void GetCurrentPosition(int& currentX, int& currentY);
	void CheckIfAtTargetTile(int currentX, int currentY);
	void ModeChangeTurnAroundCheck(int level);
	void RunModeCode(int elapsedTime, int currentX, int currentY, int pacmanX, int pacmanY, direction pacmanDirection, Ghost* blinky, int level);

public:
	Ghost(Texture2D* texture, Vector2* position, Rect* sourceRect, mazeUnits (*mazeInput)[cMazeHeight][cMazeWidth], int ghostNumber, float leftScreenLimit, float rightScreenLimit);
	~Ghost();
	const Texture2D* GetTexturePointer();
	const Vector2* GetVectorPointer();
	const Rect* GetRectPointer();
	void Update(int elapsedTime, int level, direction pacmanDirection, float pacmanXPos, float pacmanYPos, Ghost* blinky, bool pacmanPoweredUp, bool& collidedWithPacman);
	void GhostHasBeenEaten();
	ghostMode GetMode();
};