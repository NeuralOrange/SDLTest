#pragma once
#include "Spirit.h"

class SpiritRendering
{
public:
	SpiritRendering(SDL_Renderer* renderer) :renderer_(renderer){}
private:
	RenderdePtr renderer_;
};

