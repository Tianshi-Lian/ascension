#include "core/game.hpp"

#include "yuki/debug/instrumentor.hpp"

namespace erika {

Game::Game(std::string window_title)
  : m_window_title(std::move(window_title))
{
}

bool
Game::initialize()
{
    m_plugin_manager.initialize(m_platform_state);

    return on_initialize();
}

void
Game::update(f32 delta_time)
{
    PROFILE_FUNCTION();
    on_update(delta_time);
}

void
Game::render(f32 interpolation)
{
    PROFILE_FUNCTION();
    on_render(interpolation);
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

void
Game::set_platform_state(const std::shared_ptr<yuki::platform::Platform_State>& platform_state)
{
    m_platform_state = platform_state;
}

}
