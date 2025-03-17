#include "Spirit.h"
Spirit_Result Spirit::Draw(float x, float y)
{
	SDL_FRect rect = { x, y, texture_->w, texture_->h };
	if (SDL_RenderTexture(renderer_, texture_, NULL, &rect))
		return Spirit_SUCCESS;
	else
	{
		SDL_Log("Couldn't draw surface successfully: %s", SDL_GetError());
		return Spirit_FILAURE;
	}
}

Spirit_Result Spirit::Draw(int x, int y)
{
	return this->Draw(float(x),float(y));
}
