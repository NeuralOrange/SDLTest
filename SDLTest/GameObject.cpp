#include "GameObject.h"
#include"EnemyAndBuffetManager.h"

GameObject::GameObject(const std::string& name, SpiritNode* spiritNode) : name_(name), spiritNode_(spiritNode)
{
    animePlay = new AnimePlay(spiritNode_, spiritNode_->spirit->stateNum_, false);
}

GameObject::~GameObject()
{
    if (animePlay != nullptr)
        free(animePlay);
}

const std::string& GameObject::GetName() const { return name_; }

SpiritNode* GameObject::GetSpiritNode() { return spiritNode_; }

bool GameObject::CheckCollision(std::shared_ptr<GameObject> obj1, std::shared_ptr<GameObject> obj2)
{
    for (auto& child1 : obj1->spiritNode_->children)
    {
        if (!child1->spirit->rendering)
            continue;
        for (auto& child2 : obj2->spiritNode_->children)
        {
            if (!child2->spirit->rendering)
                continue;
            const SDL_FRect& rect1 = obj1->spiritNode_->spirit->rect_;
            const SDL_FRect& rect2 = obj2->spiritNode_->spirit->rect_;
            if (SDL_HasRectIntersectionFloat(&rect1, &rect2))
                return true;
        }
    }
    if (!obj1->spiritNode_->spirit->rendering || !obj2->spiritNode_->spirit->rendering)
        return false;
    const SDL_FRect& rect1 = obj1->spiritNode_->spirit->rect_;
    const SDL_FRect& rect2 = obj2->spiritNode_->spirit->rect_;

    return SDL_HasRectIntersectionFloat(&rect1, &rect2);
}

void GameObject::MotionLimitations(float x, float y, float w, float h)
{
    SDL_FRect rect = spiritNode_->spirit->rect_;
    if (rect.x < x)
        spiritNode_->SetPosition(x, rect.y);
    else if (rect.x > x + w - rect.w)
        spiritNode_->SetPosition(x + w - rect.w, rect.y);
    rect = spiritNode_->spirit->rect_;
    if (rect.y < y)
        spiritNode_->SetPosition(rect.x, y);
    else if (rect.y > y + h - rect.h)
        spiritNode_->SetPosition(rect.x, y + h - rect.h);
}

Bullet::Bullet(const std::string& name, SpiritNode* spiritNode, BulletType bulletType) : GameObject(name, spiritNode)
{
    spiritNode->movingComponent.ChangeVelocity(0, speed);
    spiritNode->spirit->rendering = false;
    type_ = bulletType;
    animePlay->isLoop = true;
    animePlay->flashNum = 10;
}

bool Bullet::Update(Uint64 time)
{
    if (!spiritNode_->spirit->rendering)
        return false;

    auto xPos = spiritNode_->spirit->rect_.x;
    auto yPos = spiritNode_->spirit->rect_.y;

    if (xPos > SDL_WINDOW_WIDTH) {
        animePlay->Reset();
        spiritNode_->spirit->rendering = false;
        active = false;
        return false;
    }
    animePlay->PlayAnime();

    return true;
}

std::shared_ptr<Bullet> Bullet::GetPtr(std::shared_ptr<GameObject> bullet)
{
    return std::dynamic_pointer_cast<Bullet>(bullet);
}

void Bullet::ShootBullet(float x, float y)
{
    active = true;
    spiritNode_->SetPosition(x, y - spiritNode_->spirit->rect_.h / 2);
    spiritNode_->movingComponent.ChangeVelocity(0.2, 0);
    spiritNode_->spirit->rendering = true;
    animePlay->flashStart = true;
}

std::string Bullet::BulletName(int i, BulletType bulletType)
{
    std::string bulletName;
    switch (bulletType)
    {
    case Bullet::EighthRest:
        bulletName = "EighthRest";
        break;
    case Bullet::SixteenthRest:
        bulletName = "SixteenthRest";
        break;
    default:
        break;
    }
    return bulletName + std::to_string(i);
}

void Bullet::HitTarget()
{
    spiritNode_->spirit->rendering = false;
    active = false;
    animePlay->Reset();
}

Button::Button(const std::string& name, SpiritNode* spiritNode, unsigned type) : GameObject(name, spiritNode)
{

}

bool Button::Update(Uint64 time)
{
    if (!spiritNode_->spirit->rendering)
        return false;
    return true;
}

std::shared_ptr<Button> Button::GetPtr(std::shared_ptr<GameObject> button)
{
    return std::dynamic_pointer_cast<Button>(button);
}

void Button::Holding(unsigned state)
{
    spiritNode_->spirit->state_ = state;
    isHolding = state;
}

bool Button::PointIn(float x, float y)
{
    SDL_FPoint point{ x,y };
    return SDL_PointInRectFloat(&point, &spiritNode_->spirit->rect_);
}

Player::Player(const std::string name, SpiritNode* spiritNode) :GameObject(name, spiritNode)
{
    if (animePlay != nullptr)
        animePlay = new AnimePlay(spiritNode->children[0], flashTimes, false);
}

bool Player::Update(Uint64 time)
{
    if (!spiritNode_->spirit->rendering)
        return false;
    MotionLimitations(0, 0, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT);
    auto& attackEffect = spiritNode_->children[0]->spirit;
    if (staminaStart)
    {
        if (!attackEffect->rendering)
        {
            animePlay->flashStart = true;
            attackEffect->rendering = true;
        }
        else
        {
            if (animePlay->PlayAnime(2))
            {

            }
            else
            {
                staminaTime++;
            }
        }
    }
    else
    {
        attackEffect->rendering = false;

    }
    return true;
}

std::string Player::Attack()
{
    if (staminaTime < flashTimes)
    {
        staminaStart = false;
        animePlay->Reset();
        return "too fast";
    }
    float space = 120;
    shootX = spiritNode_->spirit->rect_.x + spiritNode_->children[0]->spirit->rect_.w + spiritNode_->spirit->rect_.w;
    shootY = spiritNode_->spirit->rect_.y + spiritNode_->spirit->rect_.h / 2;
    staminaStart = false;
    animePlay->Reset();
    SDL_FRect* rect_ = &spiritNode_->spirit->rect_;
    auto widthUnit = (SDL_WINDOW_HEIGHT - space) / 6;
    for (int i = 1; i != 7; i++)
    {
        if (shootY < widthUnit * i + space / 2)
        {
            return AudioName[i - 1];
        }
    }
    return "";

}

std::shared_ptr<Player> Player::GetPtr(std::shared_ptr<GameObject> player)
{
    return std::dynamic_pointer_cast<Player>(player);
}

Bullet::BulletType Player::GetBulletType()
{
    if (staminaTime < flashTimes)
        return Bullet::NONEBULLET;
    else if (staminaTime < flashTimes * 2)
        return Bullet::SixteenthRest;
    else
        return Bullet::EighthRest;

}

Background::Background(const std::string name, SpiritNode* spiritNode) :GameObject(name, spiritNode)
{
    spiritNode->movingComponent.ChangeVelocity(-0.1, 0);
}

bool Background::Update(Uint64 time)
{

    if (!spiritNode_->spirit->rendering)
        return false;
    auto background = spiritNode_->spirit;
    if (background->rect_.x < -SDL_WINDOW_WIDTH)
        background->rect_.x = 0;

    return true;
}

std::shared_ptr<Background> Background::GetPtr(std::shared_ptr<GameObject> background)
{
    return std::dynamic_pointer_cast<Background>(background);
}

Enemy::Enemy(const std::string& name, SpiritNode* spiritNode, int life) : GameObject(name, spiritNode), life_(life)
{
    maxLife = life;
    height = spiritNode->spirit->rect_.h;
    spiritNode->spirit->rendering = false;
    animePlay->flashNum = 4;
}

bool Enemy::Update(Uint64 time)
{
    if (!spiritNode_->spirit->rendering)
        return false;


    if (spiritNode_->spirit->rect_.x < -spiritNode_->spirit->rect_.w)
    {
        Reset();
        return false;
    }


    if (++ChangeTrackCount == ChangeTrackNum) {
        if ((TrackPos != 0) & ((type & isSharp) == isSharp))
        {
                spiritNode_->SetPosition(spiritNode_->spirit->rect_.x, EnemyAndBuffetManager::possiableYpos[--TrackPos] - height);
        }

        if ((TrackPos != EnemyAndBuffetManager::possiableYpos.size() - 1) & ((type & isFlat) == isFlat))
        {
            spiritNode_->SetPosition(spiritNode_->spirit->rect_.x, EnemyAndBuffetManager::possiableYpos[++TrackPos] - height);

        }
        ChangeTrackCount = 0;
    }
    if (life_ <= 0)
    {
        auto xPos = spiritNode_->spirit->rect_.x;
        auto yPos = spiritNode_->spirit->rect_.y;
        animePlay->flashStart = true;
        if (animePlay->PlayAnime())
            Reset();


    }
    return true;
}

void Enemy::TakeDamage(int damage)
{
    life_ -= damage;
}

bool Enemy::GenerateEnemy(float x, float y, float xSpeed, float ySpeed)
{
    active = true;
    spiritNode_->Active(true);
    life_ = maxLife;
    spiritNode_->movingComponent.ChangeVelocity(xSpeed, ySpeed);
    spiritNode_->SetPosition(x, y);
    return active;

}

std::shared_ptr<Enemy> Enemy::GetPtr(std::shared_ptr<GameObject> enemy)
{
    return std::dynamic_pointer_cast<Enemy>(enemy);
}

std::string Enemy::EnemyName(int i)
{
    return "enemy" + std::to_string(i);
}

void Enemy::Reset()
{
    type = isEnemy;
    spiritNode_->Active(false);
    spiritNode_->ClearChild();
    active = false;
    animePlay->Reset();
}
