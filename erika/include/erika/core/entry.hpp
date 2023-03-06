#include "erika/core/engine.hpp"
#include "erika/core/game.hpp"

#include "yuki/debug/instrumentor.hpp"

int
main() // NOLINT
{
    // TODO: End initialize session and create a session for the game.name
    PROFILE_BEGIN_SESSION("erika_initialize", "logs/erika_profile.json");

    erika::Engine engine;
    engine.initialize();

    auto game = erika::game::create();
    engine.run(game);

    PROFILE_END_SESSION();
    return 0;
}
