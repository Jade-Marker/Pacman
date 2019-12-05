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
	int noOfFrames;

	void Animate(int elapsedTime, int frameTime)
	{
		currentFrameTime += elapsedTime;
		if (currentFrameTime > frameTime)
		{
			frame++;
			if (frame >= noOfFrames)
				frame = 0;

			currentFrameTime = 0;
		}
	}

	static void Animate(int elapsedTime, int frameTime, int noOfFrames, int& currentFrame, int& currentFrameTimeInput)
	{
		currentFrameTimeInput += elapsedTime;
		if (currentFrameTimeInput > frameTime)
		{
			currentFrame++;
			if (currentFrame >= noOfFrames)
				currentFrame = 0;

			currentFrameTimeInput = 0;
		}
	}
};
