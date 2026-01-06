#pragma once
#include "core/ecs.h"

typedef enum {
    GRASSLAND_BOSS_NONE = 0,
    GRASSLAND_BOSS_SHIELD_BEETLE,
    GRASSLAND_BOSS_QUEEN_ANT,
    GRASSLAND_BOSS_GRASS_COLOSSUS
} GrasslandBossId;

void grassland_bosses_init(void);
void grassland_bosses_update(float dt, float run_time, Entity player);

int  grassland_bosses_is_active(void);
GrasslandBossId grassland_bosses_get_active_id(void);
