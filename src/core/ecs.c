#include "core/ecs.h"
#include "biomes/grassland/grassland_enemies.h"
#include <math.h>

EntityMeta g_entities[MAX_ENTITIES];
Position   g_positions[MAX_ENTITIES];
Velocity   g_velocities[MAX_ENTITIES];
Health     g_health[MAX_ENTITIES];
Damage     g_damage[MAX_ENTITIES];
Lifetime   g_lifetime[MAX_ENTITIES];
EnemyType  g_enemy_types[MAX_ENTITIES];
float      g_enemy_timers[MAX_ENTITIES];

void ecs_init(void) {
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        g_entities[e].alive = false;
        g_entities[e].mask  = 0;

        g_positions[e].x = g_positions[e].y = 0.0f;
        g_velocities[e].vx = g_velocities[e].vy = 0.0f;

        g_health[e].hp = 0;
        g_health[e].max_hp = 0;

        g_damage[e].dmg = 0;
        g_lifetime[e].time_remaining = 0.0f;

        g_enemy_types[e] = ENEMY_TYPE_NONE;
        g_enemy_timers[e] = 0.0f;
    }
}

Entity ecs_create_entity(void) {
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!g_entities[e].alive) {
            g_entities[e].alive = true;
            g_entities[e].mask  = 0;

            // ✅ resets importants pour éviter les "mobs roses"
            g_enemy_types[e] = ENEMY_TYPE_NONE;
            g_enemy_timers[e] = 0.0f;
            g_health[e].hp = 0;
            g_health[e].max_hp = 0;

            return e;
        }
    }
    return INVALID_ENTITY;
}


void ecs_destroy_entity(Entity e) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].alive = false;
    g_entities[e].mask  = 0;
    // on ne nettoie pas tout, ce n'est pas strictement nécessaire
}

bool ecs_is_alive(Entity e) {
    if (e >= MAX_ENTITIES) return false;
    return g_entities[e].alive;
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

void ecs_add_health(Entity e, int hp) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].mask |= COMP_HEALTH;
    g_health[e].hp = hp;
    g_health[e].max_hp = hp;
}

void ecs_add_damage(Entity e, int dmg) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].mask |= COMP_DAMAGE;
    g_damage[e].dmg = dmg;
}

void ecs_add_projectile_tag(Entity e) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].mask |= COMP_PROJECTILE;
}

void ecs_add_lifetime(Entity e, float seconds) {
    if (e >= MAX_ENTITIES) return;
    g_entities[e].mask |= COMP_LIFETIME;
    g_lifetime[e].time_remaining = seconds;
}

int ecs_has_components(Entity e, ComponentMask mask) {
    if (e >= MAX_ENTITIES) return 0;
    if (!g_entities[e].alive) return 0;
    return ((g_entities[e].mask & mask) == mask);
}

void ecs_set_enemy_type(Entity e, EnemyType type) {
    if (e >= MAX_ENTITIES) return;
    g_enemy_types[e] = type;
}

EnemyType ecs_get_enemy_type(Entity e) {
    if (e >= MAX_ENTITIES) return ENEMY_TYPE_NONE;
    return g_enemy_types[e];
}

