#include "biomes/grassland/boss_shield_beetle.h"
#include "core/ecs.h"
#include <math.h>

typedef enum {
    SB_STATE_IDLE = 0,
    SB_STATE_CHARGE,
    SB_STATE_RECOVER
} ShieldBeetleState;

static Entity boss_e = (Entity)0;

static int boss_active = 0;

static ShieldBeetleState state = SB_STATE_IDLE;
static float state_t = 0.0f;

// tuning
static const int   SB_HP = 450;
static const float SB_IDLE_TIME   = 1.0f;
static const float SB_CHARGE_TIME = 0.8f;
static const float SB_RECOVER_TIME= 1.2f;
static const float SB_CHARGE_SPEED= 320.0f;

static void sb_set_velocity_towards_player(Entity player, float speed) {
    if (!ecs_is_alive(player)) return;
    if (!ecs_has_components(player, COMP_POSITION)) return;

    Position *bp = &g_positions[boss_e];
    Position *pp = &g_positions[player];

    float dx = pp->x - bp->x;
    float dy = pp->y - bp->y;
    float len = sqrtf(dx*dx + dy*dy);

    if (len < 0.0001f) {
        g_velocities[boss_e].vx = 0.0f;
        g_velocities[boss_e].vy = 0.0f;
        return;
    }

    dx /= len; dy /= len;
    g_velocities[boss_e].vx = dx * speed;
    g_velocities[boss_e].vy = dy * speed;
}

void boss_shield_beetle_start(Entity player) {
    // already running
    if (boss_active && ecs_is_alive(boss_e)) return;

    // create
    boss_e = ecs_create_entity();
    if (boss_e == INVALID_ENTITY) { boss_active = 0; return; }


    boss_active = 1;

    // spawn near player
    float x = 0.0f, y = 0.0f;
    if (ecs_is_alive(player) && ecs_has_components(player, COMP_POSITION)) {
        x = g_positions[player].x + 180.0f;
        y = g_positions[player].y;
    }

    ecs_add_position(boss_e, x, y);
    ecs_add_velocity(boss_e, 0.0f, 0.0f);
    ecs_add_enemy_tag(boss_e);
    ecs_set_enemy_type(boss_e, ENEMY_TYPE_BOSS_SHIELD_BEETLE);
    ecs_add_health(boss_e, SB_HP);
    ecs_add_damage(boss_e, 18);

    // state
    state = SB_STATE_IDLE;
    state_t = 0.0f;
}

void boss_shield_beetle_update(float dt, Entity player) {
    if (!boss_active) return;
    if (!ecs_is_alive(boss_e)) { boss_active = 0; return; }
    if (!ecs_has_components(boss_e, COMP_POSITION | COMP_VELOCITY | COMP_HEALTH | COMP_ENEMY)) {
        boss_active = 0;
        return;
    }

    state_t += dt;

    switch (state) {
        case SB_STATE_IDLE:
            // stand still
            g_velocities[boss_e].vx = 0.0f;
            g_velocities[boss_e].vy = 0.0f;

            if (state_t >= SB_IDLE_TIME) {
                state = SB_STATE_CHARGE;
                state_t = 0.0f;
                sb_set_velocity_towards_player(player, SB_CHARGE_SPEED);
            }
            break;

        case SB_STATE_CHARGE:
            // keep current velocity for the duration
            if (state_t >= SB_CHARGE_TIME) {
                state = SB_STATE_RECOVER;
                state_t = 0.0f;
                g_velocities[boss_e].vx = 0.0f;
                g_velocities[boss_e].vy = 0.0f;
            }
            break;

        case SB_STATE_RECOVER:
            g_velocities[boss_e].vx = 0.0f;
            g_velocities[boss_e].vy = 0.0f;

            if (state_t >= SB_RECOVER_TIME) {
                state = SB_STATE_IDLE;
                state_t = 0.0f;
            }
            break;
    }
}

int boss_shield_beetle_is_done(void) {
    if (!boss_active) return 1;
    if (!ecs_is_alive(boss_e)) return 1;
    if (!ecs_has_components(boss_e, COMP_HEALTH)) return 1;
    if (g_health[boss_e].hp <= 0) return 1;
    return 0;
}
