#pragma once
#include "Spirit.h"
#include "GameObject.h"
#include <iostream>
class EnemyAndBuffetManager
{
public:
	EnemyAndBuffetManager()
	{
		for (int i = 2; i != 37; i++) 
		{
			possiableYpos.push_back(i*18+60);
		}
	}

	bool AddEnemy(std::shared_ptr<Enemy>& enemy) 
	{
		if (enemy == nullptr)
			return false;
		Enemys.push_back(enemy);
		enemyNum++;
		return true;
	}
	bool AddBullet(std::shared_ptr<Bullet>& bullet) 
	{
		if (bullet == nullptr)
			return false;
		Bullets.push_back(bullet);
		bulletNum++;
		return true;
	}
	bool UpdataAll() 
	{
		for (int i = 0; i != enemyNum; i++)
		{
			if (!Enemys[i]->Update(0)||Enemys[i]->life_<=0)
				continue;
			for (int j = 0; j != bulletNum; j++)
			{
				if (GameObject::CheckCollision(Enemys[i], Bullets[j]))
				{
					Enemys[i]->TakeDamage(Bullets[j]->damage_);
					Bullets[j]->HitTarget();

				}
			}
		}

		for (auto& bullet : Bullets) 
		{
			bullet->Update(0);
		}

		if (++generateCount == generateStepNum) {
			GenerateEnemy(-0.1, 0);
			generateCount = 0;
		}

		return true;
	}
	void GenerateEnemy(float xSpeed, float ySpeed) 
	{
		for (int i = 0;i!=enemyNum;i++) 
		{
			if (!Enemys[i]->active)
			{
				int r = SDL_rand(possiableYpos.size());
				Enemys[i]->GenerateEnemy(1024, possiableYpos[r] - Enemys[i]->height,xSpeed, ySpeed);
				return;
			}
		}
		std::cerr << "敌人对象不足 " << SDL_GetError() << std::endl;
	}
private:
	std::vector<std::shared_ptr<Enemy>> Enemys;
	std::vector<std::shared_ptr<Bullet>> Bullets;
	unsigned enemyNum = 0;
	unsigned bulletNum = 0;
	unsigned generateCount = 0;
	unsigned generateStepNum = 100;
	std::vector<float> possiableYpos;
};

