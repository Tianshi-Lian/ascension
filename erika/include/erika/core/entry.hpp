#pragma once

#include "erika/core/engine.hpp"
#include "erika/core/game.hpp"

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include "yuki/platform/platform.hpp"

int
main(int /*argc*/, char** /*args*/) // NOLINT
{
    yuki::debug::Logger::initialize("logs/app.log", yuki::debug::Log_Level::DEBUG, true, true);
    PROFILE_BEGIN_SESSION("erika_profiling", "logs/timings.json");

    erika::Engine engine;
    engine.initialize();

    auto game = erika::game::create();

    constexpr static u32 default_screen_pos = 128;
    constexpr static u32 default_screen_width = 1600;
    constexpr static u32 default_screen_height = 900;

    const auto state = std::make_shared<yuki::platform::Platform_State>();
    if (!yuki::platform::Platform::initialize(
            state, game->get_window_title(), default_screen_pos, default_screen_pos, default_screen_width, default_screen_height
        )) {

        yuki::debug::Logger::critical("Failed to initialize platform layer for game %s", game->get_window_title().c_str());
        return 1;
    }

    engine.run(game);
    // TODO: Move the game loop inside of the engine.run()
    u64 milliseconds_elapsed = 0;
    while (milliseconds_elapsed < 3000) {
        yuki::platform::Platform::process_messages();
        yuki::platform::Platform::sleep(50);
        milliseconds_elapsed += 50;
    }
    yuki::platform::Platform::shutdown(state);

    PROFILE_END_SESSION();
    return 0;
}
