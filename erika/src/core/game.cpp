#include "core/game.hpp"

namespace erika {

Game::Game(std::string window_title)
  : m_window_title(std::move(window_title))
{
}

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

const std::string&
Game::get_window_title() const
{
    return m_window_title;
}

void
Game::set_window_title(std::string value)
{
    m_window_title = std::move(value);
}

}
