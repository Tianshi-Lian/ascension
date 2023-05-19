/**
 * File: buffer_object.cpp
 * Project: ascension
 * File Created: 2023-04-12 15:45:35
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-19 20:25:03
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

#include "graphics/buffer_object.hpp"

#include <GL/glew.h>

#include "core/log.hpp"

namespace {

constexpr u32
gl_target_type(ascension::graphics::Buffer_Type type)
{
    switch (type) {
        case ascension::graphics::Buffer_Type::Vertex:
            return GL_ARRAY_BUFFER;
        case ascension::graphics::Buffer_Type::Index:
            return GL_ELEMENT_ARRAY_BUFFER;
        default:
            return 0;
    }
}

constexpr u32
gl_draw_mode(ascension::graphics::Draw_Mode mode)
{
    switch (mode) {
        case ascension::graphics::Draw_Mode::Points:
            return GL_POINTS;
        case ascension::graphics::Draw_Mode::Lines:
            return GL_LINES;
        case ascension::graphics::Draw_Mode::Triangles:
            return GL_TRIANGLES;
        default:
            return 0;
    }
}

}

namespace ascension::graphics {

// Buffer_Object
Buffer_Object::Buffer_Object()
  : m_id(0)
  , m_buffer_type(0)
  , m_is_bound(false)
{
}

Buffer_Object::Buffer_Object(Buffer_Type type)
  : m_id(0)
  , m_buffer_type(gl_target_type(type))
  , m_is_bound(false)
{
}

Buffer_Object::~Buffer_Object()
{
    if (m_id > 0) {
        glDeleteBuffers(1, &m_id);
    }
}

void
Buffer_Object::create(u32 size)
{
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(m_buffer_type, size, nullptr, GL_DYNAMIC_DRAW);
}

void
Buffer_Object::create(u32 size, const void* data)
{
    if (data == nullptr) {
        return create(size);
    }

    glGenBuffers(1, &m_id);
    bind();
    glBufferData(m_buffer_type, size, data, GL_STATIC_DRAW);
}

void
Buffer_Object::bind()
{
    glBindBuffer(m_buffer_type, m_id);
    m_is_bound = true;
}

void
Buffer_Object::unbind()
{
    glBindBuffer(m_buffer_type, 0);
    m_is_bound = false;
}

void
Buffer_Object::buffer_data(u32 size, const void* data)
{
    bind();
    glBufferSubData(m_buffer_type, 0, size, data);
}

void
Buffer_Object::buffer_sub_data(i32 offset, u32 size, const void* data)
{
    bind();
    glBufferSubData(m_buffer_type, offset, size, data);
}

bool
Buffer_Object::is_bound() const
{
    return m_is_bound;
}

// Vertex_Object_Element
Vertex_Object_Element::Vertex_Object_Element(Shader_Data_Type _type, i32 _count, bool _normalize)
  : type(_type)
  , count(_count)
  , normalize(_normalize)
{
}

// Vertex_Buffer_Object
Vertex_Buffer_Object::Vertex_Buffer_Object()
  : Buffer_Object(Buffer_Type::Vertex)
{
}

void
Vertex_Buffer_Object::set_layout(const Vertex_Buffer_Layout& layout)
{
    m_layout = layout;
}

const Vertex_Buffer_Layout&
Vertex_Buffer_Object::get_layout() const
{
    return m_layout;
}

void
Vertex_Buffer_Object::draw_arrays(i32 start_index, i32 count, Draw_Mode mode)
{
    if (!is_bound()) {
        core::log::error("Vertex_Buffer_Object::draw_arrays(): attempting to draw unbound buffer!");
        return;
    }

    glDrawArrays(gl_draw_mode(mode), start_index, count);
}

// Index_Buffer_Object
Index_Buffer_Object::Index_Buffer_Object()
  : Buffer_Object(Buffer_Type::Index)
{
}

void
Index_Buffer_Object::draw_elements(i32 count, Draw_Mode mode)
{
    if (!is_bound()) {
        core::log::error("Vertex_Buffer_Object::draw_elements(): attempting to draw unbound buffer!");
        return;
    }

    glDrawElements(gl_draw_mode(mode), count, GL_UNSIGNED_INT, nullptr);
}

}
