#pragma once
#include "Spirit.h"
typedef enum
{
	DIR_RIGHT,
	DIR_UP,
	DIR_LEFT,
	DIR_DOWN
} MoveDirection;

class ControllableSpirit : Spirit
{
	public:
		SDL_FRect rect_;
		ControllableSpirit(Spirit spirit,int x,int y):Spirit(spirit)
		{
			rect_.x = x;
			rect_.y = y;
			rect_.w = surface_->w;
			rect_.h = surface_->h;
		}
		Spirit_Result Draw();
		Spirit_Result Move(MoveDirection dir,float moveDistance);
		Spirit_Result SetPosition(int x, int y);
		Spirit_Result ChangePosition(int x_change, int y_change);
};

