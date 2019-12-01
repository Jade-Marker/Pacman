#include "Enemy.h"
/// <summary> Constructs the Enemy class. </summary>
Enemy::Enemy(Texture2D* textureInput, Vector2* positionInput, Rect* sourceRectInput, mazeUnits (*mazeInput)[cMazeHeight][cMazeWidth], int ghostNumber, float leftScreenLimit, float rightScreenLimit): _cSpeed(0.075f), _cFrameTime(500), _cTimeToLeaveHouse(700), _cEatenSpeedMultiplier(2.0f), _cFrightenedSpeedMultiplier(0.5f), _cHouseX(13), _cHouseY(14)
{
	enemySprite = new Sprite();

	enemySprite->texture = textureInput;
	enemySprite->position = positionInput;
	enemySprite->sourceRect = sourceRectInput;
	enemySprite->direction = NONE;
	maze = mazeInput;

	ghost = static_cast<ghostType>(ghostNumber);

	leftLimit = leftScreenLimit;
	rightLimit = rightScreenLimit;

	mt19937 initMT(rd());
	mt = initMT;

	uniform_int_distribution<int> dist(0, _cFrameTime);

	enemySprite->currentFrameTime = dist(mt);
	enemySprite->frame = dist(mt) % 2;

	if (ghost != BLINKY)
	{
		currMode = INHOUSE;
		ableToLeaveHouse = false;
	}
}

/// <summary> Destroys the Enemy class. </summary>
Enemy::~Enemy()
{
	delete enemySprite->texture;
	delete enemySprite->position;
	delete enemySprite->sourceRect;
	delete enemySprite;
}

/// <summary> Returns a constant pointer to the texture, so that it can be accessed but not modified </summary>
const Texture2D* Enemy::GetTexturePointer()
{
	return enemySprite->texture;
}

/// <summary> Returns a constant pointer to the position, so that it can be accessed but not modified </summary>
const Vector2* Enemy::GetVectorPointer()
{
	return enemySprite->position;
}

/// <summary> Returns a constant pointer to the rect, so that it can be accessed but not modified </summary>
const Rect* Enemy::GetRectPointer()
{
	return enemySprite->sourceRect;
}

void Enemy::Update(int elapsedTime, int level, direction pacmanDirection, float pacmanXPos, float pacmanYPos, Enemy* blinky, bool pacmanPoweredUp, bool& collidedWithPacman)
{
	int pacmanX = CalculateMazeX(pacmanXPos, enemySprite->sourceRect->Width, cTilesetTileWidth);
	int pacmanY = CalculateMazeY(pacmanYPos, enemySprite->sourceRect->Height, cTilesetTileHeight);

	int currentX;
	int currentY;

	GetCurrentPosition(currentX, currentY);
	CheckIfAtTargetTile(currentX, currentY);

	if (reachedNewTile) {
		if (currMode != INHOUSE) {
			if (pacmanPoweredUp && currMode != EATEN)
			{
				//if not in house, not eaten and pacman is powered up, swap to frightened mode
				currMode = FRIGHTENED;

				//turn around when entering frightened mode
				if (!turnedAroundWhenFrightened) {
					enemySprite->direction = OppositeDirection(enemySprite->direction);
					turnedAroundWhenFrightened = true;
				}
			}
			else if (currMode != EATEN)
			{
				turnedAroundWhenFrightened = false;
				ModeChangeTurnAround();
			}
		}

		RunModeCode(elapsedTime, currentX, currentY, pacmanX, pacmanY, pacmanDirection, blinky);

		//then calculate the correct direction based on target tile
		CalculateDirection(currentX, currentY);
		ableToLeaveHouse = false;
		reachedNewTile = false;
	}

	//direction is now set, so we want to move
	Move(elapsedTime);

	//Animate ghost
	Animate(elapsedTime);

	//check for screen wrapping
	ScreenWrapCheck();

	//now need to check if we've collided with pacman
	collidedWithPacman = PacmanCollision(enemySprite->position->X, enemySprite->position->Y, pacmanXPos, pacmanYPos);
}

/// <summary> Called by pacman to show that the ghost has been eaten</summary>
void Enemy::GhostHasBeenEaten()
{
	currMode = EATEN;
}

/// <summary> Returns the current mode the ghost is in</summary>
ghostMode Enemy::GetMode()
{
	return currMode;
}

/// <summary> Checks if the ghost can move in each of the 4 possible directions and sets the corresponding bool in the array</summary>
void Enemy::CheckDirection(bool(&ableToMoveInDirections)[4], int currentX, int currentY)
{
	//For ableToMoveInDirections, bools represent the following directions:
	//up, left, down, right
	//Done this way so that if the distance in each direction is equal, priority is given to up, then left, etc.

	if (currentX >= cMazeWidth)
		currentX = cMazeWidth - 1;
	if (currentX <= 0)
		currentX = 0;

	if (ableToLeaveHouse) {
		if ((*maze)[currentY - 1][currentX] != PELLET && (*maze)[currentY - 1][currentX] != EMPTY && (*maze)[currentY - 1][currentX] != GHOST_ENTRANCE)
			ableToMoveInDirections[0] = false;
		if ((*maze)[currentY][currentX - 1] != PELLET && (*maze)[currentY][currentX - 1] != EMPTY && (*maze)[currentY][currentX - 1] != GHOST_ENTRANCE)
			ableToMoveInDirections[1] = false;
		if ((*maze)[currentY + 1][currentX] != PELLET && (*maze)[currentY + 1][currentX] != EMPTY && (*maze)[currentY + 1][currentX] != GHOST_ENTRANCE)
			ableToMoveInDirections[2] = false;
		if ((*maze)[currentY][currentX + 1] != PELLET && (*maze)[currentY][currentX + 1] != EMPTY && (*maze)[currentY][currentX + 1] != GHOST_ENTRANCE)
			ableToMoveInDirections[3] = false;
	}
	else {
		if ((*maze)[currentY - 1][currentX] != PELLET && (*maze)[currentY - 1][currentX] != EMPTY)
			ableToMoveInDirections[0] = false;
		if ((*maze)[currentY][currentX - 1] != PELLET && (*maze)[currentY][currentX - 1] != EMPTY)
			ableToMoveInDirections[1] = false;
		if ((*maze)[currentY + 1][currentX] != PELLET && (*maze)[currentY + 1][currentX] != EMPTY)
			ableToMoveInDirections[2] = false;
		if ((*maze)[currentY][currentX + 1] != PELLET && (*maze)[currentY][currentX + 1] != EMPTY)
			ableToMoveInDirections[3] = false;
	}
}

/// <summary> Calculates which direction to move in </summary>
void Enemy::CalculateDirection(int currentX, int currentY)
{
	//up, left, down, right
	bool ableToMoveInDir[4] = { true,true,true,true };
	int distSquared;
	int smallestDistSquared = INT_MAX;
	direction newDirection = enemySprite->direction;

	CheckDirection(ableToMoveInDir, currentX, currentY);

	//up, left, down, right is the priority
	//ghost can't move backwards, so each internal if has enemySprite->direction != opposite direction
	//The ghosts use a greedy shortest path algorithm, so they look for the direction which gives the shortest distance
	//distance is left as squared, as if the distance is smallest squared, it is also smallest when rooted

	if (ableToMoveInDir[0])
	{
		distSquared = DistanceSquared(targetX, currentX, targetY, currentY - 1);
		if (distSquared < smallestDistSquared && enemySprite->direction != DOWN)
		{
			smallestDistSquared = distSquared;
			newDirection = UP;
			newTileX = currentX;
			newTileY = currentY - 1;
		}
	}
	if (ableToMoveInDir[1])
	{
		distSquared = DistanceSquared(targetX, currentX - 1, targetY, currentY);
		if (distSquared < smallestDistSquared && enemySprite->direction != RIGHT)
		{
			smallestDistSquared = distSquared;
			newDirection = LEFT;
			newTileX = currentX - 1;
			newTileY = currentY;
		}
	}
	if (ableToMoveInDir[2])
	{
		distSquared = DistanceSquared(targetX, currentX, targetY, currentY + 1);
		if (distSquared < smallestDistSquared && enemySprite->direction != UP)
		{
			smallestDistSquared = distSquared;
			newDirection = DOWN;
			newTileX = currentX;
			newTileY = currentY + 1;
		}
	}
	if (ableToMoveInDir[3])
	{
		distSquared = DistanceSquared(targetX, currentX + 1, targetY, currentY);
		if (distSquared < smallestDistSquared && enemySprite->direction != LEFT)
		{
			smallestDistSquared = distSquared;
			newDirection = RIGHT;
			newTileX = currentX + 1;
			newTileY = currentY;
		}
	}
	enemySprite->direction = newDirection;
}

/// <summary> Calculates the squared distance between two points</summary>
int Enemy::DistanceSquared(int x1, int x2, int y1, int y2)
{
	return ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
}

/// <summary> Sets the target tile for the ghost. As it is chase, the ghosts go for some tile relative to pacman </summary>
void Enemy::Chase(int currentX, int currentY, int pacmanX, int pacmanY, direction pacmanDirection, Enemy* blinky)
{
	int distSquared;
	int intermediateX, intermediateY;
	int blinkyX, blinkyY;
	int vectorX, vectorY;

	//blinky x&y calculated as the target tile for Inky is set based on blinkys target tile
	blinkyX = CalculateMazeX(blinky->GetVectorPointer()->X, enemySprite->sourceRect->Width, cTilesetTileWidth);
	blinkyY = CalculateMazeY(blinky->GetVectorPointer()->Y, enemySprite->sourceRect->Width, cTilesetTileHeight);

	//set target tile based on type of ghost
	switch (ghost)
	{
	case BLINKY:
		//Blinky (red) sets the target as the tile pacman is located
		targetX = pacmanX;
		targetY = pacmanY;
		break;

	case PINKY:
		//Pinky (pink) sets the target as 4 tiles ahead of pacman (based on direction pacman is facing) but when facing up is 4 tiles up and 4 tiles to the left
		switch (pacmanDirection)
		{
		case UP:
			targetX = pacmanX - 4;
			targetY = pacmanY - 4;
			break;
		case DOWN:
			targetX = pacmanX;
			targetY = pacmanY + 4;
			break;
		case LEFT:
			targetX = pacmanX - 4;
			targetY = pacmanY;
			break;
		case RIGHT:
			targetX = pacmanX + 4;
			targetY = pacmanY;
			break;
		}
		break;

	case INKY:
		//Inky (blue) intermediate tile is found which is 2 tiles ahead of pacman (if up, 2 ahead and 2 to left)
		//then vector from this to blinkys position is rotated 180 degrees, end point is target tile
		switch (pacmanDirection)
		{
		case UP:
			intermediateX = pacmanX - 2;
			intermediateY = pacmanY - 2;
			break;
		case DOWN:
			intermediateX = pacmanX;
			intermediateY = pacmanY + 2;
			break;
		case LEFT:
			intermediateX = pacmanX - 2;
			intermediateY = pacmanY;
			break;
		case RIGHT:
			intermediateX = pacmanX + 2;
			intermediateY = pacmanY;
			break;
		default:
			intermediateX = 0;
			intermediateY = 0;
		}

		vectorX = blinkyX - intermediateX;
		vectorY = blinkyY - intermediateY;

		//for rotating 180 degrees, x' = -x and y' = -y
		//so, targetX = origin (intermediate) x - vectorX this follows for Y also
		targetX = intermediateX - vectorX;
		targetY = intermediateY - vectorY;
		break;

	case CLYDE:
		//Clyde (orange) target tile is the same as blinky if 8 or more tiles away from pacman else target is the same as in scatter mode
		//so, we need to see if the distance squared from the current tile to pacmans tile is < 8 squared
		distSquared = DistanceSquared(currentX, pacmanX, currentY, pacmanY);
		if (distSquared <= 8 * 8)
		{
			targetX = 0;
			targetY = cMazeHeight;
		}
		else 
		{
			targetX = pacmanX;
			targetY = pacmanY;
		}
		break;
	};

	//keep targetX and targetY inside of the maze
	if (targetX < 0)
		targetX = 0;
	if (targetX >= cMazeWidth)
		targetX = cMazeWidth - 1;

	if (targetY < 0)
		targetY = 0;
	if (targetY >= cMazeHeight)
		targetY = cMazeHeight - 1;
}

/// <summary> Sets the target tile for the ghost. As it is scatter, the target tile is a specific corner of the maze</summary>
void Enemy::Scatter(int currentX, int currentY, int pacmanX, int pacmanY)
{
	//in scatter, each ghost sets it target tile to a specific corner

	switch (ghost)
	{
	case BLINKY:
		targetX = cMazeWidth - 2;
		targetY = 0;
		break;

	case PINKY:
		targetX = 1;
		targetY = 0;
		break;

	case INKY:
		targetX = cMazeWidth - 1;
		targetY = cMazeHeight;
		break;

	case CLYDE:
		targetX = 0;
		targetY = cMazeHeight;
		break;
	};
}

/// <summary> Sets the target tile for the ghost. As it is eaten, target tile is inside the house</summary>
void Enemy::Eaten(int currentX, int currentY)
{
	//set target tile to inside the house, and set ableToLeaveHouse to true so that the ghost can get into the house
	targetX = _cHouseX;
	targetY = _cHouseY;
	ableToLeaveHouse = true;

	if (currentX == targetX && currentY == targetY)
	{
		currMode = INHOUSE;
	}
}

/// <summary> Sets the target tile for the ghost. As it is frightened, the target tile is a random valid adjecent tile</summary>
void Enemy::Frightened(int currentX, int currentY)
{
	//up, left, down, right
	//Done this way so that if the distance in each direction is equal, priority is given to up, then left, etc.
	bool ableToMoveInDir[4] = { true,true,true,true };
	CheckDirection(ableToMoveInDir, currentX, currentY);

	bool directionChosen = false;
	uniform_int_distribution<int> dist(0, 3);
	int randomIndex;

	while (!directionChosen)
	{
		randomIndex = dist(mt);
		if (ableToMoveInDir[randomIndex])
			directionChosen = true;
	}

	switch (randomIndex)
	{
	case 0:
		targetX = currentX;
		targetY = currentY - 1;
		break;
	case 1:
		targetX = currentX - 1;
		targetY = currentY;
		break;
	case 2:
		targetX = currentX;
		targetY = currentY + 1;
		break;
	case 3:
		targetX = currentX + 1;
		targetY = currentY;
		break;
	}

}

/// <summary> Sets the target tile for the ghost. As it is inhouse, the target tile is outside of the house so the ghost will try and leave</summary>
void Enemy::InHouse(int currentX, int currentY)
{
	//set target tile to outside of the house, so the ghost will try and leave
	targetX = _cHouseX;
	targetY = _cHouseY - 3;

	if (totalElapsedTime >= _cTimeToLeaveHouse * static_cast<int>(ghost)) {
		ableToLeaveHouse = true;

		if (currentX == targetX && currentY == targetY)
			currMode = GetMode(totalElapsedTime);
	}
}

/// <summary> Moves the ghost</summary>
void Enemy::Move(int elapsedTime)
{
	float moveAmount = 0;

	switch (currMode)
	{
	case EATEN:
		moveAmount = _cEatenSpeedMultiplier * _cSpeed * elapsedTime;
		break;

	case FRIGHTENED:
		moveAmount = _cFrightenedSpeedMultiplier * _cSpeed * elapsedTime;
		break;

	default:
		moveAmount = _cSpeed * elapsedTime;
		break;
	}

	switch (enemySprite->direction)
	{
	case UP:
		enemySprite->position->Y -= moveAmount;
		break;

	case DOWN:
		enemySprite->position->Y += moveAmount;
		break;

	case LEFT:
		enemySprite->position->X -= moveAmount;
		break;

	case RIGHT:
		enemySprite->position->X += moveAmount;
		break;
	}
}

/// <summary> Checks if the ghost has gone off the screen and wraps them back around if they have</summary>
void Enemy::ScreenWrapCheck()
{
	if (enemySprite->position->X < leftLimit)
	{
		enemySprite->position->X += (cMazeWidth + 1) * cTilesetTileWidth;
		newTileX = cMazeWidth - 1;
	}

	if (enemySprite->position->X > rightLimit)
	{
		enemySprite->position->X -= (cMazeWidth + 2) * cTilesetTileWidth;
		newTileX = 0;
	}
}

/// <summary> Checks if the ghost has collided with pacman</summary>
bool Enemy::PacmanCollision(float ghostX, float ghostY, float pacmanX, float pacmanY)
{
	float collisionCentreX = ghostX + enemySprite->sourceRect->Width / 2.0f;
	float collisionCentreY = ghostY + enemySprite->sourceRect->Height / 2.0f;

	//each offset is 1/4 of the width or height so that the centre +- the offset is the coordinate of an edge
	float xOffset = enemySprite->sourceRect->Width / 4.0f;
	float yOffset = enemySprite->sourceRect->Height / 4.0f;

	return ((collisionCentreX - xOffset < pacmanX + enemySprite->sourceRect->Width) && (collisionCentreX + xOffset > pacmanX) && (collisionCentreY - yOffset < pacmanY + enemySprite->sourceRect->Height) && (collisionCentreY + yOffset > pacmanY));
}

/// <summary> Animates the ghost</summary>
void Enemy::Animate(int elapsedTime)
{
	enemySprite->Animate(elapsedTime, _cFrameTime);

	switch (currMode)
	{
	case CHASE:
	case SCATTER:
	case INHOUSE:
		enemySprite->sourceRect->X = enemySprite->sourceRect->Width * enemySprite->direction;
		break;

	case FRIGHTENED:
		enemySprite->sourceRect->X = enemySprite->sourceRect->Width * 8;
		break;

	case EATEN:
		enemySprite->sourceRect->X = enemySprite->sourceRect->Width * 4 + enemySprite->sourceRect->Width * enemySprite->direction;
		break;
	}
	enemySprite->sourceRect->Y = enemySprite->sourceRect->Width * enemySprite->frame;
}

/// <summary> Returns the mode the ghost should be in based on the time</summary>
ghostMode Enemy::GetMode(unsigned int totalElapsedTime)
{
	//imitates what mode the ghost should be in based on: https://gameinternals.com/understanding-pac-man-ghost-behavior
	//Only imitates for level 1
	
	if (totalElapsedTime <= 7 * 100)
		return SCATTER;
	else if (totalElapsedTime <= 7 * 100 + 20 * 100)
		return CHASE;
	else if (totalElapsedTime <= 7 * 100 + 20 * 100 + 7 * 100)
		return SCATTER;
	else if (totalElapsedTime <= 7 * 100 + 20 * 100 + 7 * 100 + 20 * 100)
		return CHASE;
	else if (totalElapsedTime <= 7 * 100 + 20 * 100 + 7 * 100 + 20 * 100 + 5 * 100)
		return SCATTER;
	else if (totalElapsedTime <= 7 * 100 + 20 * 100 + 7 * 100 + 20 * 100 + 5 * 100 + 20 * 100)
		return CHASE;
	else if (totalElapsedTime <= 7 * 100 + 20 * 100 + 7 * 100 + 20 * 100 + 5 * 100 + 20 * 100 + 5 * 100)
		return SCATTER;
	else
		return CHASE;
}

/// <summary> Returns the position the ghost has in the maze</summary>
void Enemy::GetCurrentPosition(int& currentX, int& currentY)
{
	switch (enemySprite->direction)
	{
	case UP:
		currentX = CalculateMazeX(enemySprite->position->X, enemySprite->sourceRect->Width, cTilesetTileWidth);
		currentY = CalculateMazeY(enemySprite->position->Y + enemySprite->sourceRect->Height / 4.0f, enemySprite->sourceRect->Height, cTilesetTileHeight);
		//adds offset so that the position is the centre

		if (currentX == newTileX && currentY <= newTileY)
			reachedNewTile = true;
		break;

	case DOWN:
		currentX = CalculateMazeX(enemySprite->position->X, enemySprite->sourceRect->Width, cTilesetTileWidth);
		currentY = CalculateMazeY(enemySprite->position->Y - enemySprite->sourceRect->Height / 4.0f, enemySprite->sourceRect->Height, cTilesetTileHeight);
		//subtracts offset so that the position is the centre

		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX == newTileX && currentY >= newTileY)
			reachedNewTile = true;
		break;
	case LEFT:
		currentX = CalculateMazeX(enemySprite->position->X + enemySprite->sourceRect->Width / 4.0f, enemySprite->sourceRect->Width, cTilesetTileWidth);
		//adds offset so that the position is the centre
		currentY = CalculateMazeY(enemySprite->position->Y, enemySprite->sourceRect->Height, cTilesetTileHeight);

		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX <= newTileX && currentY == newTileY)
			reachedNewTile = true;
		break;
	case RIGHT:
		currentX = CalculateMazeX(enemySprite->position->X - enemySprite->sourceRect->Width / 4.0f, enemySprite->sourceRect->Width, cTilesetTileWidth);
		//subtracts offset so that the position is the centre
		currentY = CalculateMazeY(enemySprite->position->Y, enemySprite->sourceRect->Height, cTilesetTileHeight);

		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX >= newTileX && currentY == newTileY)
			reachedNewTile = true;
		break;
	default:
		currentX = CalculateMazeX(enemySprite->position->X, enemySprite->sourceRect->Width, cTilesetTileWidth);
		currentY = CalculateMazeY(enemySprite->position->Y, enemySprite->sourceRect->Height, cTilesetTileHeight);
		break;
	}
}

/// <summary> Checks if the ghost has reached or gone past the target tile</summary>
void Enemy::CheckIfAtTargetTile(int currentX, int currentY)
{
	//if we have moved past the target tile, just consider it reached so that we don't get stuck
	switch (enemySprite->direction)
	{
	case UP:
		if (currentX == newTileX && currentY <= newTileY)
			reachedNewTile = true;
		break;

	case DOWN:
		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX == newTileX && currentY >= newTileY)
			reachedNewTile = true;
		break;
	case LEFT:
		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX <= newTileX && currentY == newTileY)
			reachedNewTile = true;
		break;
	case RIGHT:
		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX >= newTileX && currentY == newTileY)
			reachedNewTile = true;
		break;
	}

	if (currentX == newTileX && currentY == newTileY)
		reachedNewTile = true;
}

/// <summary> Checks if there has been a mode change and turns around if there has</summary>
void Enemy::ModeChangeTurnAround()
{
	ghostMode oldMode = currMode;
	currMode = GetMode(totalElapsedTime);

	//if there has been a mode change, turn around
	if (oldMode != currMode && enemySprite->direction != NONE)
	{
		enemySprite->direction = OppositeDirection(enemySprite->direction);
	}
}

/// <summary> Runs the function for the current mode</summary>
void Enemy::RunModeCode(int elapsedTime, int currentX, int currentY, int pacmanX, int pacmanY, direction pacmanDirection, Enemy* blinky)
{
	switch (currMode)
	{
	case CHASE:
		totalElapsedTime += elapsedTime;
		Chase(currentX, currentY, pacmanX, pacmanY, pacmanDirection, blinky);
		break;

	case SCATTER:
		totalElapsedTime += elapsedTime;
		Scatter(currentX, currentY, pacmanX, pacmanY);
		break;

	case EATEN:
		totalElapsedTime += elapsedTime;
		Eaten(currentX, currentY);
		break;

	case FRIGHTENED:
		//Don't add to totalElapsedTime here as totalElapsedTime is used to tell which ghost mode we should be in, and it isn't supposed to increment in frightened mode
		Frightened(currentX, currentY);
		break;

	case INHOUSE:
		totalElapsedTime += elapsedTime;
		InHouse(currentX, currentY);
		break;
	}
}