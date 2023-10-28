#include <stdlib.h>
#include <SDL2/SDL.h>
#include "constants.h"

int GAME_PAUSED = FALSE;
static uint32_t pause_time = 0;
static uint32_t time_paused = 0;

void pause() {
    pause_time = SDL_GetTicks();
    GAME_PAUSED = TRUE;
}

void unpause() {
    time_paused += SDL_GetTicks() - pause_time;
    GAME_PAUSED = FALSE;
}

float get_delta_time()
{
    static int last_frame_time = 0;

    float delta_time = (SDL_GetTicks() - last_frame_time - time_paused) / 1000.0f;
    
    last_frame_time = SDL_GetTicks();
    time_paused = 0;

    return delta_time;
}