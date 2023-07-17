/**
 * File: sprite_font.hpp
 * Project: ascension
 * File Created: 2023-07-17 20:38:36
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-17 21:30:52
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

#ifndef ASCENSION_GRAPHICS_SPRITE_FONT_HPP
#define ASCENSION_GRAPHICS_SPRITE_FONT_HPP

#include "graphics/texture_2d.hpp"

namespace ascension::graphics {

class Sprite_Font {
public:
    struct Glyph {
        Texture_2D sub_texture;

        v2 bearing;
        u32 advance;
    };
    using Glyph_Cache = std::map<u32, Sprite_Font::Glyph>;

    struct Size_Cache {
        u32 font_size;

        v2 next_char_texture_position;
        std::shared_ptr<Texture_2D> texture;

        Glyph_Cache glyph_cache;

        void* font_face;
    };
    using Font_Cache = std::map<u32, Size_Cache>;

    static constexpr u32 DEFAULT_TEXTURE_SIZE = 2048;

    Sprite_Font();
    ~Sprite_Font();

    static bool initialize();

    void create(std::string filepath);

    [[nodiscard]] static bool is_initialized();

    [[nodiscard]] const Glyph& get_glyph(u32 character, u32 font_size);
    [[nodiscard]] const v2 measure_string(const std::string& value, u32 font_size);

    Sprite_Font(const Sprite_Font&) = default;
    Sprite_Font(Sprite_Font&&) = delete;
    Sprite_Font& operator=(const Sprite_Font&) = default;
    Sprite_Font& operator=(Sprite_Font&&) = delete;

private:
    u32 m_max_texture_size;

    std::string m_filepath;
    Font_Cache m_font_cache;

    static void* s_internal;
};

}

#endif // ASCENSION_GRAPHICS_SPRITE_FONT_HPP
