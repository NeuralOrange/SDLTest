#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Spirit.h"
#include "ControllableSpirit.h"


#define STEP_RATE_IN_MILLISECONDS  125
#define SDL_WINDOW_WIDTH           1000
#define SDL_WINDOW_HEIGHT          625

static SDL_Joystick* joystick = NULL;//手柄十字键控制

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint64 last_step;
    ControllableSpirit* spirit;
} AppState;


/// <summary>
/// 键盘输入事件
/// </summary>
/// <param name="conSpir">可控Spirit</param>
/// <param name="key_code">键盘输入</param>
/// <returns></returns>
static SDL_AppResult handle_key_event_(ControllableSpirit* conSpir,SDL_Scancode key_code)
{
    switch (key_code) {
        /* Quit. */
    case SDL_SCANCODE_ESCAPE:
    case SDL_SCANCODE_Q:
        return SDL_APP_SUCCESS;
        /* Restart the game as if the program was launched. */
    case SDL_SCANCODE_R:
        conSpir->SetPosition(0,0);
        break;
        /* Decide new direction of the snake. */
    case SDL_SCANCODE_RIGHT:
        conSpir->Move(DIR_RIGHT, 5);
        break;
    case SDL_SCANCODE_UP:
        conSpir->Move(DIR_UP, 5);
        break;
    case SDL_SCANCODE_LEFT:
        conSpir->Move(DIR_LEFT, 5);
        break;
    case SDL_SCANCODE_DOWN:
        conSpir->Move(DIR_DOWN, 5);
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

static SDL_AppResult handle_ball_event_(ControllableSpirit* conSpir, SDL_JoyAxisEvent event) {
    
    if (event.axis == SDL_GAMEPAD_AXIS_LEFTX) {
        conSpir->ChangePosition(event.value/1000,0);
    }
    else if (event.axis == SDL_GAMEPAD_AXIS_LEFTY) {
        conSpir->ChangePosition(0, event.value / 1000);
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
static SDL_AppResult handle_hat_event_(ControllableSpirit* conSpir, Uint8 hat) {
    switch (hat) {
    case SDL_HAT_RIGHT:
        conSpir->Move(DIR_RIGHT, 5);
        break;
    case SDL_HAT_UP:
        conSpir->Move(DIR_UP, 5);
        break;
    case SDL_HAT_LEFT:
        conSpir->Move(DIR_LEFT, 5);
        break;
    case SDL_HAT_DOWN:
        conSpir->Move(DIR_DOWN, 5);
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
        as->last_step += STEP_RATE_IN_MILLISECONDS;
    }
    SDL_RenderClear(as->renderer);
    //图像渲染
    as->spirit->Draw();
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
    size_t i;

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

    as->last_step = SDL_GetTicks();
    as->spirit = new ControllableSpirit(Spirit("resource/uika.bmp", as->renderer),0,0);
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
    ControllableSpirit* spirit = ((AppState*)appstate)->spirit;
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
        return handle_hat_event_(spirit, event->jhat.value);
    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        return handle_ball_event_(spirit,event->jaxis);
    case SDL_EVENT_KEY_DOWN:
        return handle_key_event_(spirit, event->key.scancode);
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
