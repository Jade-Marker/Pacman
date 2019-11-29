#pragma once

#include "S2D/S2D.h"
using namespace S2D;

struct Menu
{
	Texture2D* texture;
	Rect* rect;
	Vector2* stringPosition;
	bool keyDown;
	bool inUse;
	Input::Keys interactKey;
};