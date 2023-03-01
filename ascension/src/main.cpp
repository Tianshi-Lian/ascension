#include <iostream>

#include <erika/erika.hpp>
#include <yuki/yuki.hpp>

#include <yuki/debug/logger.hpp>

#include "ascension.hpp"

int
main()
{
    yuki::Logger::initialize("logs/app.log", yuki::Log_Level::DEBUG, true, true);

    i32 e_result = erika::test();
    i32 y_result = yuki::test();

    yuki::Logger::debug("ascension > debug: %s", "test");
    yuki::Logger::notice("ascension > erika::test: %d, yuki::test: %d", e_result, y_result);

    yuki::Logger::info("ascension > Initializing Ascension...");
    ascension::Ascension game;
    game.initialize();
    game.run();

    return 0;
}
