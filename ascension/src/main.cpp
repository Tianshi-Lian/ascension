#include "core/application.hpp"

#include <SDL.h>

#define UNUSED_PARAM(a) (void)a

constexpr glm::vec2 Default_Window_Position{ 200, 200 };
constexpr glm::vec2 Default_Window_Size{ 1600, 900 };

int
main(int argc, char** argv)
{
    using namespace ascension;

    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);

    core::Application app;
    app.setup("Ascension", Default_Window_Position, Default_Window_Size);
    app.run();

    return 0;
}
