#include "yuki.hpp"

#include "debug/logger.hpp"

int
yuki::test()
{
    debug::Logger::info("yuki > void yuki::test()");

    return 2;
}
