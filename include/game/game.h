#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "core/engine.h"

// Initialise l'ECS, le joueur, l'arme, les timers, etc.
void game_init(Engine *engine);

// Met à jour tout le gameplay (input, spawn, IA, systèmes, caméra).
void game_update(Engine *engine, float dt);

// Fait tout le rendu (timer + entités).
void game_render(Engine *engine);

// Cleanup si besoin plus tard.
void game_shutdown(void);

#endif // GAME_GAME_H
