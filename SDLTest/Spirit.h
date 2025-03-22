#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include <string>

using SurfacePtr = std::shared_ptr<SDL_Surface>;
using TexturePtr = std::shared_ptr<SDL_Texture>;
using RendererPtr = std::shared_ptr<SDL_Renderer>;

typedef enum Spirit_Result
{
	Spirit_FILAURE,
	Spirit_SUCCESS
}Spirit_Result;

class Spirit
{
public:
	friend class SpiritNode;
	friend class GameObject;
	bool rendering = true;
	unsigned state_ = 0;
	unsigned stateNum_;

	TexturePtr texture_;
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="surface"></param>
	/// <param name="renderer"></param>
	/// <param name="stateNum"></param>
	Spirit(SDL_Surface* surface, RendererPtr renderer,Uint64 stateNum = 1,float x = 0,float y = 0) : 
		surface_(SurfacePtr(surface, SDL_DestroySurface)), renderer_(renderer),stateNum_(stateNum)
	{
		Uint32 colorkey = SDL_MapRGB(SDL_GetPixelFormatDetails(surface_.get()->format), NULL,0, 255, 0);
		SDL_SetSurfaceColorKey(surface_.get(), true, colorkey);
		texture_ = TexturePtr(SDL_CreateTextureFromSurface(renderer_.get(), surface_.get()), SDL_DestroyTexture);
		rect_ = { x,y,float(texture_.get()->w / stateNum_),float(texture_.get()->h) };
	}
	Spirit(const char* bitmapFileName, RendererPtr renderer,Uint64 stateNum = 1,float x = 0,float y = 0) : Spirit(SDL_LoadBMP(bitmapFileName), renderer,stateNum,x,y)
	{

	}

	Spirit(TexturePtr texture, RendererPtr renderer, Uint64 stateNum = 1, float x = 0, float y = 0) 
	{
		surface_ = nullptr;
		texture_ = texture;
		rect_ = { x,y,float(texture_.get()->w / stateNum_),float(texture_.get()->h) };
		renderer_ = renderer;
		stateNum_ = stateNum;
	}

	Spirit_Result Draw();
	Spirit_Result FlashState();
	Spirit_Result ChangePosition(float x_change, float y_change);
	Spirit_Result SetPosition(float x, float y);
	Spirit_Result Scaling(float scaling_ratio);

protected:
	SDL_FRect rect_;
	SurfacePtr surface_;
	RendererPtr renderer_;
	unsigned stateFlashTimes_ = 5;
	unsigned stateCounter_ = 0;
};
