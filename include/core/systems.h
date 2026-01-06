#ifndef CORE_SYSTEMS_H
#define CORE_SYSTEMS_H

#include "core/ecs.h"

// systèmes génériques
void system_movement(float dt);
void system_projectiles(float dt);
void system_collisions(void);
void system_enemy_player_contact(float dt);


#endif // CORE_SYSTEMS_H
