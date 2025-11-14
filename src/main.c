#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "engine.h"
#include "ecs.h"
#include "weapon.h"
#include "enemy.h" 

#define RUN_DURATION_SECONDS 1200.0f  // 20 minutes = 20 * 60

static void render_timer(Engine *engine, float run_time) {
    if (!engine->font) return;

    int total = (int)run_time;
    int minutes = total / 60;
    int seconds = total % 60;

    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", minutes, seconds);

    SDL_Color color = { 255, 255, 255, 255 };

    SDL_Surface *surf = TTF_RenderText_Blended(engine->font, buf, color);
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
    dst.x = (engine->width - tw) / 2;  // centré en haut
    dst.y = 10;                        // marge haut

    SDL_RenderCopy(engine->renderer, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}


// Renvoie le type d'ennemi à spawner en fonction du temps de run
static EnemyType pick_enemy_type_for_time(float run_time) {
    int r = rand() % 10;

    // 0–3 min : T1 only (Aphidling + Mite)
    if (run_time < 180.0f) {
        if (r < 7) return ENEMY_TYPE_APHIDLING; // 70%
        else       return ENEMY_TYPE_MITE;      // 30%
    }

    // 3–7 min : T1 majoritaire, T2 timides
    else if (run_time < 420.0f) {
        if (r < 4) return ENEMY_TYPE_APHIDLING;    // 40%
        if (r < 7) return ENEMY_TYPE_MITE;         // 30%
        if (r < 9) return ENEMY_TYPE_LEAFHOPPER;   // 20%
        return ENEMY_TYPE_MINI_BEETLE;             // 10%
    }

    // 7–10 min : T1 + T2 équilibrés
    else if (run_time < 600.0f) {
        if (r < 2) return ENEMY_TYPE_APHIDLING;    // 20%
        if (r < 4) return ENEMY_TYPE_MITE;         // 20%
        if (r < 7) return ENEMY_TYPE_LEAFHOPPER;   // 30%
        return ENEMY_TYPE_MINI_BEETLE;             // 30%
    }

    // 10–12 min : T2 renforcé (comme ton post-mini-boss 1 futur)
    else if (run_time < 720.0f) {
        if (r < 1) return ENEMY_TYPE_APHIDLING;    // 10%
        if (r < 2) return ENEMY_TYPE_MITE;         // 10%
        if (r < 6) return ENEMY_TYPE_LEAFHOPPER;   // 40%
        return ENEMY_TYPE_MINI_BEETLE;             // 40%
    }

    // 12–15 min : introduction T3 faible
    else if (run_time < 900.0f) {
        if (r < 2) return ENEMY_TYPE_APHIDLING;     // 20%
        if (r < 4) return ENEMY_TYPE_MITE;          // 20%
        if (r < 7) return ENEMY_TYPE_LEAFHOPPER;    // 30%
        if (r < 9) return ENEMY_TYPE_MINI_BEETLE;   // 20%
        // 1/10 T3
        if (rand() % 2 == 0) return ENEMY_TYPE_ANT_SCOUT;
        return ENEMY_TYPE_HORNED_BEETLE;
    }

    // 15–20 min : chaos T1/T2/T3
    else if (run_time < 1200.0f) {
        if (r < 1) return ENEMY_TYPE_APHIDLING;     // 10%
        if (r < 2) return ENEMY_TYPE_MITE;          // 10%
        if (r < 4) return ENEMY_TYPE_LEAFHOPPER;    // 20%
        if (r < 6) return ENEMY_TYPE_MINI_BEETLE;   // 20%
        if (r < 8) return ENEMY_TYPE_ANT_SCOUT;     // 20%
        return ENEMY_TYPE_HORNED_BEETLE;            // 20%
    }

    // 20+ : en attendant la vraie logique de boss, gros mix
    else {
        if (r < 1) return ENEMY_TYPE_APHIDLING;
        if (r < 2) return ENEMY_TYPE_MITE;
        if (r < 4) return ENEMY_TYPE_LEAFHOPPER;
        if (r < 6) return ENEMY_TYPE_MINI_BEETLE;
        if (r < 8) return ENEMY_TYPE_ANT_SCOUT;
        return ENEMY_TYPE_HORNED_BEETLE;
    }
}



static float get_enemy_spawn_interval(float run_time) {
    // 0–1 min : tranquille
    if (run_time < 60.0f) {
        return 1.5f;
    }
    // 1–3 min : un peu plus serré
    else if (run_time < 180.0f) {
        return 1.2f;
    }
    // 3–7 min : rythme moyen
    else if (run_time < 420.0f) {
        return 1.0f;
    }
    // 7–10 min : assez dense
    else if (run_time < 600.0f) {
        return 0.8f;
    }
    // 10+ : dense (en attendant mini-boss & cie)
    else {
        return 0.6f;
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    Engine engine;
    if (!engine_init(&engine, "Tiny Frontier", 1280, 720)) {
        return 1;
    }

    // init ECS
    ecs_init();

    // ---- Création du joueur ----
    Entity player = ecs_create_entity();
    float player_w = 32.0f;
    float player_h = 32.0f;

    // Position de départ du joueur : au centre de l'écran (en coordonnées monde)
    float start_x = engine.width  / 2.0f - player_w / 2.0f;
    float start_y = engine.height / 2.0f - player_h / 2.0f;

    ecs_add_position(player, start_x, start_y);
    ecs_add_velocity(player, 0.0f, 0.0f);
    ecs_add_player_tag(player);

    // Vitesse du joueur
    float player_speed = 300.0f;

    // Caméra centrée sur le joueur
    engine.camera_x = start_x + player_w / 2.0f;
    engine.camera_y = start_y + player_h / 2.0f;

    // ---- Arme du joueur : Thorn Shot ----
    Weapon playerWeapon;
    weapon_init(&playerWeapon, WEAPON_THORN_SHOT);

    // ---- Gestion du temps ----
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = now;
    double freq = (double)SDL_GetPerformanceFrequency();
    float run_time = 0.0f;

    // ---- Spawn d'ennemis ----
    float enemy_spawn_timer    = 0.0f;
    float enemy_spawn_interval = 1.5f; // valeur de départ, sera mise à jour par get_enemy_spawn_interval

    while (engine.running) {
        last = now;
        now  = SDL_GetPerformanceCounter();
        float dt = (float)((now - last) / freq);
        run_time += dt * 10.0f;

        // Gestion des événements (fermeture fenêtre, ESC...)
        engine_handle_events(&engine);

        // ---- Input clavier pour le joueur ----
        const Uint8 *keys = SDL_GetKeyboardState(NULL);

        float dx = 0.0f;
        float dy = 0.0f;

        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_Z]) dy -= 1.0f;
        if (keys[SDL_SCANCODE_S])                        dy += 1.0f;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_Q]) dx -= 1.0f;
        if (keys[SDL_SCANCODE_D])                        dx += 1.0f;

        float len = sqrtf(dx * dx + dy * dy);
        if (len > 0.0f) {
            dx /= len;
            dy /= len;
        }

        // Mise à jour de la Velocity du joueur dans l'ECS
        if (ecs_has_components(player, COMP_POSITION | COMP_VELOCITY | COMP_PLAYER)) {
            g_velocities[player].vx = dx * player_speed;
            g_velocities[player].vy = dy * player_speed;
        }

        enemy_spawn_interval = get_enemy_spawn_interval(run_time);

        // ---- Spawn d'ennemis périodique ----
        enemy_spawn_timer += dt;
        if (enemy_spawn_timer >= enemy_spawn_interval) {
            enemy_spawn_timer -= enemy_spawn_interval;

            Position *playerPosForSpawn = &g_positions[player];
            EnemyType t = pick_enemy_type_for_time(run_time);
            spawn_enemy_around_player(t, playerPosForSpawn);
        }


        // ---- IA ennemie (velocity vers le joueur) ----
        system_enemy_ai(dt, player);

        // ---- Tir du joueur (via Weapon system) ----
        system_weapon_fire(dt, player, &playerWeapon, &engine);

        // ---- Systèmes ECS génériques ----
        system_projectiles(dt);
        system_collisions();
        system_movement(dt);

        // ---- Mise à jour de la caméra (suit le joueur) ----
        if (ecs_has_components(player, COMP_POSITION)) {
            Position *p = &g_positions[player];

            // Pas de clamp du monde ici : le monde est "infini"
            engine.camera_x = p->x + player_w / 2.0f;
            engine.camera_y = p->y + player_h / 2.0f;
        }

        // ---- Rendu ----
        engine_begin_render(&engine);

        render_timer(&engine, run_time);

        // On dessine toutes les entités qui ont une Position
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (!g_entities[e].alive) continue;
            if (!ecs_has_components(e, COMP_POSITION)) continue;

            Position *p = &g_positions[e];

            int isPlayer = ecs_has_components(e, COMP_PLAYER);
            int isEnemy  = ecs_has_components(e, COMP_ENEMY);
            int isProj   = ecs_has_components(e, COMP_PROJECTILE);

            float w = 20.0f;
            float h = 20.0f;

            if (isPlayer) {
                w = player_w;
                h = player_h;
            } else if (isEnemy) {
                EnemyType t = g_enemy_types[e];
                if (t == ENEMY_TYPE_MITE) {
                    w = 16.0f;
                    h = 16.0f;
                } else if (t == ENEMY_TYPE_MINI_BEETLE) {
                    w = 28.0f;
                    h = 28.0f;
                } else if (t == ENEMY_TYPE_HORNED_BEETLE) {
                    w = 32.0f;
                    h = 32.0f;
                }else {
                    w = 24.0f;
                    h = 24.0f;
                }
            } else if (isProj) {
                w = 8.0f;
                h = 8.0f;
            }

            // Conversion monde -> écran
            float screen_x = p->x - engine.camera_x + engine.width  / 2.0f;
            float screen_y = p->y - engine.camera_y + engine.height / 2.0f;

            // Petit culling simple : on ignore ce qui est hors écran
            if (screen_x + w < 0 || screen_x > engine.width)  continue;
            if (screen_y + h < 0 || screen_y > engine.height) continue;

            SDL_Rect rect = {
                .x = (int)screen_x,
                .y = (int)screen_y,
                .w = (int)w,
                .h = (int)h
            };

            rect.y = (int)screen_y;

            if (isPlayer) {
                SDL_SetRenderDrawColor(engine.renderer, 0, 0, 255, 255);      // bleu
            } else if (isEnemy) {
                EnemyType t = g_enemy_types[e];
                switch (t) {
                    case ENEMY_TYPE_APHIDLING:
                        SDL_SetRenderDrawColor(engine.renderer, 255, 0, 0, 255);   // rouge
                        break;
                    case ENEMY_TYPE_MITE:
                        SDL_SetRenderDrawColor(engine.renderer, 255, 128, 0, 255); // orange
                        break;
                    case ENEMY_TYPE_LEAFHOPPER:
                        SDL_SetRenderDrawColor(engine.renderer, 0, 200, 0, 255);       // vert
                        break;
                    case ENEMY_TYPE_MINI_BEETLE:
                        SDL_SetRenderDrawColor(engine.renderer, 150, 75, 0, 255);      // marron
                        break;
                    case ENEMY_TYPE_ANT_SCOUT:
                        SDL_SetRenderDrawColor(engine.renderer, 0, 200, 200, 255);     // cyan
                        break;
                    case ENEMY_TYPE_HORNED_BEETLE:
                        SDL_SetRenderDrawColor(engine.renderer, 180, 0, 80, 255);      // bordeaux
                        break;
                    default:
                        SDL_SetRenderDrawColor(engine.renderer, 255, 0, 255, 255); // rose = inconnu
                        break;
                }
            } else if (isProj) {
                SDL_SetRenderDrawColor(engine.renderer, 255, 255, 0, 255);    // jaune
            } else {
                SDL_SetRenderDrawColor(engine.renderer, 0, 255, 0, 255);      // vert
            }

            SDL_RenderFillRect(engine.renderer, &rect);
        }

        engine_end_render(&engine);
    }

    engine_shutdown(&engine);
    return 0;
}
