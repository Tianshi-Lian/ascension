#include <iostream>

#include <erika/core/engine.hpp>

#include "ascension.hpp"

int
main()
{
    erika::Engine engine;
    engine.initialize();

    auto game = std::make_shared<ascension::Ascension>();
    engine.run(game);

    return 0;
}
