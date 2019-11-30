#pragma once

#include "S2D/S2D.h"
#include "Maze.h"
#include "Direction.h"
#include "Sprite.h"
#include <random>
using namespace S2D;
//due to differences in certain parts of the AI, maybe have this be a base class and each ghost type be inherited

//video used: https://www.youtube.com/watch?v=ataGotQ7ir8

//4 behaviour states
//scatter, chase, frightened and eaten
//alternate between scatter and chase (table showing times is at 1:11 in video)
//Blinky (red) can act as in chase when in scatter if the dots remaining is below a threshold (table seen at 1:55)

//For pathfinding, can't move backwards
//Also can't move into solid tile (duh)
//Of remaining options, one closest to target is chosen
//If tiles are same distance, then the priority is up, left, down then right

//In scatter mode, each ghost targets a specific corner of the maze

//Frightened mode
//All ghosts enter frightened mode when pacman eats a power pellet on levels 1-16 & 18
//When pacman touches them in this mode, the player gains points and the ghosts go back to the ghost house
//All ghosts will also turn 180
//Instead of looking for the shortest path, they randomly choose a direction

//Eaten mode
//Ghost turns into eyeballs and heads back to the ghost house
//Done by setting target tile to directly infront of gate
//Once they reach it, they go inside and revert to scatter or chase mode

//Chase mode
//All ghosts turn 180
//Also turn when leaving chase and going into scatter
//In chase mode, target tile is calculated before choosing where to move
//Blinky (red) sets the target as the tile pacman is located
//Pinky (pink) sets the target as 4 tiles ahead of pacman (based on direction pacman is facing) but when facing up is 4 tiles up and 4 tiles to the left
//Inky (blue) intermediate tile is found which is 2 tiles ahead of pacman (if up, 2 ahead and 2 to left) then vector from this to blinkys position is rotated 180 degrees, end point is target tile
//Clyde (orange) target tile is the same as blinky if 8 or more tiles away from pacman else target is the same as in scatter mode

//Ghosts cannot turn up near ghost house and near where pacman starts (see 13:25)

//https://www.spriters-resource.com/arcade/pacman/sheet/52631/

class Enemy
{
private:
	enum ghostMode { CHASE, SCATTER, EATEN, FRIGHTENED, INHOUSE };
	enum ghostType { BLINKY, PINKY, INKY, CLYDE };

	Sprite* enemySprite;
	unsigned int totalElapsedTime = 0;

	//Data for maze
	mazeUnits(*maze)[_mazeHeight][_mazeWidth];
	float tilesetTileWidth;
	float tilesetTileHeight;

	random_device rd;
	mt19937 mt;

	//Data for ghost state
	bool turnedAroundWhenFrightened = false;
	bool ableToLeaveHouse = false;
	ghostMode currMode = CHASE;
	ghostType ghost;

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

	void CheckDirection(bool(&ableToMoveInDirections)[4], int currentX, int currentY);
	void CalculateDirection(int currentX, int currentY);
	int DistanceSquared(int x1, int x2, int y1, int y2);
	void Chase(int currentX, int currentY, int pacmanX, int pacmanY, direction pacmanDirection, Enemy* blinky);
	void Scatter(int currentX, int currentY, int pacmanX, int pacmanY);
	void Eaten(int currentX, int currentY);
	void Frightened(int currentX, int currentY);
	void InHouse(int currentX, int currentY);
	void Move(int elapsedTime);
	void ScreenWrapCheck();
	bool PacmanCollision(float ghostX, float ghostY, float pacmanX, float pacmanY);
	void Animate(int elapsedTime);
	ghostMode GetMode(unsigned int totalElapsedTime);
	void GetCurrentPosition(int& currentX, int& currentY);
	void CheckIfAtTargetTile(int currentX, int currentY);
	void ModeChangeTurnAround();
	void RunModeCode(int elapsedTime, int currentX, int currentY, int pacmanX, int pacmanY, direction pacmanDirection, Enemy* blinky, bool& inChaseOrScatterMode);

public:
	Enemy(Texture2D* texture, Vector2* position, Rect* sourceRect, mazeUnits (*mazeInput)[_mazeHeight][_mazeWidth], int ghostNumber, float backgroundElementWidth, float backgroundElementHeight);
	const Texture2D* GetTexturePointer();
	const Vector2* GetVectorPointer();
	const Rect* GetRectPointer();
	void Update(int elapsedTime, int level, direction pacmanDirection, float pacmanXPos, float pacmanYPos, Enemy* blinky, bool pacmanPoweredUp, bool& collidedWithPacman, bool& inChaseOrScatterMode);
	void GhostHasBeenEaten();
};