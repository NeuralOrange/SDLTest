#pragma once
#include "Spirit.h"

typedef enum
{
    DIR_RIGHT,
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN
} MoveDirection;

class MovingSpiritComponent
{
public:
    float xVelocity = 0;
    float yVelocity = 0;
    float vLimit = 0.4;
    float acceleration = 0.2;

    Spirit_Result ChangeVelocity(MoveDirection dir, float limit = 1);
    Spirit_Result MoveBySpeed(Uint64 time, SpiritNode& spirit);
    Spirit_Result Move(MoveDirection dir, float moveDistance, SpiritNode& spirit);
};


