#pragma once

#include "erika/core/engine.hpp"
#include "erika/core/game.hpp"

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include <iostream>

int
main() // NOLINT
{
    yuki::debug::Logger::initialize("logs/app.log", yuki::debug::Log_Level::DEBUG, true, true);
    PROFILE_BEGIN_SESSION("erika_profiling", "logs/timings.json");

    erika::Engine engine;
    engine.initialize();

    auto game = erika::game::create();
    engine.run(game);

    PROFILE_END_SESSION();
    return 0;
}
