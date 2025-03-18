#pragma once
#include <SDL3/SDL.h>
#include <memory>

typedef enum Spirit_Result
{
	Spirit_FILAURE,
	Spirit_SUCCESS
}Spirit_Result;

class Spirit
{
public:
	using SurfacePtr = std::shared_ptr<SDL_Surface>;
	using TexturePtr = std::shared_ptr<SDL_Texture>;
	using RenderdePtr = std::shared_ptr<SDL_Renderer>;
	
	unsigned state_ = 0;
	unsigned stateNum_ = 1;
	Spirit(SDL_Surface* surface,SDL_Renderer* renderer) : surface_(SurfacePtr(surface,SDL_DestroySurface)),renderer_(RenderdePtr(renderer,SDL_DestroyRenderer))
	{
		texture_ =TexturePtr(SDL_CreateTextureFromSurface(renderer_.get(), surface_.get()),SDL_DestroyTexture);
	}
	Spirit(const char* bitmapFileName, SDL_Renderer* renderer) : Spirit(SDL_LoadBMP(bitmapFileName), renderer)
	{

	}

	Spirit_Result Draw(float x, float y);
	Spirit_Result Draw(int x, int y);
protected:
	SurfacePtr surface_;
	TexturePtr texture_;
	RenderdePtr renderer_;
}; 
