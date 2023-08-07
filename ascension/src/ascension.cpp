/**
 * File: ascension.cpp
 * Project: ascension
 * File Created: 2023-04-13 20:17:48
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-07 15:03:18
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
    m_asset_manager.load_texture_2d("textures/unicorn");
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

    const auto& glyph_a = sprite_font->get_glyph('A', 16);
    const auto& glyph_s = sprite_font->get_glyph('s', 16);
    const auto& glyph_c = sprite_font->get_glyph('c', 16);
    const auto& glyph_e = sprite_font->get_glyph('e', 16);
    const auto& glyph_n = sprite_font->get_glyph('n', 16);
    const auto& glyph_i = sprite_font->get_glyph('i', 16);
    const auto& glyph_o = sprite_font->get_glyph('o', 16);
    m_sprite_batch.draw(glyph_a.texture, glyph_a.sub_texture, { 0, 870 }, true);
    m_sprite_batch.draw(glyph_s.texture, glyph_s.sub_texture, { 16, 870 }, true);
    m_sprite_batch.draw(glyph_c.texture, glyph_c.sub_texture, { 32, 870 }, true);
    m_sprite_batch.draw(glyph_e.texture, glyph_e.sub_texture, { 48, 870 }, true);
    m_sprite_batch.draw(glyph_n.texture, glyph_n.sub_texture, { 64, 870 }, true);
    m_sprite_batch.draw(glyph_s.texture, glyph_s.sub_texture, { 80, 870 }, true);
    m_sprite_batch.draw(glyph_i.texture, glyph_i.sub_texture, { 100, 870 }, true);
    m_sprite_batch.draw(glyph_o.texture, glyph_o.sub_texture, { 104, 870 }, true);
    m_sprite_batch.draw(glyph_n.texture, glyph_n.sub_texture, { 120, 870 }, true);
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
