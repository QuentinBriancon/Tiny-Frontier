#ifndef CORE_ENGINE_H
#define CORE_ENGINE_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    int width;
    int height;
    bool running;
    float camera_x;
    float camera_y;
    TTF_Font *font;
} Engine;

// Initialise SDL, la fenêtre, le renderer, la font.
// Retourne true si OK, false si erreur.
bool engine_init(Engine *engine, const char *title, int width, int height);

// Gestion des events (fermeture, ESC, etc.)
void engine_handle_events(Engine *engine);

// Début d'un frame de rendu.
void engine_begin_render(Engine *engine);

// Fin d'un frame de rendu (present).
void engine_end_render(Engine *engine);

// Libération des ressources.
void engine_shutdown(Engine *engine);

#endif // CORE_ENGINE_H
