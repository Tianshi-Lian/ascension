/**
 * File: sprite_font.cpp
 * Project: ascension
 * File Created: 2023-07-17 21:08:32
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-17 21:30:34
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

#include "core/log.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

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

}
