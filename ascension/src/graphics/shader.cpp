/**
 * File: shader.cpp
 * Project: ascension
 * File Created: 2023-04-11 20:36:05
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-11 20:53:17
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

#include "yuki/debug/logger.hpp"

namespace {

constexpr size_t Info_Log_Size = 1024;

enum class Error_Check_Type {
    SHADER,
    LINK
};

bool
check_shader_errors(u32 object, Error_Check_Type type)
{
    int success = 1;
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
        yuki::debug::Logger::error("ascension", "Errors find when creating shader.");
        yuki::debug::Logger::error("ascension", "{}", info_log);
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

}
