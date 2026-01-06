#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "game/game.h"
#include "game/weapon.h"
#include "core/ecs.h"
#include "core/render_helpers.h"
#include "core/engine.h"
#include "game/biome.h"
#include "core/systems.h"
#include "biomes/grassland/grassland_biome.h"

// 20 minutes = 20 * 60
#define RUN_DURATION_SECONDS 1200.0f

// État global du "run"
static Entity player = 0;
static float player_w = 32.0f;
static float player_h = 32.0f;
static float player_speed = 300.0f;

static Weapon playerWeapon;

static float run_time = 0.0f;  // temps de jeu (avec ton *10.0f)

static BiomeAPI s_current_biome;


void game_init(Engine *engine) {
    srand((unsigned int)time(NULL));

    ecs_init();

    // Joueur
    player = ecs_create_entity();

    float start_x = engine->width  / 2.0f - player_w / 2.0f;
    float start_y = engine->height / 2.0f - player_h / 2.0f;

    ecs_add_position(player, start_x, start_y);
    ecs_add_velocity(player, 0.0f, 0.0f);
    ecs_add_player_tag(player);
    ecs_add_health(player, 100);

    engine->camera_x = start_x + player_w / 2.0f;
    engine->camera_y = start_y + player_h / 2.0f;

    weapon_init(&playerWeapon, WEAPON_THORN_SHOT);

    run_time = 0.0f;

    // Pour l'instant : toujours le biome Grassland
    s_current_biome = grassland_biome_get_api();
    if (s_current_biome.init) {
        s_current_biome.init();
    }
}

void game_update(Engine *engine, float dt) {
    // tu avais un *10.0f, on le garde
    run_time += dt * 10.0f;

    // Input
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_Z]) dy -= 1.0f;
    if (keys[SDL_SCANCODE_S])                         dy += 1.0f;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_Q]) dx -= 1.0f;
    if (keys[SDL_SCANCODE_D])                         dx += 1.0f;

    float len = sqrtf(dx * dx + dy * dy);
    if (len > 0.0f) {
        dx /= len;
        dy /= len;
    }

    if (ecs_has_components(player, COMP_POSITION | COMP_VELOCITY | COMP_PLAYER)) {
        g_velocities[player].vx = dx * player_speed;
        g_velocities[player].vy = dy * player_speed;
    }

    // Biome : gère spawns, logique spécifique Grassland
    if (s_current_biome.update) {
        s_current_biome.update(dt, run_time, player);
    }

    // Tir du joueur
    system_weapon_fire(dt, player, &playerWeapon, engine);

    // Systèmes génériques
    system_projectiles(dt);
    system_collisions();
    system_movement(dt);
    system_enemy_player_contact(dt);


    // Caméra
    if (ecs_has_components(player, COMP_POSITION)) {
        Position *p = &g_positions[player];
        engine->camera_x = p->x + player_w / 2.0f;
        engine->camera_y = p->y + player_h / 2.0f;
    }
}

void game_render(Engine *engine) {
    // timer
    int total = (int)run_time;
    int minutes = total / 60;
    int seconds = total % 60;

    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", minutes, seconds);

    render_text_center_top(engine, buf, (SDL_Color){255,255,255,255}, 10);

    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!g_entities[e].alive) continue;
        if (!ecs_has_components(e, COMP_POSITION)) continue;

        Position *p = &g_positions[e];

        int isPlayer = ecs_has_components(e, COMP_PLAYER);
        int isEnemy  = ecs_has_components(e, COMP_ENEMY);
        int isProj   = ecs_has_components(e, COMP_PROJECTILE);

        float w = 20.0f, h = 20.0f;
        SDL_Color c = (SDL_Color){0,255,0,255};

        if (isPlayer) {
            w = player_w; h = player_h;
            c = (SDL_Color){0,0,255,255};
            // --- HP bar au-dessus du joueur ---
            if (ecs_has_components(e, COMP_HEALTH)) {
                float pct = 0.0f;
                if (g_health[e].max_hp > 0) {
                    pct = (float)g_health[e].hp / (float)g_health[e].max_hp;
                    if (pct < 0.0f) pct = 0.0f;
                    if (pct > 1.0f) pct = 1.0f;
                }

                float bar_w = w;
                float bar_h = 6.0f;

                // p->x / p->y = top-left du joueur (si render_fill_rect_world est top-left)
                float bar_x = p->x;
                float bar_y = p->y - 10.0f - bar_h;

                // fond
                render_fill_rect_world(engine, bar_x, bar_y, bar_w, bar_h, (SDL_Color){30,30,30,255});
                // fill (part de la gauche)
                render_fill_rect_world(engine, bar_x, bar_y, bar_w * pct, bar_h, (SDL_Color){0,220,0,255});
            }

        } else if (isProj) {
            w = 8.0f; h = 8.0f;
            c = (SDL_Color){255,255,0,255};
        } else if (isEnemy) {
            EnemyType t = g_enemy_types[e];
            if (t == ENEMY_TYPE_MITE) { w = 16; h = 16; }
            else if (t == ENEMY_TYPE_MINI_BEETLE) { w = 28; h = 28; }
            else if (t == ENEMY_TYPE_HORNED_BEETLE) { w = 32; h = 32; }
            else { w = 24; h = 24; }

            switch (t) {
                case ENEMY_TYPE_APHIDLING:     c = (SDL_Color){255,0,0,255}; break;
                case ENEMY_TYPE_MITE:          c = (SDL_Color){255,128,0,255}; break;
                case ENEMY_TYPE_LEAFHOPPER:    c = (SDL_Color){0,200,0,255}; break;
                case ENEMY_TYPE_MINI_BEETLE:   c = (SDL_Color){150,75,0,255}; break;
                case ENEMY_TYPE_ANT_SCOUT:     c = (SDL_Color){0,200,200,255}; break;
                case ENEMY_TYPE_HORNED_BEETLE: c = (SDL_Color){180,0,80,255}; break;
                case ENEMY_TYPE_BOSS_SHIELD_BEETLE: c = (SDL_Color){180,0,80,255}; break;
                default:                       c = (SDL_Color){255,0,255,255}; break;
            }
        }

        render_fill_rect_world(engine, p->x, p->y, w, h, c);
    }
}

void game_shutdown(void) {
    if (s_current_biome.shutdown) {
        s_current_biome.shutdown();
    }
}
