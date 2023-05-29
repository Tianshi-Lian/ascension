/**
 * File: qilian.cpp
 * Project: qilian
 * File Created: 2023-04-04 18:39:13
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-29 19:17:34
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

#include "qilian.hpp"

#include "erika/core/entry.hpp" // IWYU pragma: keep

#include "erika/plugins/base/renderer.hpp"

namespace qilian {

Qilian::Qilian()
  : erika::Game("Qilian")
{
}

bool
Qilian::on_initialize()
{
    PROFILE_FUNCTION();
    yuki::debug::Logger::debug("qilian", "Qilian::on_initialize()");

    m_plugin_manager.change_active_renderer("OpenGL_Renderer");

    return true;
}

void
Qilian::on_update(f32 delta_time)
{
    (void)delta_time;
}

void
Qilian::on_render(f32 interpolation)
{
    (void)interpolation;

    auto renderer = m_plugin_manager.get_active_renderer();
    renderer->begin_scene();

    renderer->end_scene();
}

}

std::shared_ptr<erika::Game>
erika::game::create()
{
    return std::make_shared<qilian::Qilian>();
}
