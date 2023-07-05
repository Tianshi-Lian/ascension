/**
 * File: buffer_object.hpp
 * Project: erika
 * File Created: 2023-04-24 13:50:51
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-24 14:12:26
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

namespace erika::plugins::renderer::opengl {

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

class Vertex_Buffer_Object : public Buffer_Object {
public:
    Vertex_Buffer_Object();
    ~Vertex_Buffer_Object() override = default;

    void draw_arrays(i32 start_index, i32 count, Draw_Mode mode);

    Vertex_Buffer_Object(const Vertex_Buffer_Object&) = default;
    Vertex_Buffer_Object(Vertex_Buffer_Object&&) = delete;
    Vertex_Buffer_Object& operator=(const Vertex_Buffer_Object&) = default;
    Vertex_Buffer_Object& operator=(Vertex_Buffer_Object&&) = delete;
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
