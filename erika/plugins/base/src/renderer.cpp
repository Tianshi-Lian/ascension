/**
 * File: renderer.cpp
 * Project: erika
 * File Created: 2023-03-25 09:53:09
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-29 18:48:09
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

#include "renderer.hpp"

namespace erika::plugins {

Renderer::Renderer()
  : Plugin(Plugin_Type::Renderer)
  , m_screen_width(0)
  , m_screen_height(0)
{
}

void
Renderer::set_screen_size(const u32 width, const u32 height)
{
    on_resize(m_screen_width, m_screen_height);

    m_screen_width = width;
    m_screen_height = height;
}

u32
Renderer::get_screen_width() const
{
    return m_screen_width;
}

u32
Renderer::get_screen_height() const
{
    return m_screen_height;
}

}
