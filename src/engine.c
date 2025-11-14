#include "engine.h"
#include <stdio.h>

int engine_init(Engine *engine, const char *title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 0;
    }

    engine->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        0
    );
    if (!engine->window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    engine->renderer = SDL_CreateRenderer(engine->window, -1, SDL_RENDERER_ACCELERATED);
    if (!engine->renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(engine->window);
        SDL_Quit();
        return 0;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }

    engine->width  = width;
    engine->height = height;
    engine->running = 1;

    // INIT CAMERA
    engine->camera_x = 0.0f;
    engine->camera_y = 0.0f;

    engine->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 18);
    if (!engine->font) {
        fprintf(stderr, "TTF_OpenFont error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(engine->renderer);
        SDL_DestroyWindow(engine->window);
        TTF_Quit();
        SDL_Quit();
        return 0;
    }

    return 1;
}

void engine_handle_events(Engine *engine) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            engine->running = 0;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                engine->running = 0;
            }
        }
    }
}

void engine_begin_render(Engine *engine) {
    // fond noir
    SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 255);
    SDL_RenderClear(engine->renderer);
}

void engine_end_render(Engine *engine) {
    SDL_RenderPresent(engine->renderer);
}

void engine_shutdown(Engine *engine) {
    if (engine->font) {
        TTF_CloseFont(engine->font);
        engine->font = NULL;
    }

    if (engine->renderer) {
        SDL_DestroyRenderer(engine->renderer);
    }
    if (engine->window) {
        SDL_DestroyWindow(engine->window);
    }

    TTF_Quit();
    SDL_Quit();
}
