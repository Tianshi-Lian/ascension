/**
 * File: sprite_font.cpp
 * Project: ascension
 * File Created: 2023-07-17 21:08:32
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-10 12:31:24
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

#include "graphics/sprite_font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/ext/matrix_clip_space.hpp>

#include "core/log.hpp"

#include "graphics/frame_buffer.hpp"
#include "graphics/shader.hpp"
#include "graphics/sprite_batch.hpp"

namespace ascension::graphics {

void* Sprite_Font::s_internal = nullptr;

Sprite_Font::Sprite_Font()
  : m_max_texture_size(DEFAULT_TEXTURE_SIZE)
{
}

Sprite_Font::~Sprite_Font()
{
    auto* ft_library = static_cast<FT_Library>(s_internal);
    FT_Done_FreeType(ft_library);
}

bool
Sprite_Font::initialize()
{
    if (s_internal != nullptr) {
        core::log::error("Attempting to initialize font libraries more than once");
        return true;
    }

    auto* ft_library = static_cast<FT_Library>(s_internal);
    const i32 result = FT_Init_FreeType(&ft_library);
    if (result != 0) {
        core::log::critical("Failed to initialize FreeType with error code {}", result);
        return false;
    }

    s_internal = ft_library;
    return true;
}

void
Sprite_Font::create(std::string filepath, const std::shared_ptr<Shader>& font_shader)
{
    m_filepath = std::move(filepath);
    m_shader = font_shader;
}

const Sprite_Font::Glyph&
Sprite_Font::get_glyph(u32 character, u32 font_size)
{
    if (m_font_cache.count(font_size) == 0) {
        auto* ft_library = static_cast<FT_Library>(s_internal);

        Size_Cache size_cache;
        size_cache.font_size = font_size;
        size_cache.next_char_texture_position = v2{ 0 };
        size_cache.texture = std::make_shared<Texture_2D>();
        size_cache.texture->create(m_max_texture_size, m_max_texture_size, nullptr);

        auto* ft_font_face = static_cast<FT_Face>(size_cache.font_face);
        if (FT_New_Face(ft_library, m_filepath.c_str(), 0, &ft_font_face) != 0) {
            core::log::error("Failed to create font face {} ({}) for character {}", m_filepath, font_size, character);
            return m_empty_glyph;
        }

        // TODO: Settle on a pixel multiplier as the default one seems really small? Maybe use Set_Char_Size?
        FT_Set_Pixel_Sizes(ft_font_face, 0, font_size);
        // FT_Set_Char_Size(ft_font_face, 0, static_cast<int>(font_size) * 64, 300, 300);

        size_cache.font_face = ft_font_face;
        m_font_cache[font_size] = size_cache;
    }

    auto& size_cache = m_font_cache[font_size];
    auto* font_face = static_cast<FT_Face>(size_cache.font_face);

    if (size_cache.glyph_cache.count(character) == 0) {
        if (FT_Load_Char(font_face, character, FT_LOAD_RENDER) != 0) { // NOLINT
            core::log::error("Failed to load character {} for font {} ({})", character, m_filepath, font_size);
            return m_empty_glyph;
        }

        auto temp_texture = std::make_shared<Texture_2D>();
        temp_texture->create(
            font_face->glyph->bitmap.width,
            font_face->glyph->bitmap.rows,
            font_face->glyph->bitmap.buffer,
            Texture_2D::Format::RED
        );

        if (size_cache.next_char_texture_position.x + temp_texture->width() > m_max_texture_size) {
            size_cache.next_char_texture_position.x = 0;
            size_cache.next_char_texture_position.y += font_size;

            if ((size_cache.next_char_texture_position.y + font_size) > m_max_texture_size) {
                core::log::error("Max texture size has already been reached for {} ({})", m_filepath, font_size);
                return m_empty_glyph;
            }
        }

        if (temp_texture->width() != 0 && temp_texture->height() != 0) {
            Sprite_Batch batch;
            batch.create(1, 1, m_shader);

            Frame_Buffer frame_buffer;
            // TODO: Get the actual window sizes.
            // /t    Probably go for a Font_Manager class which has access to window sizes & clear colour which either we can
            // /t    request a framebuffer from, or we can always go through that and it can handle any logic which isn't FT2.
            // /t    Alternatively some dynamic texture manager or service which we can just request a temporary framebuffer.
            frame_buffer.start(1600, 900, size_cache.texture);

            static constexpr auto mat_identity = m4{ 1.0f };
            auto projection =
                glm::ortho(0.0f, static_cast<f32>(m_max_texture_size), 0.0f, static_cast<f32>(m_max_texture_size), -1.0f, 1.0f);
            m_shader->bind();
            m_shader->set_mat4f("m_projection_view", projection * mat_identity);

            // TODO: Do we need to bother to clear our framebuffer here?

            batch.draw_texture(temp_texture, size_cache.next_char_texture_position);
            batch.flush();
            frame_buffer.end();
        }

        const auto texture_size_f = 1 / static_cast<f32>(m_max_texture_size);

        const auto sub_tex_uv_x = static_cast<f32>(size_cache.next_char_texture_position.x) * texture_size_f;
        const auto sub_tex_uv_y = static_cast<f32>(size_cache.next_char_texture_position.y) * texture_size_f;
        const auto sub_tex_uv_w = static_cast<f32>(temp_texture->width()) * texture_size_f;
        const auto sub_tex_uv_h = static_cast<f32>(temp_texture->height()) * texture_size_f;

        const auto sub_tex_coords = v4f{ sub_tex_uv_x, sub_tex_uv_y + sub_tex_uv_h, sub_tex_uv_x + sub_tex_uv_w, sub_tex_uv_y };

        Glyph glyph;
        glyph.sub_texture.create(temp_texture->width(), temp_texture->height(), sub_tex_coords, nullptr);
        glyph.texture = temp_texture;
        glyph.texture = size_cache.texture;
        glyph.advance = static_cast<u32>(font_face->glyph->advance.x);
        glyph.bearing = v2{ font_face->glyph->bitmap_left, font_face->glyph->bitmap_top };

        size_cache.glyph_cache[character] = glyph;
        size_cache.next_char_texture_position.x += temp_texture->width();
    }

    return m_font_cache[font_size].glyph_cache[character];
}
}
