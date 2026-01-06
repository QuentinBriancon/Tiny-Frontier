#include "core/ecs.h"
#include "core/systems.h"

void system_movement(float dt) {
    ComponentMask needed = COMP_POSITION | COMP_VELOCITY;

    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!g_entities[e].alive) continue;
        if (!ecs_has_components(e, needed)) continue;

        Position *p = &g_positions[e];
        Velocity *v = &g_velocities[e];

        p->x += v->vx * dt;
        p->y += v->vy * dt;
    }
}
