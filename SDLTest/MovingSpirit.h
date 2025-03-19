#pragma once
#include "Spirit.h"
typedef enum
{
	DIR_RIGHT,
	DIR_UP,
	DIR_LEFT,
	DIR_DOWN
} MoveDirection;

class MovingSpirit :public Spirit
{
public:
	SDL_FRect rect_;
	float xVelocity = 0;
	float yVelocity = 0;
	float vLimit = 0.4;
	float acceleration = 0.2;
	MovingSpirit(Spirit spirit, int x, int y) :Spirit(spirit)
	{
		rect_.x = x;
		rect_.y = y;
		rect_.w = surface_->w;
		rect_.h = surface_->h;
	}
	Spirit_Result ChangeVelocity(MoveDirection dir , float limit = 1);
	Spirit_Result MoveBySpeed(Uint64 time);
	Spirit_Result Draw();
	Spirit_Result Move(MoveDirection dir, float moveDistance);
	Spirit_Result SetPosition(float x, float y);
	Spirit_Result ChangePosition(float x_change, float y_change);
};

