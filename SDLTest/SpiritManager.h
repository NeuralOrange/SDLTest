#pragma once
#include "Spirit.h"
#include "SpiritNode.h"
#include "GameObject.h"
#include <map>

class SpiritManager
{
public:
	SpiritManager(SDL_Renderer* renderer) :renderer_(std::shared_ptr<SDL_Renderer>(renderer,SDL_DestroyRenderer)){}
	
	Spirit_Result CreateSpirit(const std::string& filename,const std::string& name,SpiritType spiritType = SPIRIT_TYPE,Uint64 stateNum = 1,float x = 0,float y = 0);
	Spirit_Result CreateSpirit(TexturePtr texture, const std::string& name, SpiritType spiritType = SPIRIT_TYPE, Uint64 stateNum = 1, float x = 0, float y = 0);
	Spirit_Result DeleteSpirit(const std::string& name);
	SpiritNode* GetSpirit(const std::string& name);
	Spirit_Result DrawSpirit(const std::string& name);
	Spirit_Result FlashSpirit(const std::string& name);
	Spirit_Result DrawAll();
	Spirit_Result FlashALL();
	Spirit_Result MoveAllBySpeed(Uint64 time);
	TexturePtr GetTexture(const std::string& name);
	void AddGameObject(std::shared_ptr<GameObject> gameObject);

	std::shared_ptr<GameObject> GetGameObject(const std::string& name);

	void UpdateGameObjects(Uint64 time);

private:
	RendererPtr renderer_;
	std::map<std::string, SpiritNode> spirits;
	std::map<std::string, std::shared_ptr<GameObject>> gameObjects_;
	Spirit_Result AddSpirit(SpiritNode& spiritNode);
};

