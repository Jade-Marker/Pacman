#pragma once

#include "S2D/S2D.h"
#include "Sprite.h"
using namespace S2D;

struct Player
{
	Sprite playerSprite;
	int score;
	int lives;
	Vector2* scoreOutputPos;
};
