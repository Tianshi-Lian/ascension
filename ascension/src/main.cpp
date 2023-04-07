#include <SDL.h>
#include <iostream>

#define UNUSED_PARAM(a) (void)a

int
main(int argc, char** argv)
{
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);

    std::cout << "Hello, world!" << std::endl;

    return 0;
}
