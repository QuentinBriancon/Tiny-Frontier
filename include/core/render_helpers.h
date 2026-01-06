#ifndef CORE_RENDER_HELPERS_H
#define CORE_RENDER_HELPERS_H

#include <SDL2/SDL.h>
#include "core/engine.h"

// Monde -> écran (utilise la camera + taille écran)
SDL_FPoint render_world_to_screen(const Engine *engine, float world_x, float world_y);

// Dessine un rect "monde" (coords monde), en appliquant camera
void render_fill_rect_world(Engine *engine, float world_x, float world_y, float w, float h, SDL_Color color);

// Dessine un texte centré en haut (ex: timer)
void render_text_center_top(Engine *engine, const char *text, SDL_Color color, int y);

#endif // CORE_RENDER_HELPERS_H
