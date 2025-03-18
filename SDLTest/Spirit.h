#pragma once
#include <SDL3/SDL.h>
typedef enum Spirit_Result
{
	Spirit_FILAURE,
	Spirit_SUCCESS
}Spirit_Result;

class Spirit
{
public:
	unsigned state_ = 0;
	Spirit(SDL_Surface* surface,SDL_Renderer* renderer) : surface_(surface),renderer_(renderer)
	{
		texture_ = SDL_CreateTextureFromSurface(renderer_,surface_);
	}
	Spirit(const char* bitmapFileName, SDL_Renderer* renderer) : Spirit(SDL_LoadBMP(bitmapFileName), renderer)
	{

	}
	~Spirit()
	{
		SDL_DestroySurface(surface_);
	}
	Spirit_Result Draw(float x, float y);
	Spirit_Result Draw(int x, int y);
protected:
	SDL_Surface* surface_;
	SDL_Texture* texture_;
	SDL_Renderer* renderer_;
}; 
