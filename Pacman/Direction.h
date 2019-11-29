#pragma once
enum direction { RIGHT, DOWN, LEFT, UP, NONE };

inline direction OppositeDirection(direction initalDir)
{
	direction result = NONE;

	switch (initalDir)
	{
	case UP:
		result = DOWN;
		break;

	case DOWN:
		result = UP;
		break;

	case LEFT:
		result = RIGHT;
		break;

	case RIGHT:
		result = LEFT;
		break;
	}

	return result;
}