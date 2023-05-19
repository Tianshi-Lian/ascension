/**
 * File: buffer_object.hpp
 * Project: ascension
 * File Created: 2023-04-12 14:35:15
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-19 20:24:40
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

#include "graphics/shader_data_types.hpp"

namespace ascension::graphics {

enum class Buffer_Type : u32 {
    Unknown,
    Vertex,
    Index
};

enum class Draw_Mode : u32 {
    Points,
    Lines,
    Triangles,
};

class Buffer_Object {
  public:
    Buffer_Object();
    explicit Buffer_Object(Buffer_Type type);
    virtual ~Buffer_Object();

    // TODO: Add and check create result.
    void create(u32 size);
    void create(u32 size, const void* data);

    void bind();
    void unbind();

    void buffer_data(u32 size, const void* data);
    void buffer_sub_data(i32 offset, u32 size, const void* data);

    [[nodiscard]] bool is_bound() const;

    Buffer_Object(const Buffer_Object&) = default;
    Buffer_Object(Buffer_Object&&) = delete;
    Buffer_Object& operator=(const Buffer_Object&) = default;
    Buffer_Object& operator=(Buffer_Object&&) = delete;

  private:
    u32 m_id;
    u32 m_buffer_type;
    bool m_is_bound;
};

struct Vertex_Object_Element {
    Vertex_Object_Element() = default;
    Vertex_Object_Element(Shader_Data_Type _type, i32 _count, bool _normalize);

    Shader_Data_Type type;
    i32 count;
    bool normalize;
};
using Vertex_Buffer_Layout = std::vector<Vertex_Object_Element>;

class Vertex_Buffer_Object : public Buffer_Object {
  public:
    Vertex_Buffer_Object();
    ~Vertex_Buffer_Object() override = default;

    void set_layout(const Vertex_Buffer_Layout& layout);
    [[nodiscard]] const Vertex_Buffer_Layout& get_layout() const;

    void draw_arrays(i32 start_index, i32 count, Draw_Mode mode);

    Vertex_Buffer_Object(const Vertex_Buffer_Object&) = default;
    Vertex_Buffer_Object(Vertex_Buffer_Object&&) = delete;
    Vertex_Buffer_Object& operator=(const Vertex_Buffer_Object&) = default;
    Vertex_Buffer_Object& operator=(Vertex_Buffer_Object&&) = delete;

  private:
    Vertex_Buffer_Layout m_layout;
};

class Index_Buffer_Object : public Buffer_Object {
  public:
    Index_Buffer_Object();
    ~Index_Buffer_Object() override = default;

    void draw_elements(i32 count, Draw_Mode mode);

    Index_Buffer_Object(const Index_Buffer_Object&) = default;
    Index_Buffer_Object(Index_Buffer_Object&&) = delete;
    Index_Buffer_Object& operator=(const Index_Buffer_Object&) = default;
    Index_Buffer_Object& operator=(Index_Buffer_Object&&) = delete;
};

}
