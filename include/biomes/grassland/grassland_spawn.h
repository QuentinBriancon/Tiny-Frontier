#ifndef BIOMES_GRASSLAND_SPAWN_H
#define BIOMES_GRASSLAND_SPAWN_H

#include "core/ecs.h"

typedef enum {
    GRASSLAND_SPAWN_MODE_NORMAL = 0,
    GRASSLAND_SPAWN_MODE_T1_ONLY = 1,
    GRASSLAND_SPAWN_MODE_DISABLED = 2
} GrasslandSpawnMode;

void grassland_spawn_init(void);
void grassland_spawn_set_mode(GrasslandSpawnMode mode);

// update spawn selon run_time + mode
void grassland_spawn_update(float dt, float run_time, Entity player);

#endif // BIOMES_GRASSLAND_SPAWN_H
