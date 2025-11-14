#ifndef ENEMY_H
#define ENEMY_H

#include "ecs.h"  // pour EnemyType et Position

typedef struct {
    EnemyType type;
    int base_hp;
    float base_speed;
} EnemyArchetype;

// table globale d'archétypes
extern EnemyArchetype g_enemy_archetypes[];

// retourne l'archétype correspondant au type
const EnemyArchetype* get_enemy_archetype(EnemyType type);

// spawn basique autour du joueur
void spawn_enemy_around_player(EnemyType type, Position *playerPos);

#endif // ENEMY_H
