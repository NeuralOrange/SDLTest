#pragma once
#include "SpiritManager.h"

#define STEP_RATE_IN_MILLISECONDS  10
#define SDL_WINDOW_WIDTH           1024
#define SDL_WINDOW_HEIGHT          768

class GameObject
{
public:
    GameObject(const std::string& name, SpiritNode* spiritNode) : name_(name), spiritNode_(spiritNode) {}
    virtual ~GameObject() = default;

    virtual void Update(Uint64 time) = 0;

    const std::string& GetName() const { return name_; }
    SpiritNode* GetSpiritNode() { return spiritNode_; }

    static bool CheckCollision(std::shared_ptr<GameObject> obj1,std::shared_ptr<GameObject> obj2)
    {
        if(!obj1->spiritNode_->spirit->rendering||!obj2->spiritNode_->spirit->rendering)
			return false;
        const SDL_FRect rect1 = obj1->spiritNode_->spirit->rect_;
        const SDL_FRect& rect2 = obj2->spiritNode_->spirit->rect_;

        return SDL_HasRectIntersectionFloat(&rect1, &rect2);
    }

    void MotionLimitations(float x, float y, float w, float h) 
    {
        SDL_FRect rect = spiritNode_->spirit->rect_;
        if (rect.x < x)
            spiritNode_->SetPosition(x, rect.y);
        else if (rect.x > x + w - rect.w)
            spiritNode_->SetPosition(x + w - rect.w,rect.y);
        rect = spiritNode_->spirit->rect_;
        if (rect.y < y)
            spiritNode_->SetPosition(rect.x, y);
        else if (rect.y > y + h - rect.h)
            spiritNode_->SetPosition(rect.x, y + h - rect.h);
    }


protected:
    std::string name_;
    SpiritNode* spiritNode_;
};


class Player :
    public GameObject
{
public:

    bool isAttack = false;
    int bulletCount = -1;
    float shootX = 0;
    float shootY = 0;

    Player(const std::string name, SpiritNode* spiritNode) :GameObject(name, spiritNode) {}

    void Update(Uint64 time) override
    {
        if (!spiritNode_->spirit->rendering)
            return;
        MotionLimitations(0, 0, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT);
        if (isAttack)
        {
            auto& attackEffect = spiritNode_->children[0]->spirit;
            if (!attackEffect->rendering)
            {
                attackEffect->rendering = true;
            }
            else
            {
                if (attackEffect->state_ == attackEffect->stateNum_ - 1)
                {
                    attackEffect->rendering = false;
                    attackEffect->state_ = 0;
                    isAttack = false;
                }
                else
                    attackEffect->FlashState();

            }
        }

    }

    std::string Attack()
    {
        if (isAttack)
            return "too fast";
        if (bulletCount < bulletNum-1)
            bulletCount++;
        else
            bulletCount = 0;
        auto CenterX = spiritNode_->spirit->rect_.x + spiritNode_->spirit->rect_.w / 2;
        shootX = CenterX;
        shootY = spiritNode_->spirit->rect_.y - spiritNode_->children[0]->spirit->rect_.h / 2;
        isAttack = true;
        SDL_FRect* rect_ = &spiritNode_->spirit->rect_;
        auto widthUnit = SDL_WINDOW_WIDTH / 8;
        for (int i = 1; i != 9; i++) 
        {
            if (CenterX < widthUnit * i) 
            {
                return AudioName[i - 1];
            }
        }
        return "";

    }

    static std::shared_ptr<Player> GetPtr(std::shared_ptr<GameObject> player)
    {
        return std::dynamic_pointer_cast<Player>(player);
    }


private:
    std::vector<std::string> AudioName = { "C_Major", "C_Dom7", "D_Major", "E_Major", "F_Major", "G_Major", "A_Major", "B_Major" };
    int bulletNum = 20;
};

class Background :
    public GameObject
{
public:
    Background(const std::string name, SpiritNode* spiritNode) :GameObject(name, spiritNode) 
    {
        spiritNode->movingComponent.ChangeVelocity(0, 0.1);
    }

    void Update(Uint64 time) override
    {

        if (!spiritNode_->spirit->rendering)
            return;
        auto background = spiritNode_->spirit;
        if (background->rect_.y >  0)
            background->rect_.y = -SDL_WINDOW_HEIGHT;

    }

    static std::shared_ptr<Background> GetPtr(std::shared_ptr<GameObject> background)
    {
        return std::dynamic_pointer_cast<Background>(background);
    }
};

class Enemy : public GameObject
{
public:
    Enemy(const std::string& name, SpiritNode* spiritNode, int life) : GameObject(name, spiritNode), life_(life) {}

    void Update(Uint64 time) override
    {
        if (!spiritNode_->spirit->rendering)
            return;
        if (life_ <= 0)
        {
            if (spiritNode_->spirit->stateNum_ == 1)
                spiritNode_->spirit->rendering = false;
            else
            {
                if (last_time == 0)
                {
                    spiritNode_->spirit->state_++;
                    last_time = 1;

                }
                else if (last_time < 1000)
                {
                    last_time += time;
                }
                else if (last_time >= 1000)
                {
                    spiritNode_->spirit->rendering = false;
                }
            }
        }
    }

    void TakeDamage(int damage)
    {
        life_ -= damage;
    }

    static std::shared_ptr<Enemy> GetPtr(std::shared_ptr<GameObject> enemy)
    {
        return std::dynamic_pointer_cast<Enemy>(enemy);
    }

private:
    int life_;
    Uint64 last_time = 0;
};

class Bullet : public GameObject
{
public:
    Bullet(const std::string& name, SpiritNode* spiritNode,unsigned type = 0) : GameObject(name, spiritNode)
    {
        spiritNode->movingComponent.ChangeVelocity(0, speed);
        spiritNode->spirit->rendering = false;
        type_ = type;
        spiritNode->spirit->state_ = type*2;
    }

    void Update(Uint64 time) override
    {
        if (!spiritNode_->spirit->rendering)
            return;

        auto xPos = spiritNode_->spirit->rect_.x;
        auto yPos = spiritNode_->spirit->rect_.y;

        if (yPos < -spiritNode_->spirit->rect_.h) {
            spiritNode_->spirit->rendering = false;
            return;
        }

		unsigned& state = spiritNode_->spirit->state_;
        if (flashCount++ == flashNum) {
            if (type_ == 0)
            {
                if (state++ == 1)
                    state = 0;
            }
            else if (state++ == 3)
                state = 2;
            flashCount = 0;
        }

    }

    static std::shared_ptr<Bullet> GetPtr(std::shared_ptr<GameObject> bullet)
    {
        return std::dynamic_pointer_cast<Bullet>(bullet);
    }

    void ShootBullet(float x,float y) 
    {
        spiritNode_->SetPosition(x-spiritNode_->spirit->rect_.w/2, y - spiritNode_->spirit->rect_.h);
        spiritNode_->movingComponent.ChangeVelocity(0,-0.2);
        spiritNode_->spirit->rendering = true;
    }

    static std::string BulletName(int i) 
    {
        return "bullet" + std::to_string(i);
    }

private:
    Uint64 last_time = 0;
    float speed = 0.2;
    unsigned type_ = 0;
    unsigned flashCount = 0;
    unsigned flashNum = 4;
};
