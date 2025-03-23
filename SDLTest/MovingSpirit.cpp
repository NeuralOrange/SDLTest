#pragma once
#include "MovingSpirit.h"
#include "SpiritNode.h"

Spirit_Result MovingSpiritComponent::ChangeVelocity(MoveDirection dir, float limit)
{
    switch (dir)
    {
    case DIR_RIGHT:
        xVelocity += acceleration;
        if (xVelocity > limit) xVelocity = limit;
        break;
    case DIR_UP:
        yVelocity -= acceleration;
        if (yVelocity < -limit) yVelocity = -limit;
        break;
    case DIR_LEFT:
        xVelocity -= acceleration;
        if (xVelocity < -limit) xVelocity = -limit;
        break;
    case DIR_DOWN:
        yVelocity += acceleration;
        if (yVelocity > limit) yVelocity = limit;
        break;
    default:
        return Spirit_FILAURE;
    }
    return Spirit_SUCCESS;
}

Spirit_Result MovingSpiritComponent::ChangeVelocity(float vX, float vY)
{
    xVelocity = vX;
    yVelocity = vY;
    return Spirit_SUCCESS;
}

Spirit_Result MovingSpiritComponent::MoveBySpeed(Uint64 time, SpiritNode& spirit)
{
    if (!spirit.enableMovingComponent) return Spirit_FILAURE;

    float xChange = xVelocity * time;
    float yChange = yVelocity * time;

    return spirit.ChangePosition(xChange, yChange);
}

Spirit_Result MovingSpiritComponent::Move(MoveDirection dir, float moveDistance, SpiritNode& spirit)
{
    if (!spirit.enableMovingComponent) return Spirit_FILAURE;

    float xChange = 0;
    float yChange = 0;

    switch (dir)
    {
    case DIR_RIGHT:
        xChange = moveDistance;
        break;
    case DIR_UP:
        yChange = -moveDistance;
        break;
    case DIR_LEFT:
        xChange = -moveDistance;
        break;
    case DIR_DOWN:
        yChange = moveDistance;
        break;
    default:
        return Spirit_FILAURE;
    }

    return spirit.ChangePosition(xChange, yChange);
}

