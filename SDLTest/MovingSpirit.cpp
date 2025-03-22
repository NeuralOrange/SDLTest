#include "MovingSpirit.h"

Spirit_Result MovingSpirit::ChangeVelocity(MoveDirection dir, float limit)
{
	switch (dir)
	{
	case DIR_RIGHT:
		if (xVelocity < limit*vLimit)
			xVelocity += acceleration;
		break;
	case DIR_UP:
		if (yVelocity > -limit*vLimit)
			yVelocity -= acceleration;
		break;
	case DIR_LEFT:
		if (xVelocity > -limit*vLimit)
			xVelocity -= acceleration;
		break;
	case DIR_DOWN:
		if (yVelocity < limit*vLimit)
			yVelocity += acceleration;
	default:
		break;
	}
	return Spirit_SUCCESS;
}

Spirit_Result MovingSpirit::MoveBySpeed(Uint64 time)
{
	return ChangePosition(xVelocity * time, yVelocity * time);
}

Spirit_Result MovingSpirit::Move(MoveDirection dir, float moveDistance)
{
	switch (dir)
	{
	case DIR_RIGHT:
		rect_.x += moveDistance;
		break;
	case DIR_UP:
		rect_.y -= moveDistance;
		break;
	case DIR_LEFT:
		rect_.x -= moveDistance;
		break;
	case DIR_DOWN:
		rect_.y += moveDistance;
	default:
		break;
	}
	return Spirit_SUCCESS;
}

Spirit_Result MovingSpirit::SetPosition(float x, float y)
{
	rect_.x = x;
	rect_.y = y;
	return Spirit_SUCCESS;
}

Spirit_Result MovingSpirit::ChangePosition(float x_change, float y_change)
{
	rect_.x += x_change;
	rect_.y += y_change;
	return Spirit_SUCCESS;
}
