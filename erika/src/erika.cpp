#include "erika.hpp"

#include "yuki/debug/logger.hpp"

int
erika::test()
{
    yuki::debug::Logger::info("erika > void erika::test()");

    return 1;
}
