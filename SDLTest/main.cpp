#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Spirit.h"



#define STEP_RATE_IN_MILLISECONDS  125
#define SDL_WINDOW_WIDTH           1000
#define SDL_WINDOW_HEIGHT          625


typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint64 last_step;
} AppState;


static SDL_AppResult handle_key_event_(SDL_Scancode key_code)
{   
    return SDL_APP_CONTINUE;
}

/// <summary>
/// ����ѭ���з������ã����ڴ�����Ϸ�߼�����Ⱦ�� 
/// </summary>
/// <param name="appstate">����״̬</param>
/// <returns></returns>
SDL_AppResult SDL_AppIterate(void* appstate)
{
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

    Spirit* rubish = new Spirit("D:/picture/rubish.bmp", as->renderer);
    rubish->Draw(0, 0);
    SDL_RenderPresent(as->renderer);
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
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        return handle_key_event_(event->key.scancode);
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
