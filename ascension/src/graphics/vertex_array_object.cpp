/**
 * File: vertex_array_object.cpp
 * Project: ascension
 * File Created: 2023-04-12 20:54:24
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-08 21:04:01
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

#include <array>

#include "graphics/vertex_array_object.hpp"

#include <GL/glew.h>

#include "core/log.hpp"
#include "graphics/buffer_object.hpp"
#include "graphics/shader_data_types.hpp"

namespace {

constexpr GLenum
shader_type_to_opengl(ascension::graphics::Shader_Data_Type type)
{
    switch (type) {
        case ascension::graphics::Shader_Data_Type::Float:
        case ascension::graphics::Shader_Data_Type::Float2:
        case ascension::graphics::Shader_Data_Type::Float3:
        case ascension::graphics::Shader_Data_Type::Float4:
        case ascension::graphics::Shader_Data_Type::Mat2:
        case ascension::graphics::Shader_Data_Type::Mat3:
        case ascension::graphics::Shader_Data_Type::Mat4:
            return GL_FLOAT;
        case ascension::graphics::Shader_Data_Type::Int:
        case ascension::graphics::Shader_Data_Type::Int2:
        case ascension::graphics::Shader_Data_Type::Int3:
        case ascension::graphics::Shader_Data_Type::Int4:
            return GL_INT;
        case ascension::graphics::Shader_Data_Type::Bool:
            return GL_BOOL;
        default:
            ascension::core::log::error("shader_type_to_opengl() Invalid shader_data_type {}", magic_enum::enum_name(type));
            return 0;
    }

    return 0;
}

}

namespace ascension::graphics {

// Vertex_Array_Object
Vertex_Array_Object::Vertex_Array_Object()
  : m_id(0)
  , m_current_attrib_index(0)
  , m_is_bound(false)
  , m_index_buffer(nullptr)
{
}

Vertex_Array_Object::~Vertex_Array_Object()
{
    glDeleteVertexArrays(1, &m_id);
}

void
Vertex_Array_Object::create(bool bind_buffer)
{
    glGenVertexArrays(1, &m_id);

    if (bind_buffer) {
        bind();
    }
}

void
Vertex_Array_Object::bind()
{
    glBindVertexArray(m_id);
    m_is_bound = true;
}

void
Vertex_Array_Object::unbind()
{
    glBindVertexArray(0);
    m_is_bound = false;
}

void
Vertex_Array_Object::set_index_buffer(const std::shared_ptr<Index_Buffer_Object>& index_buffer)
{
    m_index_buffer = index_buffer;
}

void
Vertex_Array_Object::add_vertex_buffer(const std::shared_ptr<Vertex_Buffer_Object>& vertex_buffer)
{
    m_vertex_buffers.push_back(vertex_buffer);

    const auto list = vertex_buffer->get_layout();
    i32 stride = 0;
    if (list.size() > 1) {
        for (const auto& var : list) {
            stride += size_of_shader_data_type(var.type, var.count);
        }
    }

    i32 offset = 0;
    for (const auto& var : list) {
        // TODO: Change this to use opengl 4.3 methods
        /**
         *  //Offset specified as integer.
         * glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);
         * glVertexAttribBinding(1, 0);

         * //Some later point when you're ready to provide a buffer.
         * //Stride goes into the buffer binding.
         * glBindVertexBuffer(0, buffer_obj, 0, sizeof(GLfloat) * 8);
         */

        glVertexAttribPointer(
            m_current_attrib_index,
            var.count,
            shader_type_to_opengl(var.type),
            var.normalize ? GL_TRUE : GL_FALSE,
            stride,
            reinterpret_cast<void*>(offset) // NOLINT
        );
        glEnableVertexAttribArray(m_current_attrib_index);
        ++m_current_attrib_index;
        offset += size_of_shader_data_type(var.type, var.count);
    }
}

bool
Vertex_Array_Object::is_bound() const
{
    return m_is_bound;
}

}
