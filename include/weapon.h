#ifndef WEAPON_H
#define WEAPON_H

#include <stdint.h>
#include "ecs.h"
#include "engine.h"

// Types d'armes possibles
typedef enum {
    WEAPON_NONE = 0,
    WEAPON_THORN_SHOT = 1,
    // plus tard: WEAPON_LEAF_BLADE, WEAPON_POLLEN_BURST, etc.
} WeaponType;

typedef struct {
    WeaponType type;
    float cooldown;        // cooldown restant
    float fire_rate;       // intervalle entre tirs
    int damage;
} Weapon;

// Fonction pour initialiser une arme
void weapon_init(Weapon *w, WeaponType type);

// Système qui gère le tir
void system_weapon_fire(float dt, Entity player, Weapon *weapon, Engine *engine);

#endif
