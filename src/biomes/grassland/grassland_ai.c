#include <math.h>

#include "biomes/grassland/grassland_ai.h"
#include "biomes/grassland/grassland_enemies.h"
#include "core/ecs.h"

void grassland_enemy_ai(float dt, Entity player) {
    if (!ecs_has_components(player, COMP_POSITION)) {
        return;
    }

    Position *pp = &g_positions[player];
    float px = pp->x;
    float py = pp->y;

    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!g_entities[e].alive) continue;
        if (!ecs_has_components(e, COMP_ENEMY | COMP_POSITION | COMP_VELOCITY)) continue;

        Position *p = &g_positions[e];
        Velocity *v = &g_velocities[e];

        float dx = px - p->x;
        float dy = py - p->y;
        float len = sqrtf(dx*dx + dy*dy);
        if (len < 0.001f) {
            v->vx = v->vy = 0.0f;
            continue;
        }

        dx /= len;
        dy /= len;

        EnemyType type = g_enemy_types[e];
        const EnemyArchetype *arch = grassland_get_enemy_archetype(type);
        if (!arch) {
            float speed = 80.0f;
            v->vx = dx * speed;
            v->vy = dy * speed;
            continue;
        }

        float speed = arch->base_speed;

        switch (type) {
            case ENEMY_TYPE_APHIDLING:
                v->vx = dx * speed;
                v->vy = dy * speed;
                break;

            case ENEMY_TYPE_MITE: {
                g_enemy_timers[e] += dt;

                float perp_x = -dy;
                float perp_y = dx;

                float zigzag_strength = 0.5f;
                float zigzag = sinf(g_enemy_timers[e] * 8.0f) * zigzag_strength;

                float dir_x = dx + perp_x * zigzag;
                float dir_y = dy + perp_y * zigzag;

                float dlen = sqrtf(dir_x*dir_x + dir_y*dir_y);
                if (dlen > 0.001f) {
                    dir_x /= dlen;
                    dir_y /= dlen;
                }

                v->vx = dir_x * speed;
                v->vy = dir_y * speed;
                break;
            }

            case ENEMY_TYPE_LEAFHOPPER: {
                g_enemy_timers[e] += dt;

                float cycle = fmodf(g_enemy_timers[e], 2.0f);
                float factor;
                if (cycle < 1.2f) {
                    factor = 1.0f;
                } else if (cycle < 1.6f) {
                    factor = 3.0f;
                } else {
                    factor = 0.5f;
                }

                v->vx = dx * speed * factor;
                v->vy = dy * speed * factor;
                break;
            }

            case ENEMY_TYPE_MINI_BEETLE: {
                g_enemy_timers[e] += dt;

                float perp_x = -dy;
                float perp_y = dx;

                float zigzag_strength = 0.2f;
                float zigzag = sinf(g_enemy_timers[e] * 4.0f) * zigzag_strength;

                float dir_x = dx + perp_x * zigzag;
                float dir_y = dy + perp_y * zigzag;

                float dlen = sqrtf(dir_x*dir_x + dir_y*dir_y);
                if (dlen > 0.001f) {
                    dir_x /= dlen;
                    dir_y /= dlen;
                }

                v->vx = dir_x * speed;
                v->vy = dir_y * speed;
                break;
            }

            case ENEMY_TYPE_ANT_SCOUT: {
                g_enemy_timers[e] += dt;

                float dir_x = dx;
                float dir_y = dy;

                float perp_x = -dy;
                float perp_y = dx;

                float curve = sinf(g_enemy_timers[e] * 2.5f) * 0.6f;

                dir_x += perp_x * curve;
                dir_y += perp_y * curve;

                float dlen = sqrtf(dir_x*dir_x + dir_y*dir_y);
                if (dlen > 0.001f) {
                    dir_x /= dlen;
                    dir_y /= dlen;
                }

                v->vx = dir_x * speed;
                v->vy = dir_y * speed;
                break;
            }

            case ENEMY_TYPE_HORNED_BEETLE: {
                g_enemy_timers[e] += dt;

                float cycle = fmodf(g_enemy_timers[e], 2.0f);
                float factor;
                if (cycle < 1.2f) {
                    factor = 0.7f;
                } else if (cycle < 1.6f) {
                    factor = 3.0f;
                } else {
                    factor = 0.3f;
                }

                v->vx = dx * speed * factor;
                v->vy = dy * speed * factor;
                break;
            }

            default:
                v->vx = dx * speed;
                v->vy = dy * speed;
                break;
        }
    }
}
