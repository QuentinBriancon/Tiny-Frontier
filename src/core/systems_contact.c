#include "core/ecs.h"
#include "core/systems.h"
#include <math.h>

void system_enemy_player_contact(float dt) {
    ComponentMask playerMask = COMP_POSITION | COMP_PLAYER | COMP_HEALTH;
    ComponentMask enemyMask  = COMP_POSITION | COMP_ENEMY  | COMP_DAMAGE;

    const float r = 22.0f;
    const float r2 = r * r;

    for (Entity p = 0; p < MAX_ENTITIES; ++p) {
        if (!ecs_has_components(p, playerMask)) continue;

        // i-frames joueur
        g_enemy_timers[p] -= dt;
        if (g_enemy_timers[p] > 0.0f) continue;

        Position *pp = &g_positions[p];

        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (!ecs_has_components(e, enemyMask)) continue;

            Position *ep = &g_positions[e];

            float dx = ep->x - pp->x;
            float dy = ep->y - pp->y;
            float d2 = dx*dx + dy*dy;

            if (d2 < r2) {
                g_health[p].hp -= g_damage[e].dmg;
                if (g_health[p].hp < 0) g_health[p].hp = 0;

                g_enemy_timers[p] = 0.5f; // ✅ 0.5s invincible après hit
                return;
            }
        }
    }
}
