#ifndef ECS_H
#define ECS_H

#include <stdint.h>

#define MAX_ENTITIES 1024

typedef uint32_t Entity;
typedef uint32_t ComponentMask;

// composants
#define COMP_POSITION (1u << 0)
#define COMP_VELOCITY (1u << 1)
#define COMP_PLAYER   (1u << 2)
#define COMP_ENEMY    (1u << 3)
#define COMP_HEALTH   (1u << 4)
#define COMP_DAMAGE   (1u << 5)
#define COMP_PROJECTILE (1u << 6)
#define COMP_LIFETIME (1u << 7)


typedef enum {
    ENEMY_TYPE_NONE = 0,
    ENEMY_TYPE_APHIDLING   = 1,
    ENEMY_TYPE_MITE        = 2,
    ENEMY_TYPE_LEAFHOPPER  = 3,
    ENEMY_TYPE_MINI_BEETLE = 4,
    ENEMY_TYPE_ANT_SCOUT   = 5, 
    ENEMY_TYPE_HORNED_BEETLE = 6 
} EnemyType;





typedef struct {
    float x, y;
} Position;

typedef struct {
    float vx, vy;
} Velocity;

typedef struct {
    int alive;
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
extern Health   g_health[MAX_ENTITIES];
extern Damage   g_damage[MAX_ENTITIES];
extern Lifetime g_lifetime[MAX_ENTITIES];
extern EnemyType g_enemy_types[MAX_ENTITIES];
extern float g_enemy_timers[MAX_ENTITIES];

// API ECS de base
void   ecs_init(void);
Entity ecs_create_entity(void);
void   ecs_destroy_entity(Entity e);
void ecs_set_enemy_type(Entity e, EnemyType type);
EnemyType ecs_get_enemy_type(Entity e);


void ecs_add_position(Entity e, float x, float y);
void ecs_add_velocity(Entity e, float vx, float vy);
void ecs_add_player_tag(Entity e);
void ecs_add_enemy_tag(Entity e);
void ecs_add_health(Entity e, int hp);
void ecs_add_damage(Entity e, int dmg);
void ecs_add_projectile_tag(Entity e);
void ecs_add_lifetime(Entity e, float seconds);

int   ecs_has_components(Entity e, ComponentMask mask);

// systèmes
void system_movement(float dt);
void system_enemy_ai(float dt, Entity player);
void system_projectiles(float dt);
void system_collisions(void);

#endif // ECS_H
