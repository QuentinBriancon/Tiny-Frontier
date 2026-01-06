#ifndef GAME_BIOME_H
#define GAME_BIOME_H

#include "core/ecs.h"

// Interface commune pour un biome
typedef struct BiomeAPI {
    void (*init)(void);
    void (*update)(float dt, float run_time, Entity player);
    void (*shutdown)(void);
} BiomeAPI;

#endif // GAME_BIOME_H
