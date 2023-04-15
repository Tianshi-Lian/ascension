/**
 * File: vertex_array_object.hpp
 * Project: ascension
 * File Created: 2023-04-12 16:52:11
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-15 15:37:14
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

namespace ascension::graphics {

class Vertex_Buffer_Object;
class Index_Buffer_Object;

struct Vertex_Attrib_Floats {
    explicit Vertex_Attrib_Floats(i32 float_count, bool should_normalize = false);

    i32 count;
    bool normalize;
};
using Vertex_Attrib_Float_List = std::vector<Vertex_Attrib_Floats>;

class Vertex_Array_Object {
  public:
    Vertex_Array_Object();
    ~Vertex_Array_Object();

    void create(bool bind = false);
    void bind();
    void unbind();

    void set_vertex_buffer(const std::shared_ptr<Vertex_Buffer_Object>& vertex_buffer);
    void set_index_buffer(const std::shared_ptr<Index_Buffer_Object>& index_buffer);

    void set_attrib_ptr(i32 size, i32 stride, const void* offset, bool normalize = false);
    void set_attrib_ptr_list(const Vertex_Attrib_Float_List& list);

    [[nodiscard]] bool is_bound() const;

    Vertex_Array_Object(const Vertex_Array_Object&) = default;
    Vertex_Array_Object(Vertex_Array_Object&&) = delete;
    Vertex_Array_Object& operator=(const Vertex_Array_Object&) = default;
    Vertex_Array_Object& operator=(Vertex_Array_Object&&) = delete;

  private:
    u32 m_id;
    u32 m_current_attrib_index;
    bool m_is_bound;

    std::shared_ptr<Vertex_Buffer_Object> m_vertex_buffer;
    std::shared_ptr<Index_Buffer_Object> m_index_buffer;
};

}
