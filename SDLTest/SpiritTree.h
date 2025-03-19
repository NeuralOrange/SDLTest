#pragma once
#include"spirit.h"
#include <map>
class SpiritNode
{
public:
	std::weak_ptr<Spirit> spirit;
	std::shared_ptr<Spirit> parent;
	std::vector<SpiritNode> children;
	std::map<std::string, size_t> dictionary;
	Spirit_Result AddChild(SpiritNode childNode)
	{
		if (childNode.parent = spirit.lock())
			;
		else
			return Spirit_FILAURE;
		dictionary[childNode.spirit.lock()->name_] = children.size();
		children.push_back(childNode);
		return Spirit_SUCCESS;
	}

	Spirit_Result DeleteChlid(std::string name) 
	{
		children.erase(children.begin()+dictionary[name]);

	}
private:

};

class SpiritTree 
{
public:
	SpiritNode root;
	SpiritTree(std::shared_ptr<Spirit> spirit) 
	{
		root.parent = nullptr;
		root.spirit = std::shared_ptr<Spirit>(spirit);
	}
private:

};
