#include "core/engine.h"
#include "core/time.h"
#include "game/game.h"

int main(void) {
    Engine engine;
    if (!engine_init(&engine, "Tiny Frontier", 1280, 720)) {
        return 1;
    }

    time_init();
    game_init(&engine);

    while (engine.running) {
        engine_handle_events(&engine);

        time_update();
        float dt = time_get_dt();

        game_update(&engine, dt);

        engine_begin_render(&engine);
        game_render(&engine);
        engine_end_render(&engine);
    }

    game_shutdown();
    engine_shutdown(&engine);
    return 0;
}
