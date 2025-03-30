#pragma once
#include "SpiritManager.h"
#include "AnimePlay.h"

#define STEP_RATE_IN_MILLISECONDS  10
#define SDL_WINDOW_WIDTH           1024
#define SDL_WINDOW_HEIGHT          768


class GameObject
{
public:
    GameObject(const std::string& name, SpiritNode* spiritNode);
    ~GameObject();

    virtual bool Update(Uint64 time) = 0;

    const std::string& GetName() const;
    SpiritNode* GetSpiritNode();

    static bool CheckCollision(std::shared_ptr<GameObject> obj1,std::shared_ptr<GameObject> obj2);

    void MotionLimitations(float x, float y, float w, float h);


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

    Bullet(const std::string& name, SpiritNode* spiritNode, BulletType bulletType = EighthRest);

    bool Update(Uint64 time) override;

    static std::shared_ptr<Bullet> GetPtr(std::shared_ptr<GameObject> bullet);

    void ShootBullet(float x, float y);

    static std::string BulletName(int i,BulletType bulletType);

    void HitTarget();


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
    Button(const std::string& name, SpiritNode* spiritNode, unsigned type = 0);

    bool Update(Uint64 time) override;

    static std::shared_ptr<Button> GetPtr(std::shared_ptr<GameObject> button);

    void Holding(unsigned state);

    bool PointIn(float x, float y);
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


    Player(const std::string name, SpiritNode* spiritNode);

    bool Update(Uint64 time) override;

    std::string Attack();

    static std::shared_ptr<Player> GetPtr(std::shared_ptr<GameObject> player);


    Bullet::BulletType GetBulletType();

private:
    std::vector<std::string> AudioName = { "C_Major", "D_Major", "E_Major", "F_Major", "G_Major", "A_Major", "B_Major"};
    unsigned flashTimes = 25;
};

class Background :
    public GameObject
{
public:
    Background(const std::string name, SpiritNode* spiritNode);

    bool Update(Uint64 time) override;

    static std::shared_ptr<Background> GetPtr(std::shared_ptr<GameObject> background);
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
    float TrackPos = 0;
    Enemy(const std::string& name, SpiritNode* spiritNode, int life);

    bool Update(Uint64 time) override;

    void TakeDamage(int damage);

    bool GenerateEnemy(float x, float y,float xSpeed,float ySpeed);

    static std::shared_ptr<Enemy> GetPtr(std::shared_ptr<GameObject> enemy);

    static std::string EnemyName(int i);

    void Reset();


private:
    int maxLife;
    Uint64 last_time = 0;
    unsigned ChangeTrackCount = 0;
    unsigned ChangeTrackNum = 80;
};

