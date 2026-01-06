#include "biomes/grassland/grassland_bosses.h"
#include "biomes/grassland/boss_shield_beetle.h"

// TODO: adapte ça selon ton grassland_spawn.h
// exemple attendu : void grassland_spawn_set_mode(GrasslandSpawnMode mode);
#include "biomes/grassland/grassland_spawn.h"

static GrasslandBossId active = GRASSLAND_BOSS_NONE;
static int shield_beetle_triggered = 0;

void grassland_bosses_init(void) {
    active = GRASSLAND_BOSS_NONE;
    shield_beetle_triggered = 0;
}

int grassland_bosses_is_active(void) {
    return active != GRASSLAND_BOSS_NONE;
}

GrasslandBossId grassland_bosses_get_active_id(void) {
    return active;
}

void grassland_bosses_update(float dt, float run_time, Entity player) {
    // Trigger Shield Beetle at 10:00 = 600s
    if (!shield_beetle_triggered && run_time >= 600.0f) {
        shield_beetle_triggered = 1;
        active = GRASSLAND_BOSS_SHIELD_BEETLE;

        // Spawn rules during boss
        grassland_spawn_set_mode(GRASSLAND_SPAWN_MODE_T1_ONLY);

        boss_shield_beetle_start(player);
    }

    // Update active boss
    if (active == GRASSLAND_BOSS_SHIELD_BEETLE) {
        boss_shield_beetle_update(dt, player);

        if (boss_shield_beetle_is_done()) {
            active = GRASSLAND_BOSS_NONE;
            grassland_spawn_set_mode(GRASSLAND_SPAWN_MODE_NORMAL);
        }
    }

    // Queen Ant / Colossus plus tard
}
