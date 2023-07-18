/**
 * File: sprite_font.cpp
 * Project: ascension
 * File Created: 2023-07-17 21:08:32
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-18 21:08:35
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

#include "core/log.hpp"

#include "graphics/frame_buffer.hpp"
#include "graphics/renderer_2d.hpp"
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

    return true;
}

void
Sprite_Font::create(std::string filepath)
{
    m_filepath = std::move(filepath);
}

const Sprite_Font::Glyph&
Sprite_Font::get_glyph(u32 character, u32 font_size)
{
    if (m_font_cache.count(font_size) == 0) {
        auto* ft_library = static_cast<FT_Library>(s_internal);

        Size_Cache size_cache;
        size_cache.font_size = font_size;
        size_cache.next_char_texture_position = v2{ 0 };
        // TODO: We need to dynamically allocate the memory needed for this texture; this would be best done through
        // /t    through a (dynamic) texture manager.
        size_cache.texture->create(m_max_texture_size, m_max_texture_size, nullptr);

        auto* ft_font_face = static_cast<FT_Face>(size_cache.font_face);

        if (FT_New_Face(ft_library, m_filepath.c_str(), 0, &ft_font_face) != 0) {
            core::log::error("Failed to get character {} for font {} ({})", character, m_filepath, font_size);
            // TODO: We need an empty/failure glyph to return here.
        }

        FT_Set_Pixel_Sizes(ft_font_face, 0, font_size);

        m_font_cache[font_size] = size_cache;
    }

    auto size_cache = m_font_cache[font_size];

    if (size_cache.glyph_cache.count(character) == 0) {
        // TODO: We need to create and load a default font shader for drawing our glyphs to a texture.
        Batch_Config batch_config{ 1, size_cache.texture, nullptr, true };
        Batch draw_batch{ batch_config };

        Frame_Buffer frame_buffer;
        // TODO: Get the actual window sizes.
        frame_buffer.start(1600, 900, size_cache.texture);

        if (size_cache.next_char_texture_position == v2{ 0, 0 }) {
            // TODO: Get/restore the previous clear color.
            Renderer_2D::set_clear_color(v4f{ 0.0f });
            Renderer_2D::clear();
        }

        draw_batch.add(size_cache.texture, size_cache.next_char_texture_position);
        draw_batch.flush();
        frame_buffer.end();
    }

    // TODO: Setup the sub_texture for the glyph and then create the glyph.

    return size_cache.glyph_cache[character];
}

}
