#include "SpiritNode.h"

Spirit_Result SpiritNode::AddChild(SpiritNode* childNode)
{
	childNode->parent = spirit;
	dictionary[childNode->name_] = children.size();
	children.push_back(childNode);
	return Spirit_SUCCESS;
}

SpiritNode* SpiritNode::GetChild(const std::string& name)
{
	return children[dictionary[name]];
}

Spirit_Result SpiritNode::DeleteChlid(const std::string& name)
{
	children.erase(children.begin() + dictionary[name]);
	for (size_t i = dictionary[name] + 1; i != dictionary.size(); i++)
	{
		dictionary[children[i]->name_]--;
	}
	dictionary.erase(name);
	return Spirit_SUCCESS;

}

Spirit_Result SpiritNode::ChangePosition(float x, float y)
{
	spirit->ChangePosition(x, y);
	for(int i = 0;i!=children.size();i++)
	{
		children[i]->ChangePosition(x, y);
	}
	return Spirit_SUCCESS;
}

Spirit_Result SpiritNode::SetPosition(float x, float y)
{
	auto xChange = x - spirit->rect_.x;
	auto yChange = y - spirit->rect_.y;
	spirit->SetPosition(x, y);
	for (int i = 0; i != children.size(); i++)
	{
		children[i]->ChangePosition(xChange, yChange);

	}
	return Spirit_SUCCESS;
}

Spirit_Result SpiritNode::Draw()
{
	if (spirit != nullptr) {
		spirit->Draw();
	}
	else
		return Spirit_FILAURE;
	for (int i = 0; i != children.size(); i++)
	{
		children[i]->Draw();
	}
	return Spirit_SUCCESS;
}

Spirit_Result SpiritNode::Active(bool active)
{
	if (spirit != nullptr)
		spirit->Active(active);
	for (int i = 0; i != children.size(); i++)
	{
		children[i]->Active(active);
	}
	return Spirit_SUCCESS;
}

Spirit_Result SpiritNode::Scaling(float scaling_ratio)
{
	if (spirit != nullptr)
	spirit->Scaling(scaling_ratio);
	for (int i = 0; i != children.size(); i++)
	{
		children[i]->Scaling(scaling_ratio);
	}
	return Spirit_SUCCESS;
}

Spirit_Result SpiritNode::FlashState()
{
	if (spirit != nullptr)
	spirit->FlashState();
	for (int i = 0; i != children.size(); i++)
	{
		children[i]->FlashState();
	}
	return Spirit_SUCCESS;
}
