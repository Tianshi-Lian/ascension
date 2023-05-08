/**
 * File: shader.cpp
 * Project: ascension
 * File Created: 2023-04-11 20:36:05
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-08 21:25:19
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

#include "graphics/shader.hpp"

#include <array>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/log.hpp"

namespace {

constexpr size_t Info_Log_Size = 1024;

enum class Error_Check_Type {
    SHADER,
    LINK
};

bool
check_shader_errors(u32 object, Error_Check_Type type)
{
    i32 success = 1;
    auto info_log = std::array<char, Info_Log_Size>();
    switch (type) {
        case Error_Check_Type::SHADER:
            glGetShaderiv(object, GL_COMPILE_STATUS, &success);
            break;
        case Error_Check_Type::LINK:
            glGetShaderiv(object, GL_LINK_STATUS, &success);
            break;
    }

    if (success == 0) {
        glGetShaderInfoLog(object, Info_Log_Size, nullptr, info_log.data());
        ascension::core::log::error("Errors find when creating shader.");
        ascension::core::log::error("{}", info_log);
        return false;
    }

    return true;
}

}

namespace ascension::graphics {

Shader::Shader()
  : m_id(0)
{
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void
Shader::create(const std::string& vertex_source, const std::string& fragment_source)
{
    const char* cstr_vertex_source = vertex_source.c_str();
    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &cstr_vertex_source, nullptr);
    glCompileShader(vertex_shader);
    if (!check_shader_errors(vertex_shader, Error_Check_Type::SHADER)) {
        glDeleteShader(vertex_shader);
        return;
    }

    const char* cstr_fragment_source = fragment_source.c_str();
    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &cstr_fragment_source, nullptr);
    glCompileShader(fragment_shader);
    if (!check_shader_errors(fragment_shader, Error_Check_Type::SHADER)) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return;
    }

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex_shader);
    glAttachShader(m_id, fragment_shader);
    glLinkProgram(m_id);
    if (!check_shader_errors(m_id, Error_Check_Type::LINK)) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glDeleteProgram(m_id);
        return;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void
Shader::bind() const
{
    glUseProgram(m_id);
}

void
Shader::unbind()
{
    glUseProgram(0);
}

i32
Shader::get_uniform_location(const std::string& name)
{
    if (m_uniform_cache.count(name) == 0) {
        const auto location = glGetUniformLocation(m_id, name.c_str());
        m_uniform_cache[name] = location;
    }

    return m_uniform_cache.at(name);
}

u32
Shader::id() const
{
    return m_id;
}

// Shader uniform variable setters.
void
Shader::set_float(const std::string& name, f32 value, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniform1f(get_uniform_location(name), value);
}
void
Shader::set_float2(const std::string& name, f32 value_1, f32 value_2, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniform2f(get_uniform_location(name), value_1, value_2);
}
void
Shader::set_float3(const std::string& name, f32 value_1, f32 value_2, f32 value_3, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniform3f(get_uniform_location(name), value_1, value_2, value_3);
}
void
Shader::set_float4(const std::string& name, f32 value_1, f32 value_2, f32 value_3, f32 value_4, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniform4f(get_uniform_location(name), value_1, value_2, value_3, value_4);
}
void
Shader::set_vec2f(const std::string& name, const v2f& value, bool bind_shader)
{
    set_float2(name, value.x, value.y, bind_shader);
}
void
Shader::set_vec3f(const std::string& name, const v3f& value, bool bind_shader)
{
    set_float3(name, value.x, value.y, value.z, bind_shader);
}
void
Shader::set_vec4f(const std::string& name, const v4f& value, bool bind_shader)
{
    set_float4(name, value.x, value.y, value.z, value.w, bind_shader);
}
void
Shader::set_int(const std::string& name, i32 value, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniform1i(get_uniform_location(name), value);
}
void
Shader::set_int2(const std::string& name, i32 value_1, i32 value_2, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniform2i(get_uniform_location(name), value_1, value_2);
}
void
Shader::set_int3(const std::string& name, i32 value_1, i32 value_2, i32 value_3, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniform3i(get_uniform_location(name), value_1, value_2, value_3);
}
void
Shader::set_int4(const std::string& name, i32 value_1, i32 value_2, i32 value_3, i32 value_4, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniform4i(get_uniform_location(name), value_1, value_2, value_3, value_4);
}
void
Shader::set_vec2i(const std::string& name, const v2i& value, bool bind_shader)
{
    set_int2(name, value.x, value.y, bind_shader);
}
void
Shader::set_vec3i(const std::string& name, const v3i& value, bool bind_shader)
{
    set_int3(name, value.x, value.y, value.z, bind_shader);
}
void
Shader::set_vec4i(const std::string& name, const v4i& value, bool bind_shader)
{
    set_int4(name, value.x, value.y, value.z, value.w, bind_shader);
}
void
Shader::set_mat2f(const std::string& name, const m2f& value, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniformMatrix2fv(get_uniform_location(name), 1, 0u, glm::value_ptr(value));
}
void
Shader::set_mat3f(const std::string& name, const m3f& value, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniformMatrix3fv(get_uniform_location(name), 1, 0u, glm::value_ptr(value));
}
void
Shader::set_mat4f(const std::string& name, const m4f& value, bool bind_shader)
{
    if (bind_shader) {
        bind();
    }
    glUniformMatrix4fv(get_uniform_location(name), 1, 0u, glm::value_ptr(value));
}

}
