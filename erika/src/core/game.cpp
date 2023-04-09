/**
 * File: game.cpp
 * Project: erika
 * File Created: 2023-03-03 18:41:58
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:04:33
 * ------------------
 * Copyright 2023 Rob Graham
 * ==================
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ==================
 */

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
Game::set_platform_state(const std::shared_ptr<yuki::Platform_State>& platform_state)
{
    m_platform_state = platform_state;
}

}
