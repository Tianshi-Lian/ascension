#include "ascension.hpp"

int
main(int /*argc*/, char** /*args*/) // NOLINT
{
    ascension::Ascension game;
    game.on_initialize();
    game.on_update(1 / 60);
    game.on_render(1.0);
    return 0;
}
