#include "EnemyAndBuffetManager.h"
#include "GameObject.h"


std::vector<float> EnemyAndBuffetManager::possiableYpos = []() {
    std::vector<float> temp;
    for (int i = 2; i != 37; i++)
    {
        if (i != 13 && i != 25)
            possiableYpos.push_back(i * 18 + 60);
    }
    return temp;
    }();

bool EnemyAndBuffetManager::AddEnemy(std::shared_ptr<Enemy>& enemy)
{
    if (enemy == nullptr)
        return false;
    Enemys.push_back(enemy);
    enemyNum++;
    return true;
}

bool EnemyAndBuffetManager::AddBullet(std::shared_ptr<Bullet>& bullet, Bullet::BulletType bulletType)
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

bool EnemyAndBuffetManager::AddComponent(SpiritNode* component, ComponentType componentType)
{
	component->Active(false);
	switch (componentType)
	{
	case EnemyAndBuffetManager::Sharp:
		Sharps.push_back(component);
		break;
	case EnemyAndBuffetManager::Flat:
		Flats.push_back(component);
		break;
	default:
		break;
	}
	return true;
}

bool EnemyAndBuffetManager::UpdataAll()
{
	for (int i = 0; i != enemyNum; i++)
	{
		if (!Enemys[i]->Update(0) || Enemys[i]->life_ <= 0)
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
		int r = SDL_rand(NonePro+FlatPro+SharpPro);
		ComponentType t = NoneComponent;
		if (r < NonePro)
			t = NoneComponent;
		else if (r < NonePro + SharpPro)
			t = Sharp;
		else if (r < NonePro + SharpPro + FlatPro)
			t = Flat;
		GenerateEnemy(-0.1, 0, t);
		generateCount = 0;
	}

	return true;
}

void EnemyAndBuffetManager::GenerateEnemy(float xSpeed, float ySpeed, ComponentType type)
{
	SpiritNode* component = nullptr;
	for (int i = 0; i != enemyNum; i++)
	{
		if (!Enemys[i]->active)
		{

			switch (type)
			{
			case EnemyAndBuffetManager::NoneComponent:
				break;
			case EnemyAndBuffetManager::Sharp:
				component = GetComponent(Sharp);
				component->SetPosition(Enemys[i]->GetSpiritNode()->spirit->rect_.x - component->spirit->rect_.w, Enemys[i]->GetSpiritNode()->spirit->rect_.y);
				Enemys[i]->GetSpiritNode()->AddChild(component);
				Enemys[i]->type |= Enemy::isSharp;
				break;
			case EnemyAndBuffetManager::Flat:
				component = GetComponent(Flat);
				component->SetPosition(Enemys[i]->GetSpiritNode()->spirit->rect_.x - component->spirit->rect_.w, Enemys[i]->GetSpiritNode()->spirit->rect_.y);
				Enemys[i]->GetSpiritNode()->AddChild(component);
				Enemys[i]->type |= Enemy::isFlat;
				break;
			default:
				break;
			}
			int r = SDL_rand(EnemyAndBuffetManager::possiableYpos.size());
			Enemys[i]->TrackPos = r;
			Enemys[i]->GenerateEnemy(1024, EnemyAndBuffetManager::possiableYpos[r] - Enemys[i]->height, xSpeed, ySpeed);
			return;
		}
	}
	std::cerr << "���˶����� " << SDL_GetError() << std::endl;
}

void EnemyAndBuffetManager::GenerateBullet(float x, float y, Bullet::BulletType bulletType)
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

SpiritNode* EnemyAndBuffetManager::GetComponent(ComponentType type)
{
	switch (type)
	{
	case EnemyAndBuffetManager::NoneComponent:
		return nullptr;
		break;
	case EnemyAndBuffetManager::Sharp:
		for (auto& sharp : Sharps)
			if (!sharp->spirit->rendering)
				return sharp;
		return nullptr;
		break;
	case EnemyAndBuffetManager::Flat:
		for (auto& flat : Flats)
			if (!flat->spirit->rendering)
				return flat;
		return nullptr;
		break;
	default:
		break;
	}
	return nullptr;
}
