#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>
#include "Spirit.h"
#include "MovingSpirit.h"

#define STEP_RATE_IN_MILLISECONDS  10
#define SDL_WINDOW_WIDTH           1000
#define SDL_WINDOW_HEIGHT          625

static SDL_Joystick* joystick = NULL;//�ֱ�ʮ�ּ�����

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint64 last_step;
    MovingSpirit* spirit;
    Spirit* background;
} AppState;


/// <summary>
/// ���������¼�
/// </summary>
/// <param name="conSpir">�ɿ�Spirit</param>
/// <param name="key_code">��������</param>
/// <returns></returns>
static SDL_AppResult handle_key_down_event_(MovingSpirit* conSpir, SDL_Scancode key_code)
{
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
        conSpir->ChangeVelocity(DIR_RIGHT);
        break;
    case SDL_SCANCODE_UP:
        conSpir->ChangeVelocity(DIR_UP);
        break;
    case SDL_SCANCODE_LEFT:
        conSpir->ChangeVelocity(DIR_LEFT);
        break;
    case SDL_SCANCODE_DOWN:
        conSpir->ChangeVelocity(DIR_DOWN);
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

static SDL_AppResult handle_key_up_event_(MovingSpirit* conSpir, SDL_Scancode key_code)
{
    const bool* state = SDL_GetKeyboardState(NULL);
    switch (key_code) {
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_LEFT:
        if (!state[SDL_SCANCODE_RIGHT]&&!state[SDL_SCANCODE_LEFT])
        {
            conSpir->xVelocity = 0;
        }
        break;
    case SDL_SCANCODE_UP:
    case SDL_SCANCODE_DOWN:
        if (!state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN])
        {
            conSpir->yVelocity = 0;
        }
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

static SDL_AppResult handle_axis_event_(MovingSpirit* conSpir, SDL_JoyAxisEvent event) {

    if (event.axis == SDL_GAMEPAD_AXIS_LEFTX) {
        conSpir->ChangePosition( event.value / 3000,0);
    }
    else if (event.axis == SDL_GAMEPAD_AXIS_LEFTY) {
        conSpir->ChangePosition(0, event.value / 3000);
        // ��ҡ�˵� Y ���ƶ�
    }
    return SDL_APP_CONTINUE;
}

/// <summary>
/// �ֱ�ʮ�ּ�����
/// </summary>
/// <param name="conSpir">�ɿ�Spirit</param>
/// <param name="hat">�ֱ�ʮ�ּ�����</param>
/// <returns></returns>
static SDL_AppResult handle_hat_event_(MovingSpirit* conSpir, Uint8 hat) {
    switch (hat) {
    case SDL_HAT_RIGHT:
        conSpir->ChangeVelocity(DIR_RIGHT);
        break;
    case SDL_HAT_UP:
        conSpir->ChangeVelocity(DIR_UP);
        break;
    case SDL_HAT_LEFT:
        conSpir->ChangeVelocity(DIR_LEFT);
        break;
    case SDL_HAT_DOWN:
        conSpir->ChangeVelocity(DIR_DOWN);
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

/// <summary>
/// ����ѭ���з������ã����ڴ�����Ϸ�߼�����Ⱦ�� 
/// </summary>
/// <param name="appstate">����״̬</param>
/// <returns></returns>
SDL_AppResult SDL_AppIterate(void* appstate)
{
    AppState* as = (AppState*)appstate;
    const Uint64 now = SDL_GetTicks();
    //��Ϸ�߼�ѭ��
    while ((now - as->last_step) >= STEP_RATE_IN_MILLISECONDS) {
        as->spirit->MoveBySpeed(now - as->last_step);
        as->spirit->FlashState();
        as->last_step = now;
    }
    SDL_RenderClear(as->renderer);
    //ͼ����Ⱦ
    as->background->Draw(0,0);
    as->spirit->Draw();
    SDL_RenderPresent(as->renderer);
    return SDL_APP_CONTINUE;
}

/// <summary>
/// �����ʼ��ʱ���ã�����Ӧ��״̬���������ں���Ⱦ����
/// </summary>
/// <param name="appstate">����״̬</param>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns>����SDL_APP_CONTINUE�����ʼ���ɹ�</returns>
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
    as->spirit = new MovingSpirit(Spirit("resource/uika1.bmp", as->renderer,3), 0, 0);
    as->background = new Spirit("resource/mura.bmp", as->renderer);
    return SDL_APP_CONTINUE;
}


/// <summary>
/// ���ڴ����¼�������������롢�˳��ȣ���
/// </summary>
/// <param name="appstate">����״̬</param>
/// <param name="event">�¼�</param>
/// <returns></returns>
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    MovingSpirit* spirit = ((AppState*)appstate)->spirit;
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
        return handle_axis_event_(spirit, event->jaxis);
    case SDL_EVENT_KEY_DOWN:
        return handle_key_down_event_(spirit, event->key.scancode);
    case SDL_EVENT_KEY_UP:
        return handle_key_up_event_(spirit, event->key.scancode);
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}


/// <summary>
/// �����˳�ǰ���ã�������������
/// </summary>
/// <param name="appstate">����״̬</param>
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
