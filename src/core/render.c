#include "core/render_helpers.h"
#include <SDL2/SDL_ttf.h>

SDL_FPoint render_world_to_screen(const Engine *engine, float world_x, float world_y) {
    SDL_FPoint p;
    p.x = world_x - engine->camera_x + engine->width  / 2.0f;
    p.y = world_y - engine->camera_y + engine->height / 2.0f;
    return p;
}

void render_fill_rect_world(Engine *engine, float world_x, float world_y, float w, float h, SDL_Color color) {
    SDL_FPoint s = render_world_to_screen(engine, world_x, world_y);

    // culling basique
    if (s.x + w < 0 || s.x > engine->width)  return;
    if (s.y + h < 0 || s.y > engine->height) return;

    SDL_SetRenderDrawColor(engine->renderer, color.r, color.g, color.b, color.a);

    SDL_Rect rect;
    rect.x = (int)s.x;
    rect.y = (int)s.y;
    rect.w = (int)w;
    rect.h = (int)h;

    SDL_RenderFillRect(engine->renderer, &rect);
}

void render_text_center_top(Engine *engine, const char *text, SDL_Color color, int y) {
    if (!engine->font || !text) return;

    SDL_Surface *surf = TTF_RenderText_Blended(engine->font, text, color);
    if (!surf) return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(engine->renderer, surf);
    if (!tex) {
        SDL_FreeSurface(surf);
        return;
    }

    int tw, th;
    SDL_QueryTexture(tex, NULL, NULL, &tw, &th);

    SDL_Rect dst;
    dst.w = tw;
    dst.h = th;
    dst.x = (engine->width - tw) / 2;
    dst.y = y;

    SDL_RenderCopy(engine->renderer, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}
