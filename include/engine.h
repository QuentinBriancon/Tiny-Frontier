#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
    int running;
    float camera_x;
    float camera_y;
    TTF_Font *font;
} Engine;


int  engine_init(Engine *engine, const char *title, int width, int height);
void engine_handle_events(Engine *engine);
void engine_begin_render(Engine *engine);
void engine_end_render(Engine *engine);
void engine_shutdown(Engine *engine);

#endif // ENGINE_H
