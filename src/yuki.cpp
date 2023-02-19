#include "yuki.h"

#include "debug/logger.h"

int yuki::test() {
    Logger::initialize("logs/app.txt");
    Logger::set_log_severity_level(Log_Level::DEBUG);
    Logger::enable_console_logging(true);

    Logger::info("void yuki::test()");

    return 2;
}
