#include "SpiritManager.h"

Spirit_Result SpiritManager::CreateSpirit(const std::string& filename, const std::string& name, SpiritType spiritType, Uint64 stateNum, float x, float y)
{
    if (spiritType == SPIRIT_TYPE)
    {
        SpiritNode spiritNode(std::make_shared<Spirit>(filename.c_str(), renderer_, stateNum, x, y), name);
        spiritNode.spiritType = SPIRIT_TYPE;
        return AddSpirit(spiritNode);
    }
    else if (spiritType == MOVE_SPIRIT_TYPE)
    {
        auto spirit = std::make_shared<Spirit>(filename.c_str(), renderer_, stateNum, x, y);
        SpiritNode spiritNode(spirit, name);
        spiritNode.enableMovingComponent = true;
        spiritNode.spiritType = MOVE_SPIRIT_TYPE;
        return AddSpirit(spiritNode);
    }
    else
    {
        return Spirit_FILAURE;
    }
}

Spirit_Result SpiritManager::CreateSpirit(TexturePtr texture, const std::string& name, SpiritType spiritType, Uint64 stateNum, float x, float y)
{
	if (spiritType == SPIRIT_TYPE)
	{
		SpiritNode spiritNode(std::make_shared<Spirit>(texture, renderer_, stateNum, x, y), name);
		spiritNode.spiritType = SPIRIT_TYPE;
		return AddSpirit(spiritNode);
	}
	else if (spiritType == MOVE_SPIRIT_TYPE)
	{
		auto spirit = std::make_shared<Spirit>(texture, renderer_, stateNum, x, y);
		SpiritNode spiritNode(spirit, name);
		spiritNode.enableMovingComponent = true;
		spiritNode.spiritType = MOVE_SPIRIT_TYPE;
		return AddSpirit(spiritNode);
	}
	else
	{
		return Spirit_FILAURE;
	}
}

Spirit_Result SpiritManager::CreateScense(const std::string& name)
{
	SpiritNode spiritNode(nullptr, name);
	return AddSpirit(spiritNode);
}

Spirit_Result SpiritManager::DeleteSpirit(const std::string& name)
{
	if (spirits.find(name) != spirits.end())
	{
		spirits.erase(name);
		return Spirit_SUCCESS;
	}
	else
	{
		return Spirit_FILAURE;
	}
}

SpiritNode* SpiritManager::GetSpirit(const std::string& name)
{
    if(spirits.find(name) != spirits.end())
	    return &spirits[name];
    else
		return nullptr;
}

Spirit_Result SpiritManager::DrawSpirit(const std::string& name)
{
	if (spirits.find(name) == spirits.end())
		return Spirit_FILAURE;
	spirits[name].Draw();
	return Spirit_SUCCESS;
}

Spirit_Result SpiritManager::FlashSpirit(const std::string& name)
{
	if (spirits.find(name) == spirits.end())
		return Spirit_FILAURE;
	spirits[name].FlashState();
	return Spirit_SUCCESS;
}

Spirit_Result SpiritManager::DrawAll()
{
	for (auto& spirit : RenderList)
	{
		if(spirit.parent==nullptr)
		spirit.Draw();
	}
	return Spirit_SUCCESS;
}

Spirit_Result SpiritManager::FlashALL()
{
	for (auto& spirit : spirits)
	{
		if(spirit.second.spirit->stateNum_>1)
			spirit.second.FlashState();
	}
	return Spirit_SUCCESS;
}

Spirit_Result SpiritManager::MoveAllBySpeed(Uint64 time)
{
	for (auto& spirit : spirits)
	{
		if (spirit.second.enableMovingComponent)
		{
			spirit.second.movingComponent.MoveBySpeed(time, spirit.second);
		}
	}
	return Spirit_SUCCESS;
}

TexturePtr SpiritManager::GetTexture(const std::string& name)
{
	if (spirits.find(name) == spirits.end())
		return nullptr;
	return spirits[name].spirit->texture_;
}

void SpiritManager::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	gameObjects_[gameObject->GetName()] = gameObject;
}

std::shared_ptr<GameObject> SpiritManager::GetGameObject(const std::string& name)
{
	return gameObjects_[name];
}


void SpiritManager::UpdateGameObjects(Uint64 time)
{
	for (auto& [name, gameObject] : gameObjects_)
	{
		gameObject->Update(time);
	}
}

Spirit_Result SpiritManager::AddSpirit(SpiritNode& spiritNode)
{
	if (spirits.find(spiritNode.name_) == spirits.end())
	{
		spirits[spiritNode.name_] = spiritNode;
		RenderList.push_back(spiritNode);
		return Spirit_SUCCESS;
	}
	else
	{
		return Spirit_FILAURE;
	}
}

