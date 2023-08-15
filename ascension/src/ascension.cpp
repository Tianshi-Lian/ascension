/**
 * File: ascension.cpp
 * Project: ascension
 * File Created: 2023-04-13 20:17:48
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-15 21:20:30
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

#include <glm/ext/matrix_clip_space.hpp>

#include "graphics/shader.hpp"
#include "graphics/sprite_font.hpp"
#include "graphics/texture_atlas.hpp"

#include "yuki/debug/instrumentor.hpp"

namespace ascension {

const i32 WINDOW_WIDTH = 1600, WINDOW_HEIGHT = 900, OBJECT_COUNT = 1000;

void
Ascension::on_initialize()
{
    m_asset_manager.load_asset_file("assets/assets.xml");
    m_asset_manager.load_texture("textures/unicorn");
    auto fruit_atlas = m_asset_manager.load_texture_atlas("textures/fruits");
    auto sprite_shader = m_asset_manager.load_shader("shaders/spritebatch");
    auto font_shader = m_asset_manager.load_shader("shaders/spritefont");
    auto sprite_font = m_asset_manager.load_font("fonts/arial");

    // viewport setup
    {
        static constexpr auto mat_identity = m4{ 1.0f };
        auto projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -1.0f, 1.0f);
        sprite_shader->bind();
        sprite_shader->set_mat4f("m_projection_view", projection * mat_identity);
        font_shader->bind();
        font_shader->set_mat4f("m_projection_view", projection * mat_identity);
    }

    m_sprite_batch.create(16, 2048, sprite_shader);
    m_font_batch.create(8, 2048, font_shader);

    auto fruits = std::make_shared<graphics::Batch>();
    fruits->create({ OBJECT_COUNT, fruit_atlas->get_texture(), sprite_shader, true });

    for (u32 i = 0; i < OBJECT_COUNT; ++i) {
        u32 id = static_cast<u32>(rand() % 9);
        const auto fruit_texture = fruit_atlas->get_sub_texture(id);

        const v2f position = { static_cast<i16>((static_cast<u32>(rand()) % (WINDOW_WIDTH - fruit_texture.width()))),
                               static_cast<i16>((static_cast<u32>(rand()) % (WINDOW_HEIGHT - fruit_texture.height()))) };

        fruits->add(fruit_texture, position);
    }

    m_sprite_batch.add_batch(fruits);

    m_sprite_batch.draw_string(sprite_font, 48, { 0, 850 }, "Ascension");
    m_sprite_batch.draw_string(
        sprite_font, 32, { 0, 820 }, "A 2D roguelike game about ascending through the 9 planes of mortality."
    );
}

void
Ascension::on_update(f64 delta_time)
{
    (void)delta_time;

    if (m_input_manager.is_key_down(input::Key::ESCAPE)) {
        quit();
    }
}

void
Ascension::on_render(f32 interpolation)
{
    PROFILE_FUNCTION();
    (void)interpolation;

    m_sprite_batch.flush();
    m_font_batch.flush();
}
}
