#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
public:
    Enemy(const std::string& name, SpiritNode spiritNode, int life) : GameObject(name, spiritNode), life_(life) {}

    void Update(Uint64 time) override
    {
        if (!spiritNode_.spirit->rendering)
            return;
        if (life_ <= 0)
        {
            if(spiritNode_.spirit->stateNum_== 1)
                spiritNode_.spirit->rendering = false;
            else
            {
                if(last_time==0)
                {
					spiritNode_.spirit->state_++;
					last_time = 1;
					
				}
				else if (last_time < 1000)
                {
					last_time += time;
                }
                else if (last_time >= 1000)
                {
					spiritNode_.spirit->rendering = false;
                }
            }
        }
    }

    void TakeDamage(int damage)
    {
        life_ -= damage;
    }

	static std::shared_ptr<Enemy> GetEnemyPtr(std::shared_ptr<GameObject> enemy)
	{
		return std::dynamic_pointer_cast<Enemy>(enemy);
	}

private:
    int life_;
	Uint64 last_time = 0;
};
