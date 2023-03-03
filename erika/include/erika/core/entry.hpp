#include "erika/core/engine.hpp"
#include "erika/core/game.hpp"

int
main() // NOLINT
{
    erika::Engine engine;
    engine.initialize();

    auto game = erika::game::create();
    engine.run(game);

    return 0;
}
