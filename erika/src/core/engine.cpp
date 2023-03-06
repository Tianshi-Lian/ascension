#include "core/engine.hpp"

#include "yuki/debug/logger.hpp"

#include "core/game.hpp"

namespace erika {

void
Engine::initialize()
{
    yuki::debug::Logger::initialize("logs/app.log", yuki::debug::Log_Level::DEBUG, true, true);
    yuki::debug::Logger::info("erika > Initializing game engine...");

    yuki::debug::Logger::notice("erika > Initialized game engine.");
}

void
Engine::run(const std::shared_ptr<Game>& game)
{
    if (!game) {
        yuki::debug::Logger::critical("Engine::run() failed to run: invalid game!");
    }

    game->initialize();

    // TODO game loop
    game->update(0.16f);
    game->render(0.16f);
}

}
