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

    yuki::platform::Platform::initialize(std::make_shared<int>(1), "", 0, 0, 0, 0);

    erika::Engine engine;
    engine.initialize();

    auto game = erika::game::create();
    engine.run(game);

    PROFILE_END_SESSION();
    return 0;
}
