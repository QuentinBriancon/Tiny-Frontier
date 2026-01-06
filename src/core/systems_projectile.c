#include "core/ecs.h"
#include "core/systems.h"

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
