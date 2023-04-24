/**
 * File: vertex_array_object.cpp
 * Project: erika
 * File Created: 2023-04-24 13:51:16
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-24 14:48:01
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

#include "vertex_array_object.hpp"

#include <GL/glew.h>

#include "buffer_object.hpp"
#include "shader.hpp"

namespace erika::plugins::renderer::opengl {

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

std::shared_ptr<Vertex_Buffer_Object>
Vertex_Array_Object::add_vertex_buffer(u32 size, const void* data)
{
    m_vertex_buffer = std::make_shared<Vertex_Buffer_Object>();
    m_vertex_buffer->create(size, data);
    return m_vertex_buffer;
}

std::shared_ptr<Index_Buffer_Object>
Vertex_Array_Object::add_index_buffer(u32 size, const void* data)
{
    m_index_buffer = std::make_shared<Index_Buffer_Object>();
    m_index_buffer->create(size, data);
    return m_index_buffer;
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
    // TODO: Change this to use opengl 4.3 methods
    /**
     *  //Offset specified as integer.
     * glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);
     * glVertexAttribBinding(1, 0);

     * //Some later point when you're ready to provide a buffer.
     * //Stride goes into the buffer binding.
     * glBindVertexBuffer(0, buffer_obj, 0, sizeof(GLfloat) * 8);
     */
    glVertexAttribPointer(m_current_attrib_index, size, GL_FLOAT, normalize ? GL_TRUE : GL_FALSE, stride, offset);
    glEnableVertexAttribArray(m_current_attrib_index);
    ++m_current_attrib_index;
}

void
Vertex_Array_Object::set_attrib_ptr_list(const Vertex_Attrib_Float_List& list)
{
    i32 stride = 0;
    if (list.size() > 1) {
        for (const auto& var : list) {
            stride += size_of_shader_data_type(Shader_Data_Type::Float, var.count);
        }
    }

    i32 offset = 0;
    for (const auto& var : list) {
        set_attrib_ptr(var.count, stride, reinterpret_cast<void*>(offset), var.normalize); // NOLINT
        offset += size_of_shader_data_type(Shader_Data_Type::Float, var.count);
    }
}

bool
Vertex_Array_Object::is_bound() const
{
    return m_is_bound;
}

}
