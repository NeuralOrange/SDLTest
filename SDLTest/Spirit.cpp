#include "Spirit.h"
Spirit_Result Spirit::Draw()
{
	if (!rendering||texture_ == nullptr)
		return Spirit_FILAURE;
	SDL_FRect srect = { texture_->w / stateNum_ * state_,0,texture_->w / stateNum_,texture_->h };
	if (SDL_RenderTexture(renderer_.get(), texture_.get(), &srect, &rect_))
		return Spirit_SUCCESS;
	else
	{
		SDL_Log("Couldn't draw surface successfully: %s", SDL_GetError());
		return Spirit_FILAURE;
	}
}

Spirit_Result Spirit::FlashState()
{
	if (stateCounter_ < stateFlashTimes_)
		stateCounter_++;
	else
	{
		stateCounter_ = 0;
		if (++state_  == stateNum_)
			state_ = 0;

	}
	return Spirit_SUCCESS;
}
