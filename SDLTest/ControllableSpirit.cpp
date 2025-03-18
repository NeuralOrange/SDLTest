#include "ControllableSpirit.h"

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

Spirit_Result ControllableSpirit::SetPosition(int x, int y)
{
	rect_.x = x;
	rect_.y = y;
	return Spirit_SUCCESS;
}

Spirit_Result ControllableSpirit::ChangePosition(int x_change, int y_change)
{
	rect_.x += x_change;
	rect_.y += y_change;
	return Spirit_SUCCESS;
}
