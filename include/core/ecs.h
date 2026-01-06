#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_ENTITIES 1024

typedef uint32_t Entity;
typedef uint32_t ComponentMask;

// Bits de composants / tags
#define COMP_POSITION    (1u << 0)
#define COMP_VELOCITY    (1u << 1)
#define COMP_PLAYER      (1u << 2)
#define COMP_ENEMY       (1u << 3)
#define COMP_HEALTH      (1u << 4)
#define COMP_DAMAGE      (1u << 5)
#define COMP_PROJECTILE  (1u << 6)
#define COMP_LIFETIME    (1u << 7)
#define COMP_BOSS        (1u << 8)  // pour taguer un boss plus tard

// Types d'ennemis (Grassland pour l'instant)
typedef enum {
    ENEMY_TYPE_NONE = 0,
    ENEMY_TYPE_APHIDLING      = 1,
    ENEMY_TYPE_MITE           = 2,
    ENEMY_TYPE_LEAFHOPPER     = 3,
    ENEMY_TYPE_MINI_BEETLE    = 4,
    ENEMY_TYPE_ANT_SCOUT      = 5,
    ENEMY_TYPE_HORNED_BEETLE  = 6
} EnemyType;

typedef struct {
    float x, y;
} Position;

typedef struct {
    float vx, vy;
} Velocity;

typedef struct {
    bool alive;
    ComponentMask mask;
} EntityMeta;

typedef struct {
    int hp;
    int max_hp;
} Health;

typedef struct {
    int dmg;
} Damage;

typedef struct {
    float time_remaining;
} Lifetime;

// tableaux globaux (définis dans ecs.c)
extern EntityMeta g_entities[MAX_ENTITIES];
extern Position   g_positions[MAX_ENTITIES];
extern Velocity   g_velocities[MAX_ENTITIES];
extern Health     g_health[MAX_ENTITIES];
extern Damage     g_damage[MAX_ENTITIES];
extern Lifetime   g_lifetime[MAX_ENTITIES];
extern EnemyType  g_enemy_types[MAX_ENTITIES];
extern float      g_enemy_timers[MAX_ENTITIES];

// API ECS de base
void   ecs_init(void);
Entity ecs_create_entity(void);
void   ecs_destroy_entity(Entity e);
bool   ecs_is_alive(Entity e);

void      ecs_set_enemy_type(Entity e, EnemyType type);
EnemyType ecs_get_enemy_type(Entity e);

// Ajout de composants
void ecs_add_position(Entity e, float x, float y);
void ecs_add_velocity(Entity e, float vx, float vy);
void ecs_add_player_tag(Entity e);
void ecs_add_enemy_tag(Entity e);
void ecs_add_health(Entity e, int hp);
void ecs_add_damage(Entity e, int dmg);
void ecs_add_projectile_tag(Entity e);
void ecs_add_lifetime(Entity e, float seconds);

// Check de mask de composants
int ecs_has_components(Entity e, ComponentMask mask);

#endif // ECS_H
