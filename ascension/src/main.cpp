#include <iostream>

#include <erika/erika.hpp>
#include <yuki/yuki.hpp>

#include <yuki/debug/logger.hpp>

#include "ascension.hpp"

int main() {
    yuki::Logger::initialize("logs/app.txt");
    yuki::Logger::set_log_severity_level(yuki::Log_Level::DEBUG);
    yuki::Logger::enable_console_logging(true);
    yuki::Logger::enable_file_logging(true);

    i32 e_result = erika::test();
    i32 y_result = yuki::test();

    yuki::Logger::debug("debug: %s", "test");
    yuki::Logger::notice("erika::test: %d, yuki::test: %d", e_result, y_result);

    yuki::Logger::info("Initializing Ascension...");
    ascension::Ascension game;
    game.initialize();
    game.run();

    return 0;
}
