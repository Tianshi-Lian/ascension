/**
 * File: ascension.cpp
 * Project: ascension
 * File Created: 2023-04-13 20:17:48
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-30 15:26:39
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

#include "ascension.hpp"

#include "graphics/sprite_batch.hpp"

namespace ascension {

void
Ascension::on_initialize()
{
    m_asset_manager.load_asset_file("assets/assets.xml");
    m_asset_manager.load_texture_2d("textures/unicorn");

    const auto sprite_shader = m_asset_manager.load_shader("shaders/spritebatch");
    m_sprite_batch.initialize(m_window->get_width(), m_window->get_height(), sprite_shader);
}

void
Ascension::on_update(f64 delta_time)
{
    (void)delta_time;
}

void
Ascension::on_render(f32 interpolation)
{
    (void)interpolation;

    m_sprite_batch.begin();
    m_sprite_batch.draw(m_asset_manager.get_texture_2d("textures/unicorn"), v2f{ 0.0f, 0.0f });
    m_sprite_batch.end();
}

}
