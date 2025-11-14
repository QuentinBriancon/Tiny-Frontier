#include "ecs.h"
#include "enemy.h"
#include <stddef.h>
#include <math.h>

EntityMeta g_entities[MAX_ENTITIES];
Position   g_positions[MAX_ENTITIES];
Velocity   g_velocities[MAX_ENTITIES];
Health   g_health[MAX_ENTITIES];
Damage   g_damage[MAX_ENTITIES];
Lifetime g_lifetime[MAX_ENTITIES];
EnemyType g_enemy_types[MAX_ENTITIES];
float g_enemy_timers[MAX_ENTITIES];

void ecs_init(void) {
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        g_entities[e].alive = 0;
        g_entities[e].mask  = 0;

        // init composants
        g_positions[e].x = g_positions[e].y = 0.0f;
        g_velocities[e].vx = g_velocities[e].vy = 0.0f;
        // ...

        g_enemy_types[e] = ENEMY_TYPE_NONE;
        g_enemy_timers[e] = 0.0f;
    }
}


Entity ecs_create_entity(void) {
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!g_entities[e].alive) {
            g_entities[e].alive = 1;
            g_entities[e].mask  = 0;
            return e;
        }
    }
    // plus de place, pour l'instant on renvoie 0
    return (Entity)0;
}

void ecs_destroy_entity(Entity e) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].alive = 0;
    g_entities[e].mask  = 0;
}

void ecs_add_position(Entity e, float x, float y) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].mask |= COMP_POSITION;
    g_positions[e].x = x;
    g_positions[e].y = y;
}

void ecs_add_velocity(Entity e, float vx, float vy) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].mask |= COMP_VELOCITY;
    g_velocities[e].vx = vx;
    g_velocities[e].vy = vy;
}

void ecs_add_player_tag(Entity e) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].mask |= COMP_PLAYER;
}

void ecs_add_enemy_tag(Entity e) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].mask |= COMP_ENEMY;
}

int ecs_has_components(Entity e, ComponentMask mask) {
    if (e >= MAX_ENTITIES) return 0;
    if (!g_entities[e].alive) return 0;
    return ( (g_entities[e].mask & mask) == mask );
}

void ecs_add_health(Entity e, int hp) {
    g_entities[e].mask |= COMP_HEALTH;
    g_health[e].hp = hp;
    g_health[e].max_hp = hp;
}

void ecs_add_damage(Entity e, int dmg) {
    g_entities[e].mask |= COMP_DAMAGE;
    g_damage[e].dmg = dmg;
}

void ecs_add_projectile_tag(Entity e) {
    g_entities[e].mask |= COMP_PROJECTILE;
}

void ecs_add_lifetime(Entity e, float seconds) {
    g_entities[e].mask |= COMP_LIFETIME;
    g_lifetime[e].time_remaining = seconds;
}

void ecs_set_enemy_type(Entity e, EnemyType type) {
    if (e >= MAX_ENTITIES) return;
    g_enemy_types[e] = type;
}

EnemyType ecs_get_enemy_type(Entity e) {
    if (e >= MAX_ENTITIES) return ENEMY_TYPE_NONE;
    return g_enemy_types[e];
}

// ---- systèmes ----

void system_movement(float dt) {
    ComponentMask needed = COMP_POSITION | COMP_VELOCITY;

    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!g_entities[e].alive) continue;
        if (!ecs_has_components(e, needed)) continue;

        Position *p = &g_positions[e];
        Velocity *v = &g_velocities[e];

        p->x += v->vx * dt;
        p->y += v->vy * dt;
    }
}

// IA ennemie : ils se dirigent vers le joueur
void system_enemy_ai(float dt, Entity player) {
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
        const EnemyArchetype *arch = get_enemy_archetype(type);
        if (!arch) {
            // fallback : simple chase
            float speed = 80.0f;
            v->vx = dx * speed;
            v->vy = dy * speed;
            continue;
        }

        float speed = arch->base_speed;

        switch (type) {
            case ENEMY_TYPE_APHIDLING:
                // Puceron : suit simplement le joueur
                v->vx = dx * speed;
                v->vy = dy * speed;
                break;

            case ENEMY_TYPE_MITE: {
                // Mite : zigzag rapide vers le joueur
                // timer spécifique à l'ennemi
                g_enemy_timers[e] += dt;

                // vecteur perpendiculaire
                float perp_x = -dy;
                float perp_y = dx;

                float zigzag_strength = 0.5f;     // amplitude
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
                // Leafhopper : marche normale + dash périodique
                g_enemy_timers[e] += dt;

                // cycle de 2 secondes : 1.2s marche, 0.4s dash, 0.4s "ralenti"
                float cycle = fmodf(g_enemy_timers[e], 2.0f);
                float factor;
                if (cycle < 1.2f) {
                    factor = 1.0f;      // marche
                } else if (cycle < 1.6f) {
                    factor = 3.0f;      // dash
                } else {
                    factor = 0.5f;      // ralentit un peu
                }

                v->vx = dx * speed * factor;
                v->vy = dy * speed * factor;
                break;
            }

            case ENEMY_TYPE_MINI_BEETLE: {
                // Mini-Beetle : tank lent + léger zigzag
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
                // Ant Scout : vite, contourne le joueur (trajectoire courbe)
                g_enemy_timers[e] += dt;

                // direction vers le joueur
                float dir_x = dx;
                float dir_y = dy;

                // vecteur perpendiculaire
                float perp_x = -dy;
                float perp_y = dx;

                // on ajoute un léger "arc" en fonction du temps
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
                // Horned Beetle : tank lent + charges périodiques
                g_enemy_timers[e] += dt;

                // cycle simple : 0–1.2s marche lente, 1.2–1.6s charge, 1.6–2.0s pause
                float cycle = fmodf(g_enemy_timers[e], 2.0f);
                float factor;
                if (cycle < 1.2f) {
                    factor = 0.7f;   // marche lente
                } else if (cycle < 1.6f) {
                    factor = 3.0f;   // charge
                } else {
                    factor = 0.3f;   // ralenti
                }

                v->vx = dx * speed * factor;
                v->vy = dy * speed * factor;
                break;
            }

            default:
                // Ennemi non géré spécifiquement : chase simple
                v->vx = dx * speed;
                v->vy = dy * speed;
                break;
        }
    }
}



void system_projectiles(float dt) {
    ComponentMask needed = COMP_PROJECTILE | COMP_LIFETIME;

    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!ecs_has_components(e, needed)) continue;

        g_lifetime[e].time_remaining -= dt;
        if (g_lifetime[e].time_remaining <= 0.0f) {
            ecs_destroy_entity(e);
        }
    }
}


void system_collisions(void) {
    ComponentMask projMask = COMP_POSITION | COMP_DAMAGE | COMP_PROJECTILE;
    ComponentMask enemyMask = COMP_POSITION | COMP_HEALTH | COMP_ENEMY;

    for (Entity p = 0; p < MAX_ENTITIES; ++p) {
        if (!ecs_has_components(p, projMask)) continue;

        Position *pp = &g_positions[p];

        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (!ecs_has_components(e, enemyMask)) continue;

            Position *ep = &g_positions[e];

            float dx = ep->x - pp->x;
            float dy = ep->y - pp->y;
            float dist2 = dx*dx + dy*dy;

            if (dist2 < 20*20) { // collision simple
                g_health[e].hp -= g_damage[p].dmg;

                // détruire projectile
                ecs_destroy_entity(p);

                if (g_health[e].hp <= 0) {
                    ecs_destroy_entity(e);
                }
                break; // projectile détruit → on arrête
            }
        }
    }
}
