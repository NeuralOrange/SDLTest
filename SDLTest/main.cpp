#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>
#include <vector>
#include <string>
#include "SpiritNode.h"
#include "Spirit.h"
#include "MovingSpirit.h"
#include "SpiritManager.h"
#include "GameObject.h"
#include "Enemy.h"

#define STEP_RATE_IN_MILLISECONDS  10
#define SDL_WINDOW_WIDTH           1000
#define SDL_WINDOW_HEIGHT          625

static SDL_Joystick* joystick = NULL;//手柄十字键控制

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint64 last_step;
	SpiritManager* spiritManager;
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
		as->spiritManager->FlashSpirit("uika");
		auto enemy = as->spiritManager->GetGameObject("enemy");
		std::shared_ptr<Enemy> enemyPtr = Enemy::GetEnemyPtr(enemy);
		enemyPtr->Update(now-as->last_step);
		if (GameObject::CheckCollision(as->spiritManager->GetGameObject("uika"), enemy))
		    enemyPtr->TakeDamage(2);
        as->last_step = now;
    }
    SDL_RenderClear(as->renderer);

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

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    AppState* as = (AppState*)SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_APP_FAILURE;
    }

    *appstate = as;

    if (!SDL_CreateWindowAndRenderer("examples/demo/snake", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, 0, &as->window, &as->renderer)) {
        return SDL_APP_FAILURE;
    }
    
	as->spiritManager = new SpiritManager(as->renderer);
	as->spiritManager->CreateSpirit("resource/uika1.bmp", "uika", MOVE_SPIRIT_TYPE, 3,200,70);
    as->spiritManager->CreateSpirit("resource/mura.bmp", "background", SPIRIT_TYPE);
	as->spiritManager->CreateSpirit("resource/sakiko.bmp", "sakiko", SPIRIT_TYPE,2,0,0);
	as->spiritManager->GetSpirit("sakiko")->Scaling(0.6);
    //as->spiritManager->GetSpirit("uika")->AddChild(as->spiritManager->GetSpirit("sakiko"),"sakiko");
	std::shared_ptr<GameObject> enemy = std::make_shared<Enemy>("enemy", *as->spiritManager->GetSpirit("sakiko"), 100);
    std::shared_ptr<GameObject> uika = std::make_shared<Enemy>("uika", *as->spiritManager->GetSpirit("uika"), 200);
	as->spiritManager->AddGameObject(enemy);
	as->spiritManager->AddGameObject(uika);
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
        return handle_hat_event_(appstate,"uika", event->jhat.value);
    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        return handle_axis_event_(appstate,"uika", event->jaxis);
    case SDL_EVENT_KEY_DOWN:
        return handle_key_down_event_(appstate,"uika", event->key.scancode);
    case SDL_EVENT_KEY_UP:
        return handle_key_up_event_(appstate,"uika", event->key.scancode);
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
