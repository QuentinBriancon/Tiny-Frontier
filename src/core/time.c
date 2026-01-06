#include "core/time.h"
#include <SDL2/SDL.h>

static uint64_t s_now = 0;
static uint64_t s_last = 0;
static double   s_freq = 0.0;

static float s_dt = 0.0f;
static float s_seconds = 0.0f;

void time_init(void) {
    s_now  = SDL_GetPerformanceCounter();
    s_last = s_now;
    s_freq = (double)SDL_GetPerformanceFrequency();
    s_dt = 0.0f;
    s_seconds = 0.0f;
}

void time_update(void) {
    s_last = s_now;
    s_now  = SDL_GetPerformanceCounter();

    s_dt = (float)((s_now - s_last) / s_freq);
    if (s_dt < 0.0f) s_dt = 0.0f;

    s_seconds += s_dt;
}

float time_get_dt(void) {
    return s_dt;
}

float time_get_seconds(void) {
    return s_seconds;
}
