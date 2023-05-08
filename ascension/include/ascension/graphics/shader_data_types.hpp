/**
 * File: shader_data_types.hpp
 * Project: ascension
 * File Created: 2023-05-08 20:36:09
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-08 20:51:53
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

#ifndef ASCENSION_GRAPHICS_SHADER_DATA_TYPES_HPP
#define ASCENSION_GRAPHICS_SHADER_DATA_TYPES_HPP

namespace ascension::graphics {

enum class Shader_Data_Type : u32 {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat2,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool,
    COUNT
};

static constexpr i32
size_of_shader_data_type(Shader_Data_Type type, i32 count = 1)
{
    switch (type) {
        case Shader_Data_Type::Float:
            return 4 * count;
        case Shader_Data_Type::Float2:
            return 4 * 2 * count;
        case Shader_Data_Type::Float3:
            return 4 * 3 * count;
        case Shader_Data_Type::Float4:
            return 4 * 4 * count;
        case Shader_Data_Type::Mat2:
            return 4 * 3 * 2 * count;
        case Shader_Data_Type::Mat3:
            return 4 * 3 * 3 * count;
        case Shader_Data_Type::Mat4:
            return 4 * 4 * 4 * count;
        case Shader_Data_Type::Int:
            return 4 * count;
        case Shader_Data_Type::Int2:
            return 4 * 2 * count;
        case Shader_Data_Type::Int3:
            return 4 * 3 * count;
        case Shader_Data_Type::Int4:
            return 4 * 4 * count;
        case Shader_Data_Type::Bool:
            return 1 * count;
        default:
            return 0;
    }

    return 0;
}

}

#endif // ASCENSION_GRAPHICS_SHADER_DATA_TYPES_HPP
