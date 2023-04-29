/**
 * File: renderer_2d.hpp
 * Project: ascension
 * File Created: 2023-04-29 16:55:06
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-29 17:04:06
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

#pragma once

namespace ascension::graphics {

enum class Blend_Function : u32 {
    SRC_COLOR,
    DST_COLOR,
    SRC_ALPHA,
    DST_ALPHA,
};

class Renderer_2D {
  public:
    static bool initialize();

    static void set_clear_color(v4f color);
    static void clear();

    static void enable_blending(Blend_Function blend_func = Blend_Function::SRC_ALPHA);
};

}
