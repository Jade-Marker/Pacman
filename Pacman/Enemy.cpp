#include "Enemy.h"
/// <summary> Constructs the Enemy class. </summary>
Enemy::Enemy(Texture2D* textureInput, Vector2* positionInput, Rect* sourceRectInput, mazeUnits (*mazeInput)[_mazeHeight][_mazeWidth], int ghostNumber, float backgroundElementWidth, float backgroundElementHeight)
{
	texture = textureInput;
	position = positionInput;
	sourceRect = sourceRectInput;
	maze = mazeInput;

	ghost = static_cast<ghostType>(ghostNumber);

	backgroundTileHeight = backgroundElementHeight;
	backgroundTileWidth = backgroundElementWidth;

	mt19937 initMT(rd());
	mt = initMT;

	uniform_int_distribution<int> dist(0, FRAMETIME);

	currentFrameTime = dist(mt);
	frame = dist(mt) % 2;

	if (ghost != BLINKY)
	{
		currMode = INHOUSE;
		ableToLeaveHouse = false;
	}
}

/// <summary> Returns a constant pointer to the texture, so that it can be accessed but not modified </summary>
const Texture2D* Enemy::GetTexturePointer()
{
	return texture;
}

/// <summary> Returns a constant pointer to the position, so that it can be accessed but not modified </summary>
const Vector2* Enemy::GetVectorPointer()
{
	return position;
}

/// <summary> Returns a constant pointer to the rect, so that it can be accessed but not modified </summary>
const Rect* Enemy::GetRectPointer()
{
	return sourceRect;
}

void Enemy::Update(int elapsedTime, int level, direction pacmanDirection, float pacmanXPos, float pacmanYPos, Enemy* blinky, bool pacmanPoweredUp, bool& collidedWithPacman, bool& inChaseOrScatterMode)
{
	int pacmanX = CalculateMazeX(pacmanXPos, sourceRect->Width, backgroundTileWidth);
	int pacmanY = CalculateMazeY(pacmanYPos, sourceRect->Height, backgroundTileHeight);

	int currentX;
	int currentY;

	switch (currDirection)
	{
	case UP:
		currentX = CalculateMazeX(position->X, sourceRect->Width, backgroundTileWidth);
		currentY = CalculateMazeY(position->Y + sourceRect->Height / 4.0f, sourceRect->Height, backgroundTileHeight);
		//adds offset so that the position is the centre

		if (currentX == newTileX && currentY <= newTileY)
			reachedNewTile = true;
		break;

	case DOWN:
		currentX = CalculateMazeX(position->X, sourceRect->Width, backgroundTileWidth);
		currentY = CalculateMazeY(position->Y - sourceRect->Height / 4.0f, sourceRect->Height, backgroundTileHeight);
		//subtracts offset so that the position is the centre

		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX == newTileX && currentY >= newTileY)
			reachedNewTile = true;
		break;
	case LEFT:
		currentX = CalculateMazeX(position->X + sourceRect->Width / 4.0f, sourceRect->Width, backgroundTileWidth);
		//adds offset so that the position is the centre
		currentY = CalculateMazeY(position->Y, sourceRect->Height, backgroundTileHeight);

		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX <= newTileX && currentY == newTileY)
			reachedNewTile = true;
		break;
	case RIGHT:
		currentX = CalculateMazeX(position->X - sourceRect->Width / 4.0f, sourceRect->Width, backgroundTileWidth);
		//subtracts offset so that the position is the centre
		currentY = CalculateMazeY(position->Y, sourceRect->Height, backgroundTileHeight);

		//if we have moved past the target tile, just consider it reached so that we don't get stuck
		if (currentX >= newTileX && currentY == newTileY)
			reachedNewTile = true;
		break;
	default:
		currentX = CalculateMazeX(position->X, sourceRect->Width, backgroundTileWidth);
		currentY = CalculateMazeY(position->Y, sourceRect->Height, backgroundTileHeight);
		break;
	}

	if (currentX == newTileX && currentY == newTileY)
		reachedNewTile = true;


	inChaseOrScatterMode = (currMode == CHASE || currMode == SCATTER);

	if (reachedNewTile) {
		if (currMode != INHOUSE) {
			if (pacmanPoweredUp && currMode != EATEN)
			{
				//if not in house, not eaten and pacman is powered up, swap to frightened mode
				currMode = FRIGHTENED;

				//turn around when entering frightened mode
				if (!turnedAroundWhenFrightened) {
					currDirection = OppositeDirection(currDirection);
					turnedAroundWhenFrightened = true;
				}
			}
			else if (currMode != EATEN)
			{
				turnedAroundWhenFrightened = false;
				ghostMode oldMode = currMode;
				currMode = GetMode(totalElapsedTime);

				//if there has been a mode change, turn around
				if (oldMode != currMode && currDirection != NONE)
				{
					currDirection = OppositeDirection(currDirection);
				}
			}
		}

		switch (currMode)
		{
		case CHASE:
			totalElapsedTime += elapsedTime;
			Chase(currentX, currentY, pacmanX, pacmanY, pacmanDirection, blinky);
			inChaseOrScatterMode = true;
			break;

		case SCATTER:
			totalElapsedTime += elapsedTime;
			Scatter(currentX, currentY, pacmanX, pacmanY);
			inChaseOrScatterMode = true;
			break;

		case EATEN:
			totalElapsedTime += elapsedTime;
			Eaten(currentX, currentY);
			inChaseOrScatterMode = false;
			break;

		case FRIGHTENED:
			//Don't add to totalElapsedTime here as totalElapsedTime is used to tell which ghost mode we should be in, and it isn't supposed to increment in frightened mode
			Frightened(currentX, currentY);
			inChaseOrScatterMode = false;
			break;

		case INHOUSE:
			totalElapsedTime += elapsedTime;
			InHouse(currentX, currentY);
			inChaseOrScatterMode = false;
			break;
		}
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
	collidedWithPacman = PacmanCollision(position->X, position->Y, pacmanXPos, pacmanYPos);
}

void Enemy::GhostHasBeenEaten()
{
	currMode = EATEN;
}

void Enemy::CheckDirection(bool(&ableToMoveInDirections)[4], int currentX, int currentY)
{
	//For ableToMoveInDirections, bools represent the following directions:
	//up, left, down, right
	//Done this way so that if the distance in each direction is equal, priority is given to up, then left, etc.

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

void Enemy::CalculateDirection(int currentX, int currentY)
{
	//up, left, down, right
	bool ableToMoveInDir[4] = { true,true,true,true };
	int distSquared;
	int smallestDistSquared = INT_MAX;
	direction newDirection = currDirection;

	CheckDirection(ableToMoveInDir, currentX, currentY);

	//up, left, down, right is the priority
	//ghost can't move backwards, so each internal if has currDirection != opposite direction
	//The ghosts use a greedy shortest path algorithm, so they look for the direction which gives the shortest distance
	//distance is left as squared, as if the distance is smallest squared, it is also smallest when rooted

	if (ableToMoveInDir[0])
	{
		distSquared = DistanceSquared(targetX, currentX, targetY, currentY - 1);
		if (distSquared < smallestDistSquared && currDirection != DOWN)
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
		if (distSquared < smallestDistSquared && currDirection != RIGHT)
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
		if (distSquared < smallestDistSquared && currDirection != UP)
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
		if (distSquared < smallestDistSquared && currDirection != LEFT)
		{
			smallestDistSquared = distSquared;
			newDirection = RIGHT;
			newTileX = currentX + 1;
			newTileY = currentY;
		}
	}
	currDirection = newDirection;
}

int Enemy::DistanceSquared(int x1, int x2, int y1, int y2)
{
	return ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
}

void Enemy::Chase(int currentX, int currentY, int pacmanX, int pacmanY, direction pacmanDirection, Enemy* blinky)
{
	int distSquared;
	int intermediateX, intermediateY;
	int blinkyX, blinkyY;
	int vectorX, vectorY;

	//blinky x&y calculated as the target tile for Inky is set based on blinkys target tile
	blinkyX = CalculateMazeX(blinky->GetVectorPointer()->X, sourceRect->Width, backgroundTileWidth);
	blinkyY = CalculateMazeY(blinky->GetVectorPointer()->Y, sourceRect->Width, backgroundTileHeight);

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
			targetY = _mazeHeight;
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
	if (targetX >= _mazeWidth)
		targetX = _mazeWidth - 1;

	if (targetY < 0)
		targetY = 0;
	if (targetY >= _mazeHeight)
		targetY = _mazeHeight - 1;
}

void Enemy::Scatter(int currentX, int currentY, int pacmanX, int pacmanY)
{
	//in scatter, each ghost sets it target tile to a specific corner

	switch (ghost)
	{
	case BLINKY:
		targetX = _mazeWidth - 2;
		targetY = 0;
		break;

	case PINKY:
		targetX = 1;
		targetY = 0;
		break;

	case INKY:
		targetX = _mazeWidth - 1;
		targetY = _mazeHeight;
		break;

	case CLYDE:
		targetX = 0;
		targetY = _mazeHeight;
		break;
	};
}

void Enemy::Eaten(int currentX, int currentY)
{
	//set target tile to inside the house, and set ableToLeaveHouse to true so that the ghost can get into the house
	targetX = 13;
	targetY = 14;
	ableToLeaveHouse = true;

	if (currentX == targetX && currentY == targetY)
	{
		currMode = INHOUSE;
	}
}

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

void Enemy::InHouse(int currentX, int currentY)
{
	//set target tile to outside of the house, so the ghost will try and leave
	targetX = _cHouseX;
	targetY = _cHouseY;

	if (totalElapsedTime >= TIMETOLEAVEHOUSE * static_cast<int>(ghost)) {
		ableToLeaveHouse = true;

		if (currentX == targetX && currentY == targetY)
			currMode = GetMode(totalElapsedTime);
	}
}

void Enemy::Move(int elapsedTime)
{
	float moveAmount = 0;

	switch (currMode)
	{
	case EATEN:
		moveAmount = _cEatenSpeedMultiplier * SPEED * elapsedTime;
		break;

	case FRIGHTENED:
		moveAmount = _cFrightenedSpeedMultiplier * SPEED * elapsedTime;
		break;

	default:
		moveAmount = SPEED * elapsedTime;
		break;
	}

	switch (currDirection)
	{
	case UP:
		position->Y -= moveAmount;
		break;

	case DOWN:
		position->Y += moveAmount;
		break;

	case LEFT:
		position->X -= moveAmount;
		break;

	case RIGHT:
		position->X += moveAmount;
		break;
	}
}

void Enemy::ScreenWrapCheck()
{
	if (position->X < Graphics::GetViewportWidth() / 2.0f - ((_mazeWidth + 4) * backgroundTileWidth) / 2.0f)
	{
		position->X += (_mazeWidth + 1) * backgroundTileWidth;
		newTileX = _mazeWidth;
	}

	if (position->X > Graphics::GetViewportWidth() / 2.0f + ((_mazeWidth + 1) * backgroundTileWidth) / 2.0f)
	{
		position->X -= (_mazeWidth + 2) * backgroundTileWidth;
		newTileX = 0;
	}
}

bool Enemy::PacmanCollision(float ghostX, float ghostY, float pacmanX, float pacmanY)
{
	float collisionCentreX = ghostX + sourceRect->Width / 2.0f;
	float collisionCentreY = ghostY + sourceRect->Height / 2.0f;

	//each offset is 1/4 of the width or height so that the centre +- the offset is the coordinate of an edge
	float xOffset = sourceRect->Width / 4.0f;
	float yOffset = sourceRect->Height / 4.0f;

	return ((collisionCentreX - xOffset < pacmanX + sourceRect->Width) && (collisionCentreX + xOffset > pacmanX) && (collisionCentreY - yOffset < pacmanY + sourceRect->Height) && (collisionCentreY + yOffset > pacmanY));
}

void Enemy::Animate(int elapsedTime)
{
	currentFrameTime += elapsedTime;
	if (currentFrameTime > FRAMETIME)
	{
		frame++;
		if (frame >= 2)
			frame = 0;

		currentFrameTime = 0;
	}

	switch (currMode)
	{
	case CHASE:
	case SCATTER:
	case INHOUSE:
		sourceRect->X = sourceRect->Width * currDirection;
		break;

	case FRIGHTENED:
		sourceRect->X = sourceRect->Width * 8;
		break;

	case EATEN:
		sourceRect->X = sourceRect->Width * 4 + sourceRect->Width * currDirection;
		break;
	}
	sourceRect->Y = sourceRect->Width * frame;

}

Enemy::ghostMode Enemy::GetMode(unsigned int totalElapsedTime)
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