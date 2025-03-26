#pragma once
#include"Spirit.h"
#include "MovingSpirit.h"
#include <string>
#include <map>



typedef enum SpiritType
{
	SPIRIT_TYPE,
	MOVE_SPIRIT_TYPE
} SpiritType;

class SpiritNode
{
public:
	std::shared_ptr<Spirit> spirit;
	std::shared_ptr<Spirit> parent;
	std::vector<SpiritNode*> children;
	std::map<std::string, size_t> dictionary;
	
	std::string name_;
	SpiritType spiritType = SPIRIT_TYPE;
	
	bool enableMovingComponent = false; // 用于启用/禁用 MovingSpiritComponent
	MovingSpiritComponent movingComponent;

	SpiritNode() = default;
	SpiritNode(std::shared_ptr<Spirit> spirit,const std::string& name) :spirit(spirit),name_(name)
	{
		parent = nullptr;
	}

	Spirit_Result AddChild(SpiritNode* childNode);
	SpiritNode* GetChild(const std::string& name);
	Spirit_Result DeleteChlid(const std::string& name);
	Spirit_Result ChangePosition(float x, float y);
	Spirit_Result SetPosition(float x, float y);
	Spirit_Result FlashState();
	Spirit_Result Draw();
	Spirit_Result Active(bool active);
	Spirit_Result Scaling(float scaling_ratio);
	Spirit_Result ChangeState(unsigned state);
	Spirit_Result ClearChild();
private:

};


