/**
 * File: vertex_array_object.cpp
 * Project: ascension
 * File Created: 2023-04-12 20:54:24
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-12 21:38:50
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

#include "graphics/vertex_array_object.hpp"

#include <GL/glew.h>

#include "graphics/buffer_object.hpp"
#include "graphics/shader.hpp"

namespace ascension::graphics {

// Vertex_Attrib_Floats
Vertex_Attrib_Floats::Vertex_Attrib_Floats(i32 float_count, bool should_normalize)
  : count(float_count)
  , normalize(should_normalize)
{
}

// Vertex_Array_Object
Vertex_Array_Object::Vertex_Array_Object()
  : m_id(0)
  , m_current_attrib_index(0)
  , m_is_bound(false)
  , m_vertex_buffer(nullptr)
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
Vertex_Array_Object::set_vertex_buffer(const std::shared_ptr<Vertex_Buffer_Object>& vertex_buffer)
{
    m_vertex_buffer = vertex_buffer;
}

void
Vertex_Array_Object::set_index_buffer(const std::shared_ptr<Index_Buffer_Object>& index_buffer)
{
    m_index_buffer = index_buffer;
}

void
Vertex_Array_Object::set_attrib_ptr(i32 size, i32 stride, const void* offset, bool normalize)
{
    glVertexAttribPointer(m_current_attrib_index, size, GL_FLOAT, normalize ? GL_TRUE : GL_FALSE, stride, offset);
    glEnableVertexAttribArray(m_current_attrib_index);
    ++m_current_attrib_index;
}

void
Vertex_Array_Object::set_attrib_ptr_list(const Vertex_Attrib_Float_List& list)
{
    i32 stride = 0;
    for (const auto& var : list) {
        stride += static_cast<i32>(size_of_shader_data_type(Shader_Data_Type::Float)) * var.count;
    }

    intptr_t offset = 0;
    for (const auto& var : list) {
        set_attrib_ptr(var.count, stride, static_cast<void*>(&offset), var.normalize);
        offset += static_cast<intptr_t>(size_of_shader_data_type(Shader_Data_Type::Float)) * var.count;
    }
}

bool
Vertex_Array_Object::is_bound() const
{
    return m_is_bound;
}

}
