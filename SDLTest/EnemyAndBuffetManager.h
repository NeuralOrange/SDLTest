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
			if(i!=13&&i!=25)
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
	bool AddBullet(std::shared_ptr<Bullet>& bullet,Bullet::BulletType bulletType) 
	{
		if (bullet == nullptr)
			return false;
		switch (bulletType)
		{
		case Bullet::EighthRest:
			bullet->damage_ = 4;
			EighthRestBullets.push_back(bullet);
			break;
		case Bullet::SixteenthRest:
			bullet->damage_ = 2;
			SixteenthBullets.push_back(bullet);
			break;
		default:
			break;
		}
		Bullets.push_back(bullet);
		return true;
	}
	bool UpdataAll() 
	{
		for (int i = 0; i != enemyNum; i++)
		{
			if (!Enemys[i]->Update(0)||Enemys[i]->life_<=0)
				continue;
			for (int j = 0; j != Bullets.size(); j++)
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

	void GenerateBullet(float x, float y, Bullet::BulletType bulletType)
	{
		std::vector<std::shared_ptr<Bullet>>* bullets;
		switch (bulletType)
		{
		case Bullet::NONEBULLET:
			return;
			break;
		case Bullet::EighthRest:
			bullets = &EighthRestBullets;
			break;
		case Bullet::SixteenthRest:
			bullets = &SixteenthBullets;
			break;
		default:
			return;
			break;
		}
		for (int i = 0; i != bullets->size(); i++)
		{
			if (!(*bullets)[i]->active)
			{
				(*bullets)[i]->ShootBullet(x, y);
				return;
			}
		}
	}
private:
	std::vector<std::shared_ptr<Enemy>> Enemys;
	std::vector<std::shared_ptr<Bullet>> Bullets;
	std::vector<std::shared_ptr<Bullet>> EighthRestBullets;
	std::vector<std::shared_ptr<Bullet>> SixteenthBullets;
	unsigned enemyNum = 0;
	unsigned generateCount = 0;
	unsigned generateStepNum = 100;
	std::vector<float> possiableYpos;
};

