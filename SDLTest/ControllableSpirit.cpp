#include "ControllableSpirit.h"

Spirit_Result ControllableSpirit::ChangeVelocity(MoveDirection dir, Uint64 time)
{
	switch (dir)
	{
	case DIR_RIGHT:
		if(xVelocity<vLimit)
			xVelocity += time*acceleration;
		break;
	case DIR_UP:
		if(yVelocity>-vLimit)
			yVelocity -= time*acceleration;
		break;
	case DIR_LEFT:
		if(xVelocity>-vLimit)
			xVelocity-=time*acceleration;
		break;
	case DIR_DOWN:
		if(yVelocity<vLimit)
			yVelocity+=time*acceleration;
	default:
		break;
	}
	return Spirit_SUCCESS;
}

Spirit_Result ControllableSpirit::MoveBySpeed(Uint64 time)
{
	return ChangePosition(xVelocity*time,yVelocity*time);
}

Spirit_Result ControllableSpirit::Draw()
{
    return Spirit::Draw(rect_.x, rect_.y);
}

Spirit_Result ControllableSpirit::Move(MoveDirection dir, float moveDistance)
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

Spirit_Result ControllableSpirit::SetPosition(float x, float y)
{
	rect_.x = x;
	rect_.y = y;
	return Spirit_SUCCESS;
}

Spirit_Result ControllableSpirit::ChangePosition(float x_change, float y_change)
{
	rect_.x += x_change;
	rect_.y += y_change;
	return Spirit_SUCCESS;
}
