#pragma once

#include "S2D/S2D.h"
using namespace S2D;

struct Player
{
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	direction direction;
	int frame;
	int currentFrameTime;
	int score;
	Vector2* scoreOutputPos;
};
