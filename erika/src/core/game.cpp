#include "core/game.hpp"

namespace erika {

bool
Game::initialize()
{
    return on_initialize();
}

void
Game::update(f32 delta_time)
{
    on_update(delta_time);
}

void
Game::render(f32 delta_time)
{
    on_render(delta_time);
}

}
