#include <math.h>
#include <stdlib.h>

#include "biomes/grassland/grassland_enemies.h"
#include "core/ecs.h"

EnemyArchetype g_enemy_archetypes[] = {
    { ENEMY_TYPE_APHIDLING,     1, 80.0f  },
    { ENEMY_TYPE_MITE,          1, 140.0f },
    { ENEMY_TYPE_LEAFHOPPER,    2, 110.0f },
    { ENEMY_TYPE_MINI_BEETLE,   4, 60.0f  },
    { ENEMY_TYPE_ANT_SCOUT,     2, 160.0f }, 
    { ENEMY_TYPE_HORNED_BEETLE, 6, 70.0f  }, 
};



const EnemyArchetype* grassland_get_enemy_archetype(EnemyType type) {
    size_t count = sizeof(g_enemy_archetypes) / sizeof(g_enemy_archetypes[0]);
    for (size_t i = 0; i < count; ++i) {
        if (g_enemy_archetypes[i].type == type) {
            return &g_enemy_archetypes[i];
        }
    }
    return NULL;
}

void grassland_spawn_enemy_around_player(EnemyType type, Position *playerPos) {
    const EnemyArchetype *arch = grassland_get_enemy_archetype(type);
    if (!arch) return;

    Entity e = ecs_create_entity();

    float radius = 600.0f;
    float angle = (float)rand() / (float)RAND_MAX * 2.0f * 3.1415926f;

    float x = playerPos->x + cosf(angle) * radius;
    float y = playerPos->y + sinf(angle) * radius;

    ecs_add_position(e, x, y);
    ecs_add_velocity(e, 0.0f, 0.0f);
    ecs_add_enemy_tag(e);
    ecs_add_health(e, arch->base_hp);
    ecs_set_enemy_type(e, type);

    // la vitesse de base sera utilisée dans l'IA plus tard (avec arch->base_speed)
}
