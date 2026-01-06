#include "game/weapon.h"
#include <math.h>
#include <SDL2/SDL.h>

void weapon_init(Weapon *w, WeaponType type) {
    w->type = type;

    switch (type) {
        case WEAPON_THORN_SHOT:
            w->fire_rate = 0.45f;
            w->cooldown = 0.0f;
            w->damage = 1;
            break;

        default:
            w->fire_rate = 1.0f;
            w->cooldown = 0.0f;
            w->damage = 1;
            break;
    }
}

void system_weapon_fire(float dt, Entity player, Weapon *weapon, Engine *engine) {
    if (weapon->type == WEAPON_NONE)
        return;

    // cooldown
    weapon->cooldown -= dt;
    if (weapon->cooldown > 0.0f)
        return;

    // on récupère position joueur
    if (!ecs_has_components(player, COMP_POSITION))
        return;

    Position *pp = &g_positions[player];

    // coordonnée centre du joueur
    float px = pp->x;
    float py = pp->y;

    // position souris
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    // conversion souris écran -> monde
    float mouse_world_x = mx + engine->camera_x - engine->width  / 2.0f;
    float mouse_world_y = my + engine->camera_y - engine->height / 2.0f;

    float dx = mouse_world_x - px;
    float dy = mouse_world_y - py;

    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.001f)
        return;

    dx /= len;
    dy /= len;

    float speed = 480.0f;

    // créer projectile
    Entity proj = ecs_create_entity();
    ecs_add_position(proj, px, py);
    ecs_add_velocity(proj, dx * speed, dy * speed);
    ecs_add_projectile_tag(proj);
    ecs_add_damage(proj, weapon->damage);
    ecs_add_lifetime(proj, 1.2f);

    // reset cooldown
    weapon->cooldown = weapon->fire_rate;
}
