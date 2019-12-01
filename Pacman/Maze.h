#pragma once
#include "S2D/S2D.h"
using namespace S2D;
const int cMazeWidth = 28;
const int cMazeHeight = 31;

const float cTilesetTileWidth = 32.0f;
const float cTilesetTileHeight = 32.0f;

enum mazeUnits {
	EMPTY, HORIZONTAL_TOP, HORIZONTAL_BOTTOM, VERTICAL_LEFT, VERTICAL_RIGHT, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT,
	HORIZONTAL_TOP_SINGLE, HORIZONTAL_BOTTOM_SINGLE, VERTICAL_RIGHT_SINGLE, VERTICAL_LEFT_SINGLE, TOP_LEFT_SMALL, TOP_RIGHT_SMALL, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL,
	TOP_LEFT_STRAIGHT, TOP_RIGHT_STRAIGHT, BOTTOM_LEFT_STRAIGHT, BOTTOM_RIGHT_STRAIGHT,
	HORIZONTAL_LEFT_STRAIGHT_CORNER, HORIZONTAL_RIGHT_STRAIGHT_CORNER,
	VERTICAL_LEFT_STRAIGHT_CORNER_TOP, VERTICAL_LEFT_STRAIGHT_CORNER_BOTTOM, VERTICAL_RIGHT_STRAIGHT_CORNER_TOP, VERTICAL_RIGHT_STRAIGHT_CORNER_BOTTOM, GHOST_ENTRANCE, PELLET, POWER_PELLET
};

const mazeUnits cMazeTemplate[cMazeHeight][cMazeWidth] =
{
	{TOP_LEFT, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_RIGHT_STRAIGHT_CORNER, HORIZONTAL_LEFT_STRAIGHT_CORNER, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, TOP_RIGHT},
	{VERTICAL_LEFT, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, POWER_PELLET, VERTICAL_LEFT_SINGLE, EMPTY, EMPTY, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_LEFT_SINGLE, EMPTY, EMPTY, EMPTY, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_LEFT_SINGLE, EMPTY, EMPTY, EMPTY, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_LEFT_SINGLE, EMPTY, EMPTY, VERTICAL_RIGHT_SINGLE, POWER_PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, TOP_RIGHT_SMALL, TOP_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_RIGHT},
	{BOTTOM_LEFT, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, TOP_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, BOTTOM_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, EMPTY, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, EMPTY, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, BOTTOM_RIGHT_SMALL, VERTICAL_RIGHT_SINGLE, PELLET, TOP_LEFT_SMALL, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, BOTTOM_RIGHT},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, PELLET, VERTICAL_LEFT_SINGLE, TOP_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, EMPTY, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, EMPTY, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, TOP_RIGHT_SMALL, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, EMPTY, TOP_LEFT_STRAIGHT, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, GHOST_ENTRANCE, GHOST_ENTRANCE, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, TOP_RIGHT_STRAIGHT, EMPTY, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, EMPTY, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, EMPTY, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, PELLET, EMPTY, EMPTY, EMPTY, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, EMPTY, EMPTY, EMPTY, PELLET, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, TOP_RIGHT_SMALL, EMPTY, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, EMPTY, TOP_LEFT_SMALL, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, EMPTY, BOTTOM_LEFT_STRAIGHT, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, BOTTOM_RIGHT_STRAIGHT, EMPTY, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, VERTICAL_LEFT, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, EMPTY, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, EMPTY, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, VERTICAL_RIGHT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{TOP_LEFT, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, EMPTY, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, TOP_RIGHT_SMALL, TOP_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, EMPTY, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, HORIZONTAL_TOP, TOP_RIGHT},
	{VERTICAL_LEFT, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, TOP_RIGHT_SMALL, VERTICAL_RIGHT_SINGLE, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, TOP_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, POWER_PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, POWER_PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT_STRAIGHT_CORNER_TOP, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, TOP_LEFT_SMALL, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, TOP_LEFT_SMALL, TOP_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, VERTICAL_RIGHT_STRAIGHT_CORNER_TOP},
	{VERTICAL_LEFT_STRAIGHT_CORNER_BOTTOM, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, TOP_RIGHT_SMALL, TOP_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, VERTICAL_RIGHT_STRAIGHT_CORNER_BOTTOM},
	{VERTICAL_LEFT, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, BOTTOM_RIGHT_SMALL, BOTTOM_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, VERTICAL_LEFT_SINGLE, VERTICAL_RIGHT_SINGLE, PELLET, TOP_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, BOTTOM_RIGHT_SMALL, BOTTOM_LEFT_SMALL, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, HORIZONTAL_TOP_SINGLE, TOP_RIGHT_SMALL, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, BOTTOM_RIGHT_SMALL, PELLET, BOTTOM_LEFT_SMALL, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, HORIZONTAL_BOTTOM_SINGLE, BOTTOM_RIGHT_SMALL, PELLET, VERTICAL_RIGHT},
	{VERTICAL_LEFT, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, PELLET, VERTICAL_RIGHT},
	{BOTTOM_LEFT, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, HORIZONTAL_BOTTOM, BOTTOM_RIGHT}
};

int CalculateMazeX(float inputX, float objectWidth, float backgroundElementWidth);
int CalculateMazeY(float inputY, float objectHeight, float backgroundElementHeight);
