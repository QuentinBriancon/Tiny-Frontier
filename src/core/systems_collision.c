#include "core/ecs.h"
#include "core/systems.h"
#include <math.h>

void system_collisions(void) {
    ComponentMask projMask  = COMP_POSITION | COMP_DAMAGE | COMP_PROJECTILE;
    ComponentMask enemyMask = COMP_POSITION | COMP_HEALTH | COMP_ENEMY;

    const float r = 20.0f;
    const float r2 = r * r;

    for (Entity p = 0; p < MAX_ENTITIES; ++p) {
        if (!ecs_has_components(p, projMask)) continue;

        Position *pp = &g_positions[p];

        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (!ecs_has_components(e, enemyMask)) continue;

            Position *ep = &g_positions[e];

            float dx = ep->x - pp->x;
            float dy = ep->y - pp->y;
            float dist2 = dx*dx + dy*dy;

            if (dist2 < r2) {
                g_health[e].hp -= g_damage[p].dmg;

                ecs_destroy_entity(p);

                if (g_health[e].hp <= 0) {
                    ecs_destroy_entity(e);
                }
                break;
            }
        }
    }
}
