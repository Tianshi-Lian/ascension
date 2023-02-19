#include <iostream>

#include <erika/erika.h>
#include <yuki/yuki.h>

#include <yuki/debug/logger.h>

int main() {
    yuki::Logger::info("Initializing Ascension...");

    i32 e_result = erika::test();
    i32 y_result = yuki::test();

    yuki::Logger::notice("erika::test: %d, yuki::test: %d", e_result, y_result);

    return 0;
}
