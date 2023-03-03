#include "core/engine.hpp"

#include "yuki/debug/logger.hpp"

#include "core/game.hpp"

namespace erika {

void
Engine::initialize()
{
    yuki::Logger::initialize("logs/app.log", yuki::Log_Level::DEBUG, true, true);
    yuki::Logger::info("erika > Initializing game engine...");

    yuki::Logger::notice("erika > Initialized game engine.");
}

void
Engine::run(const std::shared_ptr<Game>& game)
{
    if (!game) {
        yuki::Logger::critical("Engine::run() failed to run: invalid game!");
    }

    game->initialize();

    // TODO game loop
    game->update(0.16f);
    game->render(0.16f);
}

}
