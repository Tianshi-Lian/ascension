#include <iostream>

#include <erika/erika.h>
#include <yuki/yuki.h>

#include <yuki/debug/logger.h>

int main() {
    yuki::Logger::initialize("logs/app.txt");
    yuki::Logger::set_log_severity_level(yuki::Log_Level::DEBUG);
    yuki::Logger::enable_console_logging(true);
    yuki::Logger::enable_file_logging(true);

    yuki::Logger::info("Initializing Ascension...");

    i32 e_result = erika::test();
    i32 y_result = yuki::test();

    yuki::Logger::debug("debug: %s", "test");
    yuki::Logger::notice("erika::test: %d, yuki::test: %d", e_result, y_result);

    return 0;
}
