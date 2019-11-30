#pragma once

#include "S2D/S2D.h"
using namespace S2D;

struct Sprite
{
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	direction direction;
	int frame;
	int currentFrameTime;
};
