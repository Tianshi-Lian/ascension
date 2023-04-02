#include "core/engine.hpp"

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include "yuki/input/input.hpp"
#include "yuki/platform/platform.hpp"

#include "core/game.hpp"

namespace erika {

void
Engine::initialize()
{
    PROFILE_FUNCTION();
    yuki::debug::Logger::info("erika > Initializing game engine...");

    yuki::debug::Logger::notice("erika > Initialized game engine.");
}

void
Engine::run(const std::shared_ptr<yuki::platform::Platform_State>& platform_state, const std::shared_ptr<Game>& game)
{
    PROFILE_FUNCTION();
    if (!game) {
        yuki::debug::Logger::critical("Engine::run() failed to run: invalid game!");
    }
    yuki::debug::Logger::debug("erika > Engine::run() starting game %s", game->get_window_title().c_str());

    game->set_platform_state(platform_state);
    game->initialize();

    // TODO game loop
    u64 milliseconds_elapsed = 0;
    while (milliseconds_elapsed < 3000) {
        yuki::platform::Platform::process_messages(platform_state);
        yuki::platform::Platform::sleep(50);
        milliseconds_elapsed += 50;
        game->update(0.16f);
        game->render(0.16f);

        if (yuki::input::Input::was_key_down(yuki::input::Key::ESCAPE)) {
            milliseconds_elapsed = 3001;
        }
    }
    yuki::platform::Platform::shutdown(platform_state);
}

}
