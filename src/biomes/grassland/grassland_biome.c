#include <stdlib.h>
#include <math.h>

#include "biomes/grassland/grassland_biome.h"
#include "biomes/grassland/grassland_enemies.h"
#include "biomes/grassland/grassland_spawn.h"
#include "biomes/grassland/grassland_ai.h"
#include "core/ecs.h"


// ---- impl de l'API Biome ----

static void grassland_init(void) {
    grassland_spawn_init();
}

static void grassland_update(float dt, float run_time, Entity player) {
    grassland_spawn_update(dt, run_time, player);
    grassland_enemy_ai(dt, player);
}

static void grassland_shutdown(void) {
    // rien pour l'instant
}

BiomeAPI grassland_biome_get_api(void) {
    BiomeAPI api = {
        .init     = grassland_init,
        .update   = grassland_update,
        .shutdown = grassland_shutdown
    };
    return api;
}
