#pragma once
#include "SpiritManager.h"
#include "AnimePlay.h"

#define STEP_RATE_IN_MILLISECONDS  10
#define SDL_WINDOW_WIDTH           1024
#define SDL_WINDOW_HEIGHT          768

class GameObject
{
public:
    GameObject(const std::string& name, SpiritNode* spiritNode) : name_(name), spiritNode_(spiritNode) 
    {
        animePlay = new AnimePlay(spiritNode_->spirit, spiritNode_->spirit->stateNum_, false);
    }
    ~GameObject() 
    {
        if (animePlay != nullptr)
            free(animePlay);
    }

    virtual bool Update(Uint64 time) = 0;

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
    AnimePlay* animePlay;
};


class Bullet : public GameObject
{
public:
    typedef enum BulletType
    {
        NONEBULLET,
        EighthRest,
        SixteenthRest
    }BulletType;

    int damage_ = 4;
    bool active = false;

    Bullet(const std::string& name, SpiritNode* spiritNode, BulletType bulletType = EighthRest) : GameObject(name, spiritNode)
    {
        spiritNode->movingComponent.ChangeVelocity(0, speed);
        spiritNode->spirit->rendering = false;
        type_ = bulletType;
        animePlay->isLoop = true;
        animePlay->flashNum = 10;
    }

    bool Update(Uint64 time) override
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

    static std::shared_ptr<Bullet> GetPtr(std::shared_ptr<GameObject> bullet)
    {
        return std::dynamic_pointer_cast<Bullet>(bullet);
    }

    void ShootBullet(float x, float y)
    {
        active = true;
        spiritNode_->SetPosition(x, y - spiritNode_->spirit->rect_.h / 2);
        spiritNode_->movingComponent.ChangeVelocity(0.2, 0);
        spiritNode_->spirit->rendering = true;
        animePlay->flashStart = true;
    }

    static std::string BulletName(int i,BulletType bulletType)
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

    void HitTarget()
    {
        spiritNode_->spirit->rendering = false;
        active = false;
        animePlay->Reset();
    }


private:
    Uint64 last_time = 0;
    float speed = 0.2;
    BulletType type_;
    unsigned flashCount = 0;
    unsigned flashNum = 4;
};

class Button : public GameObject
{
public:
    bool isHolding = false;
    Button(const std::string& name, SpiritNode* spiritNode, unsigned type = 0) : GameObject(name, spiritNode)
    {

    }

    bool Update(Uint64 time) override
    {
        if (!spiritNode_->spirit->rendering)
            return false;
        return true;
    }

    static std::shared_ptr<Button> GetPtr(std::shared_ptr<GameObject> button)
    {
        return std::dynamic_pointer_cast<Button>(button);
    }

    void Holding(unsigned state)
    {
        spiritNode_->spirit->state_ = state;
        isHolding = state;
    }

    bool PointIn(float x, float y)
    {
        SDL_FPoint point{ x,y };
        return SDL_PointInRectFloat(&point, &spiritNode_->spirit->rect_);
    }
private:
    Uint64 last_time = 0;
};



class Player :
    public GameObject
{
public:

    float shootX = 0;
    float shootY = 0;
    unsigned staminaTime = 0;
    bool staminaStart = false;


    Player(const std::string name, SpiritNode* spiritNode) :GameObject(name, spiritNode) 
    {
        if (animePlay != nullptr)
            animePlay = new AnimePlay(spiritNode->children[0]->spirit,flashTimes,false);
    }

    bool Update(Uint64 time) override
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

    std::string Attack()
    {
        if (staminaTime < flashTimes) 
        {
            staminaStart = false;
            animePlay->Reset();
            return "too fast";
        }
        float space = 120;
        shootX = spiritNode_->spirit->rect_.x + spiritNode_->children[0]->spirit->rect_.w +spiritNode_->spirit->rect_.w;
        shootY = spiritNode_->spirit->rect_.y + spiritNode_->spirit->rect_.h/2;
        staminaStart = false;
        animePlay->Reset();
        SDL_FRect* rect_ = &spiritNode_->spirit->rect_;
        auto widthUnit = (SDL_WINDOW_HEIGHT - space) / 6;
        for (int i = 1; i != 7; i++) 
        {
            if (shootY < widthUnit * i + space/2) 
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


    Bullet::BulletType GetBulletType()
    {
        if (staminaTime < flashTimes)
            return Bullet::NONEBULLET;
        else if (staminaTime < flashTimes*2)
            return Bullet::SixteenthRest;
        else
            return Bullet::EighthRest;

    }

private:
    std::vector<std::string> AudioName = { "C_Major", "D_Major", "E_Major", "F_Major", "G_Major", "A_Major", "B_Major"};
    unsigned flashTimes = 25;
};

class Background :
    public GameObject
{
public:
    Background(const std::string name, SpiritNode* spiritNode) :GameObject(name, spiritNode) 
    {
        spiritNode->movingComponent.ChangeVelocity(-0.1, 0);
    }

    bool Update(Uint64 time) override
    {

        if (!spiritNode_->spirit->rendering)
            return false;
        auto background = spiritNode_->spirit;
        if (background->rect_.x < -SDL_WINDOW_WIDTH)
            background->rect_.x = 0;

        return true;
    }

    static std::shared_ptr<Background> GetPtr(std::shared_ptr<GameObject> background)
    {
        return std::dynamic_pointer_cast<Background>(background);
    }
};

class Enemy : public GameObject
{
public:
    enum EnemyType {
        isSharp = 1 << 0,
        isFlat = 1<<1,
        isEighth = 1<<2,
        isSixteenth =1<<3,
        isFriend = 1<<4,
        isEnemy = 1<<5
    };
    bool active = false;
    int life_;
    float height;
    int type = isEighth | isEnemy;
    Enemy(const std::string& name, SpiritNode* spiritNode, int life) : GameObject(name, spiritNode), life_(life) 
    {
        maxLife = life;
        height = spiritNode->spirit->rect_.h;
        spiritNode->spirit->rendering = false;
        animePlay->flashNum = 4;
    }

    bool Update(Uint64 time) override
    {
        if (!spiritNode_->spirit->rendering)
            return false;


        if (spiritNode_->spirit->rect_.x < -spiritNode_->spirit->rect_.w)
        {
            spiritNode_->spirit->rendering = false;
            active = false;
            return false;
        }

        if (life_ <= 0)
        {
            auto xPos = spiritNode_->spirit->rect_.x;
            auto yPos = spiritNode_->spirit->rect_.y;
            animePlay->flashStart = true;
            if (animePlay->PlayAnime())
            {
                spiritNode_->spirit->rendering = false;
                active = false;
            }

        }
        return true;
    }

    void TakeDamage(int damage)
    {
       life_ -= damage;
    }

    bool GenerateEnemy(float x, float y,float xSpeed,float ySpeed)
    {
        active = true;
        spiritNode_->spirit->rendering = true;
        life_ = maxLife;
        spiritNode_->movingComponent.ChangeVelocity(xSpeed,ySpeed);
        spiritNode_->SetPosition(x, y);
        return active;

    }

    static std::shared_ptr<Enemy> GetPtr(std::shared_ptr<GameObject> enemy)
    {
        return std::dynamic_pointer_cast<Enemy>(enemy);
    }

    static std::string EnemyName(int i) 
    {
        return "enemy" + std::to_string(i);
    }


private:
    int maxLife;
    Uint64 last_time = 0;
};

