#include "biomes/grassland/grassland_spawn.h"
#include "biomes/grassland/grassland_enemies.h"
#include "core/ecs.h"
#include <stdlib.h>

static float s_timer = 0.0f;
static float s_interval = 1.5f;
static GrasslandSpawnMode s_mode = GRASSLAND_SPAWN_MODE_NORMAL;

static EnemyType pick_enemy_type_normal(float t) {
    int r = rand() % 10;

    if (t < 180.0f) {
        if (r < 7) return ENEMY_TYPE_APHIDLING;
        return ENEMY_TYPE_MITE;
    } else if (t < 420.0f) {
        if (r < 4) return ENEMY_TYPE_APHIDLING;
        if (r < 7) return ENEMY_TYPE_MITE;
        if (r < 9) return ENEMY_TYPE_LEAFHOPPER;
        return ENEMY_TYPE_MINI_BEETLE;
    } else if (t < 600.0f) {
        if (r < 2) return ENEMY_TYPE_APHIDLING;
        if (r < 4) return ENEMY_TYPE_MITE;
        if (r < 7) return ENEMY_TYPE_LEAFHOPPER;
        return ENEMY_TYPE_MINI_BEETLE;
    } else if (t < 720.0f) {
        if (r < 1) return ENEMY_TYPE_APHIDLING;
        if (r < 2) return ENEMY_TYPE_MITE;
        if (r < 6) return ENEMY_TYPE_LEAFHOPPER;
        return ENEMY_TYPE_MINI_BEETLE;
    } else if (t < 900.0f) {
        if (r < 2) return ENEMY_TYPE_APHIDLING;
        if (r < 4) return ENEMY_TYPE_MITE;
        if (r < 7) return ENEMY_TYPE_LEAFHOPPER;
        if (r < 9) return ENEMY_TYPE_MINI_BEETLE;
        if (rand() % 2 == 0) return ENEMY_TYPE_ANT_SCOUT;
        return ENEMY_TYPE_HORNED_BEETLE;
    } else if (t < 1200.0f) {
        if (r < 1) return ENEMY_TYPE_APHIDLING;
        if (r < 2) return ENEMY_TYPE_MITE;
        if (r < 4) return ENEMY_TYPE_LEAFHOPPER;
        if (r < 6) return ENEMY_TYPE_MINI_BEETLE;
        if (r < 8) return ENEMY_TYPE_ANT_SCOUT;
        return ENEMY_TYPE_HORNED_BEETLE;
    } else {
        if (r < 1) return ENEMY_TYPE_APHIDLING;
        if (r < 2) return ENEMY_TYPE_MITE;
        if (r < 4) return ENEMY_TYPE_LEAFHOPPER;
        if (r < 6) return ENEMY_TYPE_MINI_BEETLE;
        if (r < 8) return ENEMY_TYPE_ANT_SCOUT;
        return ENEMY_TYPE_HORNED_BEETLE;
    }
}

static float get_spawn_interval(float t) {
    if (t < 60.0f) return 1.5f;
    if (t < 180.0f) return 1.2f;
    if (t < 420.0f) return 1.0f;
    if (t < 600.0f) return 0.8f;
    return 0.6f;
}

void grassland_spawn_init(void) {
    s_timer = 0.0f;
    s_interval = 1.5f;
    s_mode = GRASSLAND_SPAWN_MODE_NORMAL;
}

void grassland_spawn_set_mode(GrasslandSpawnMode mode) {
    s_mode = mode;
}

void grassland_spawn_update(float dt, float run_time, Entity player) {
    if (s_mode == GRASSLAND_SPAWN_MODE_DISABLED) return;
    if (!ecs_has_components(player, COMP_POSITION)) return;

    Position *pp = &g_positions[player];

    s_interval = get_spawn_interval(run_time);
    s_timer += dt;

    if (s_timer < s_interval) return;
    s_timer -= s_interval;

    EnemyType t;

    if (s_mode == GRASSLAND_SPAWN_MODE_T1_ONLY) {
        // uniquement T1 pendant certains boss
        t = (rand() % 10 < 7) ? ENEMY_TYPE_APHIDLING : ENEMY_TYPE_MITE;
    } else {
        t = pick_enemy_type_normal(run_time);
    }

    grassland_spawn_enemy_around_player(t, pp);
}
