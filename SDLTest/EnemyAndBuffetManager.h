#pragma once
#include "Spirit.h"
#include "GameObject.h"
#include <vector>
#include <iostream>
class EnemyAndBuffetManager
{
public:
	typedef enum ComponentType
	{
		NoneComponent,
		Sharp,
		Flat
	};

	static std::vector<float> possiableYpos;
	static int life;
	static int score;
	EnemyAndBuffetManager() = default;

	bool AddEnemy(std::shared_ptr<Enemy>& enemy);
	bool AddBullet(std::shared_ptr<Bullet>& bullet,Bullet::BulletType bulletType);
	bool AddComponent(SpiritNode* component,ComponentType componentType);
	bool UpdataAll();
	void GenerateEnemy(float xSpeed, float ySpeed,ComponentType type = NoneComponent);
	void GenerateBullet(float x, float y, Bullet::BulletType bulletType);
	void ResetAll();

	SpiritNode* GetComponent(ComponentType type);

private:
	std::vector<std::shared_ptr<Enemy>> Enemys;
	std::vector<std::shared_ptr<Bullet>> Bullets;
	std::vector<std::shared_ptr<Bullet>> EighthRestBullets;
	std::vector<std::shared_ptr<Bullet>> SixteenthBullets;
	std::vector<SpiritNode*> Sharps;
	std::vector<SpiritNode*> Flats;
	unsigned enemyNum = 0;
	unsigned generateCount = 0;
	unsigned generateStepNum = 100;
	unsigned FlatPro = 1;
	unsigned SharpPro = 1;
	unsigned NonePro = 2;

	
};

