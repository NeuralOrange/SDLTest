#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <vector>
#include <string>
#include "SpiritNode.h"
#include "Spirit.h"
#include "MovingSpirit.h"
#include "SpiritManager.h"
#include "SDLAudioPlayer.h"
#include "GameObject.h"


#define STEP_RATE_IN_MILLISECONDS  10
#define SDL_WINDOW_WIDTH           1024
#define SDL_WINDOW_HEIGHT          768

const std::string ImgPath = "resource/img/";
const std::string MusicPath = "resource/music/";
static SDL_Joystick* joystick = NULL;//手柄十字键控制

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint64 last_step;
	SpiritManager* spiritManager;
	SDLAudioPlayer* audioPlayer;
} AppState;


 ///<summary>
 ///键盘输入事件
 ///</summary>
 ///<param name="conSpir">可控Spirit</param>
 ///<param name="key_code">键盘输入</param>
 ///<returns></returns>
static SDL_AppResult handle_key_down_event_(void* appstate,const std::string& name, SDL_Scancode key_code)
{
	AppState* as = (AppState*)appstate;
	SpiritNode* conSpir = as->spiritManager->GetSpirit(name);
    std::shared_ptr<Player> PlayerPtr = Player::GetPtr(as->spiritManager->GetGameObject(name));
    std::shared_ptr<Bullet> bullet;
    std::string attckResult;
	if (conSpir == nullptr||!conSpir->enableMovingComponent)
		return SDL_APP_CONTINUE;
    switch (key_code) {
        /* Quit. */
    case SDL_SCANCODE_ESCAPE:
    case SDL_SCANCODE_Q:
        return SDL_APP_SUCCESS;
        /* Restart the game as if the program was launched. */
    case SDL_SCANCODE_R:
        conSpir->SetPosition(0, 0);
        as->audioPlayer->Play("sin88");
        break;
    case SDL_SCANCODE_SPACE:
        attckResult = PlayerPtr->Attack();
        as->audioPlayer->Play(attckResult);
        bullet = Bullet::GetPtr(as->spiritManager->GetGameObject(Bullet::BulletName(PlayerPtr->bulletCount)));
        if(attckResult!="too fast")
            bullet->ShootBullet(PlayerPtr->shootX,PlayerPtr->shootY);
        //as->audioPlayer1->GenerateSineWave(SDLAudioPlayer::PianoKeyFrequency(30),1050);
        break;
    case SDL_SCANCODE_P:
        as->audioPlayer->Play("A_Major");
        break;

        /* Decide new direction of the snake. */
    case SDL_SCANCODE_RIGHT:
        conSpir->movingComponent.ChangeVelocity(DIR_RIGHT);
        break;
    case SDL_SCANCODE_UP:
        conSpir->movingComponent.ChangeVelocity(DIR_UP);
        break;
    case SDL_SCANCODE_LEFT:
        conSpir->movingComponent.ChangeVelocity(DIR_LEFT);
        break;
    case SDL_SCANCODE_DOWN:
        conSpir->movingComponent.ChangeVelocity(DIR_DOWN);
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

static SDL_AppResult handle_key_up_event_(void* appstate, const std::string& name, SDL_Scancode key_code)
{
	AppState* as = (AppState*)appstate;
	SpiritNode* conSpir = as->spiritManager->GetSpirit(name);
	if (conSpir == nullptr || !conSpir->enableMovingComponent)
		return SDL_APP_CONTINUE;
    const bool* state = SDL_GetKeyboardState(NULL);
    switch (key_code) {
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_LEFT:
        if (!state[SDL_SCANCODE_RIGHT]&&!state[SDL_SCANCODE_LEFT])
        {
            conSpir->movingComponent.xVelocity = 0;
        }
        break;
    case SDL_SCANCODE_UP:
    case SDL_SCANCODE_DOWN:
        if (!state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN])
        {
            conSpir->movingComponent.yVelocity = 0;
        }
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

static SDL_AppResult handle_axis_event_(void* appstate, const std::string& name, SDL_JoyAxisEvent event)
{
	AppState* as = (AppState*)appstate;
	SpiritNode* conSpir = as->spiritManager->GetSpirit(name);
	if (conSpir == nullptr || !conSpir->enableMovingComponent)
		return SDL_APP_CONTINUE;
    if (event.axis == SDL_GAMEPAD_AXIS_LEFTX) {
        conSpir->ChangePosition( event.value / 3000,0);
    }
    else if (event.axis == SDL_GAMEPAD_AXIS_LEFTY) {
        conSpir->ChangePosition(0, event.value / 3000);
        // 左摇杆的 Y 轴移动
    }
    return SDL_APP_CONTINUE;
}

/// <summary>
/// 手柄十字键控制
/// </summary>
/// <param name="conSpir">可控Spirit</param>
/// <param name="hat">手柄十字键输入</param>
/// <returns></returns>
static SDL_AppResult handle_hat_event_(void* appstate, const std::string& name, Uint8 hat)
{
	AppState* as = (AppState*)appstate;
	SpiritNode* conSpir = as->spiritManager->GetSpirit(name);
	if (conSpir == nullptr || !conSpir->enableMovingComponent)
		return SDL_APP_CONTINUE;
    switch (hat) {
    case SDL_HAT_RIGHT:
        conSpir->movingComponent.ChangeVelocity(DIR_RIGHT);
        break;
    case SDL_HAT_UP:
        conSpir->movingComponent.ChangeVelocity(DIR_UP);
        break;
    case SDL_HAT_LEFT:
        conSpir->movingComponent.ChangeVelocity(DIR_LEFT);
        break;
    case SDL_HAT_DOWN:
        conSpir->movingComponent.ChangeVelocity(DIR_DOWN);
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}



/// <summary>
/// 在主循环中反复调用，用于处理游戏逻辑和渲染。 
/// </summary>
/// <param name="appstate">程序状态</param>
/// <returns></returns>
SDL_AppResult SDL_AppIterate(void* appstate)
{
    AppState* as = (AppState*)appstate;
    const Uint64 now = SDL_GetTicks();
    //游戏逻辑循环
	while ((now - as->last_step) >= STEP_RATE_IN_MILLISECONDS) {

		as->spiritManager->MoveAllBySpeed(now - as->last_step);
        std::shared_ptr<Player> PlayerPtr = Player::GetPtr(as->spiritManager->GetGameObject("pinao"));
        std::shared_ptr<Background> BackgroundPtr = Background::GetPtr(as->spiritManager->GetGameObject("background"));
        BackgroundPtr->Update(now - as->last_step);
        std::shared_ptr<Bullet> bulletPtr;
		for (int i = 0; i != 20; i++) {
			bulletPtr = Bullet::GetPtr(as->spiritManager->GetGameObject(Bullet::BulletName(i)));
            bulletPtr->Update(now - as->last_step);
        }
        PlayerPtr->Update(now-as->last_step);
		//if (GameObject::CheckCollision(as->spiritManager->GetGameObject("uika"), enemy))
		//    enemyPtr->TakeDamage(2);
        as->last_step = now;
    }
    SDL_RenderClear(as->renderer);

    //SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    //SDL_RenderClear(as->renderer);  /* start with a blank canvas. */

    //SDL_SetRenderDrawColor(as->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  /* white, full alpha */
    //SDL_RenderDebugText(as->renderer, 272, 100, "Hello world!");

    //图像渲染
	as->spiritManager->DrawAll();




    SDL_RenderPresent(as->renderer);
    return SDL_APP_CONTINUE;
}



/// <summary>
/// 程序初始化时调用，设置应用状态并创建窗口和渲染器。
/// </summary>
/// <param name="appstate">程序状态</param>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns>返回SDL_APP_CONTINUE代表初始化成功</returns>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
 
    if (!SDL_SetAppMetadata("Example Snake game", "1.0", "com.example.Snake")) {
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    AppState* as = (AppState*)SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_APP_FAILURE;
    }

    *appstate = as;

    if (!SDL_CreateWindowAndRenderer("examples/demo/music", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, 0, &as->window, &as->renderer)) {
        return SDL_APP_FAILURE;
    }
    
	as->spiritManager = new SpiritManager(as->renderer);
	//as->spiritManager->CreateSpirit("resource/uika1.bmp", "uika", MOVE_SPIRIT_TYPE, 3,200,70);
	//as->spiritManager->CreateSpirit("resource/sa.png", "sa", SPIRIT_TYPE,1,0,0);
	as->audioPlayer = new SDLAudioPlayer();
	as->audioPlayer->LoadWAV("dark", "resource/music/dark.wav");
    as->audioPlayer->GenerateSineWave("sin44", 440, 1000);
    as->audioPlayer->GenerateSineWave("sin88", 880, 1000);

    as->audioPlayer->GenerateChord("A_Major", SpecialAudio::A_Major, 400);
    as->audioPlayer->GenerateChord("A_Minor", SpecialAudio::A_Minor, 400);
    as->audioPlayer->GenerateChord("B_Major", SpecialAudio::B_Major, 400);
    as->audioPlayer->GenerateChord("C_Dom7", SpecialAudio::C_Dom7, 400);
    as->audioPlayer->GenerateChord("C_Major", SpecialAudio::C_Major, 400);
    as->audioPlayer->GenerateChord("D_Major", SpecialAudio::D_Major, 400);
    as->audioPlayer->GenerateChord("D_Minor", SpecialAudio::D_Minor, 400);
    as->audioPlayer->GenerateChord("E_Major", SpecialAudio::E_Major, 400);
    as->audioPlayer->GenerateChord("E_Minor", SpecialAudio::E_Minor, 400);
    as->audioPlayer->GenerateChord("F_Major", SpecialAudio::F_Major, 400);
    as->audioPlayer->GenerateChord("G_Dom7", SpecialAudio::G_Dom7, 400);
    as->audioPlayer->GenerateChord("G_Major", SpecialAudio::G_Major, 400);

    as->spiritManager->CreateSpirit(ImgPath + "background.png", "background", MOVE_SPIRIT_TYPE, 1, 0, -768);
    as->spiritManager->GetSpirit("background")->Scaling(0.5);
    as->spiritManager->AddGameObject(std::make_shared<Background>("background", as->spiritManager->GetSpirit("background")));

    as->spiritManager->CreateSpirit(ImgPath+"pinao.png", "pinao", MOVE_SPIRIT_TYPE, 1, 100, 100);
    as->spiritManager->GetSpirit("pinao")->Scaling(0.2);
    as->spiritManager->AddGameObject(std::make_shared<Player>("pinao",as->spiritManager->GetSpirit("pinao")));

    as->spiritManager->CreateSpirit(ImgPath + "note.png", "note", SPIRIT_TYPE, 3, 100, 50);
    as->spiritManager->GetSpirit("note")->Scaling(0.2);
    as->spiritManager->GetSpirit("note")->spirit->rendering = false;
    as->spiritManager->GetSpirit("pinao")->AddChild(as->spiritManager->GetSpirit("note"), "note");

    as->spiritManager->CreateSpirit(ImgPath + "bullet.png", Bullet::BulletName(0), MOVE_SPIRIT_TYPE, 4, 0, 0);
    as->spiritManager->GetSpirit(Bullet::BulletName(0))->Scaling(0.3);
    as->spiritManager->AddGameObject(std::make_shared<Bullet>(Bullet::BulletName(0), as->spiritManager->GetSpirit(Bullet::BulletName(0))));
    auto bulletTexture = as->spiritManager->GetSpirit(Bullet::BulletName(0))->spirit->texture_;
    for (int i = 1; i != 20; i++) 
    {
        as->spiritManager->CreateSpirit(bulletTexture, Bullet::BulletName(i), MOVE_SPIRIT_TYPE, 4, 0, 0);
        as->spiritManager->GetSpirit(Bullet::BulletName(i))->Scaling(0.3);
        as->spiritManager->AddGameObject(std::make_shared<Bullet>(Bullet::BulletName(i), as->spiritManager->GetSpirit(Bullet::BulletName(i)),i%2));
    }

	//as->spiritManager->GetSpirit("sakiko")->Scaling(0.6);
    //as->spiritManager->GetSpirit("uika")->AddChild(as->spiritManager->GetSpirit("sakiko"),"sakiko");
	//std::shared_ptr<GameObject> enemy = std::make_shared<Enemy>("enemy", *as->spiritManager->GetSpirit("sakiko"), 100);
    //std::shared_ptr<GameObject> uika = std::make_shared<Enemy>("uika", *as->spiritManager->GetSpirit("uika"), 200);
	//as->spiritManager->AddGameObject(enemy);
	//as->spiritManager->AddGameObject(uika);
    SDL_AudioSpec spec;

    
    as->last_step = SDL_GetTicks();
    return SDL_APP_CONTINUE;
}



/// <summary>
/// 用于处理事件（例如键盘输入、退出等）。
/// </summary>
/// <param name="appstate">程序状态</param>
/// <param name="event">事件</param>
/// <returns></returns>
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_JOYSTICK_ADDED:
        if (joystick == NULL) {
            joystick = SDL_OpenJoystick(event->jdevice.which);
            if (!joystick) {
                SDL_Log("Failed to open joystick ID %u: %s", (unsigned int)event->jdevice.which, SDL_GetError());
            }
        }
        break;
    case SDL_EVENT_JOYSTICK_REMOVED:
        if (joystick && (SDL_GetJoystickID(joystick) == event->jdevice.which)) {
            SDL_CloseJoystick(joystick);
            joystick = NULL;
        }

        break;
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
        return handle_hat_event_(appstate,"pinao", event->jhat.value);
    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        return handle_axis_event_(appstate,"pinao", event->jaxis);
    case SDL_EVENT_KEY_DOWN:
        return handle_key_down_event_(appstate,"pinao", event->key.scancode);
    case SDL_EVENT_KEY_UP:
        return handle_key_up_event_(appstate,"pinao", event->key.scancode);
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}


/// <summary>
/// 程序退出前调用，进行清理工作。
/// </summary>
/// <param name="appstate">程序状态</param>
/// <param name="result"></param>
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (appstate != NULL) {
        AppState* as = (AppState*)appstate;
        SDL_DestroyRenderer(as->renderer);
        SDL_DestroyWindow(as->window);
        SDL_free(as);
    }
}
