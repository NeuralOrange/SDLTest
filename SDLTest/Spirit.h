#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include <string>

using SurfacePtr = std::shared_ptr<SDL_Surface>;
using TexturePtr = std::shared_ptr<SDL_Texture>;
using RenderdePtr = std::shared_ptr<SDL_Renderer>;

typedef enum Spirit_Result
{
	Spirit_FILAURE,
	Spirit_SUCCESS
}Spirit_Result;

class Spirit
{
public:
	bool rendering = true;
	std::string name_;
	unsigned state_ = 0;
	unsigned stateNum_;
	/// <summary>
	/// ¹¹Ôìº¯Êý
	/// </summary>
	/// <param name="surface"></param>
	/// <param name="renderer"></param>
	/// <param name="stateNum"></param>
	Spirit(SDL_Surface* surface, SDL_Renderer* renderer,std::string name,Uint64 stateNum = 1,float x = 0,float y = 0) : 
		surface_(SurfacePtr(surface, SDL_DestroySurface)), renderer_(RenderdePtr(renderer, SDL_DestroyRenderer)),stateNum_(stateNum),name_(name)
	{
		Uint32 colorkey = SDL_MapRGB(SDL_GetPixelFormatDetails(surface_.get()->format), NULL,0, 255, 0);
		SDL_SetSurfaceColorKey(surface_.get(), true, colorkey);
		texture_ = TexturePtr(SDL_CreateTextureFromSurface(renderer_.get(), surface_.get()), SDL_DestroyTexture);
		rect_ = { x,y,float(texture_.get()->w / stateNum_),float(texture_.get()->h) };
	}
	Spirit(const char* bitmapFileName, SDL_Renderer* renderer,std::string name,Uint64 stateNum = 1,float x = 0,float y = 0) : Spirit(SDL_LoadBMP(bitmapFileName), renderer,name,stateNum,x,y)
	{

	}

	Spirit_Result Draw();
	Spirit_Result FlashState();
protected:
	SDL_FRect rect_;
	SurfacePtr surface_;
	TexturePtr texture_;
	RenderdePtr renderer_;
	unsigned stateFlashTimes_ = 5;
	unsigned stateCounter_ = 0;
};
