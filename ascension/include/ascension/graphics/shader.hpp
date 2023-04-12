/**
 * File: shader.hpp
 * Project: ascension
 * File Created: 2023-04-11 20:33:16
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-12 15:22:47
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

#include <unordered_map>

namespace ascension::graphics {

enum class Shader_Data_Type {
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
    Bool
};

static constexpr uint32_t
size_of_shader_data_type(Shader_Data_Type type)
{
    switch (type) {
        case Shader_Data_Type::Float:
            return 4;
        case Shader_Data_Type::Float2:
            return 4 * 2;
        case Shader_Data_Type::Float3:
            return 4 * 3;
        case Shader_Data_Type::Float4:
            return 4 * 4;
        case Shader_Data_Type::Mat2:
            return 4 * 3 * 2;
        case Shader_Data_Type::Mat3:
            return 4 * 3 * 3;
        case Shader_Data_Type::Mat4:
            return 4 * 4 * 4;
        case Shader_Data_Type::Int:
            return 4;
        case Shader_Data_Type::Int2:
            return 4 * 2;
        case Shader_Data_Type::Int3:
            return 4 * 3;
        case Shader_Data_Type::Int4:
            return 4 * 4;
        case Shader_Data_Type::Bool:
            return 1;
        case Shader_Data_Type::None:
            return 0;
    }

    return 0;
}

class Shader {
  public:
    Shader();
    ~Shader();

    void create(const std::string& vertex_source, const std::string& fragment_source);
    void bind() const;
    static void unbind();

    void set_float(const std::string& name, f32 value, bool bind_shader = false);
    void set_float2(const std::string& name, f32 value_1, f32 value_2, bool bind_shader = false);
    void set_float3(const std::string& name, f32 value_1, f32 value_2, f32 value_3, bool bind_shader = false);
    void set_float4(const std::string& name, f32 value_1, f32 value_2, f32 value_3, f32 value_4, bool bind_shader = false);

    void set_vec2f(const std::string& name, const v2f& value, bool bind_shader = false);
    void set_vec3f(const std::string& name, const v3f& value, bool bind_shader = false);
    void set_vec4f(const std::string& name, const v4f& value, bool bind_shader = false);

    void set_int(const std::string& name, i32 value, bool bind_shader = false);
    void set_int2(const std::string& name, i32 value_1, i32 value_2, bool bind_shader = false);
    void set_int3(const std::string& name, i32 value_1, i32 value_2, i32 value_3, bool bind_shader = false);
    void set_int4(const std::string& name, i32 value_1, i32 value_2, i32 value_3, i32 value_4, bool bind_shader = false);

    void set_vec2i(const std::string& name, const v2i& value, bool bind_shader = false);
    void set_vec3i(const std::string& name, const v3i& value, bool bind_shader = false);
    void set_vec4i(const std::string& name, const v4i& value, bool bind_shader = false);

    void set_mat2f(const std::string& name, const m2f& value, bool bind_shader = false);
    void set_mat3f(const std::string& name, const m3f& value, bool bind_shader = false);
    void set_mat4f(const std::string& name, const m4f& value, bool bind_shader = false);

    Shader(const Shader&) = default;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = default;
    Shader& operator=(Shader&&) = delete;

  private:
    i32 get_uniform_location(const std::string& name);

    u32 m_id;
    std::unordered_map<std::string, i32> m_uniform_cache;
};

}
