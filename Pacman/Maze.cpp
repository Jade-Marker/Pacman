#include "Maze.h"

int CalculateMazeX(float inputX, float objectWidth, float backgroundElementWidth)
{
	return (int)((inputX + objectWidth / 2.0f - ((Graphics::GetViewportWidth() / 2.0f) - ((cMazeWidth * backgroundElementWidth) / 2.0f))) / backgroundElementWidth);
}

int CalculateMazeY(float inputY, float objectHeight, float backgroundElementHeight)
{
	return (int)((inputY + objectHeight / 2.0f) / backgroundElementHeight);
}