#pragma once
#include "SpiritManager.h"

class GameObject
{
public:
    GameObject(const std::string& name, SpiritNode spiritNode) : name_(name), spiritNode_(spiritNode) {}
    virtual ~GameObject() = default;

    virtual void Update(Uint64 time) = 0;

    const std::string& GetName() const { return name_; }
    SpiritNode& GetSpiritNode() { return spiritNode_; }

    static bool CheckCollision(std::shared_ptr<GameObject> obj1,std::shared_ptr<GameObject> obj2)
    {
        if(!obj1->spiritNode_.spirit->rendering||!obj2->spiritNode_.spirit->rendering)
			return false;
        const SDL_FRect& rect1 = obj1->spiritNode_.spirit->rect_;
        const SDL_FRect& rect2 = obj2->spiritNode_.spirit->rect_;

        return SDL_HasRectIntersectionFloat(&rect1, &rect2);
    }


protected:
    std::string name_;
    SpiritNode spiritNode_;
};
