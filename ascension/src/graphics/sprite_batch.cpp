/**
 * File: sprite_batch.cpp
 * Project: ascension
 * File Created: 2023-04-15 14:54:44
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-15 15:43:50
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

#include "graphics/sprite_batch.hpp"

#include <cstddef>

#include "graphics/buffer_object.hpp"
#include "graphics/shader.hpp"
// #include "graphics/texture_2d.hpp"
#include "graphics/vertex_array_object.hpp"

#include "yuki/debug/logger.hpp"

namespace ascension::graphics {

struct Sprite_Batch_Vertex {
    v2f position;
    v2f tex_coords;
    v4f color;
};

static constexpr u32 QUAD_VERTEX_COUNT = 4;
static constexpr u32 QUAD_INDEX_COUNT = 6;
// static constexpr f32 TEMP_VERTEX_BUFFER[] = {
//     0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
// };

Sprite_Batch::Sprite_Batch()
  : m_screen_width(0)
  , m_screen_height(0)
  , m_max_batch_size(0)
{
}

void
Sprite_Batch::initialize(u32 screen_width, u32 screen_height, u32 max_batch_size)
{
    if (m_max_batch_size > 0) {
        yuki::debug::Logger::error("ascension", "Sprite_Batch should only be initialized once!");
        return;
    }

    m_screen_width = screen_width;
    m_screen_height = screen_height;
    m_max_batch_size = max_batch_size;

    m_sprite_shader->bind();
    m_sprite_shader->set_int("u_texture", 0);

    std::array<u32, QUAD_INDEX_COUNT> indices_template = { 0, 1, 2, 0, 3, 1 };
    std::vector<u32> indices(static_cast<size_t>(QUAD_INDEX_COUNT * m_max_batch_size));

    for (u32 i = 0; i < m_max_batch_size; ++i) {
        const auto start_index = i * QUAD_INDEX_COUNT;
        const auto vertex_offset = i * QUAD_VERTEX_COUNT;

        indices[start_index + 0] = indices_template[0] + vertex_offset;
        indices[start_index + 1] = indices_template[1] + vertex_offset;
        indices[start_index + 2] = indices_template[2] + vertex_offset;

        indices[start_index + 3] = indices_template[3] + vertex_offset;
        indices[start_index + 4] = indices_template[4] + vertex_offset;
        indices[start_index + 5] = indices_template[5] + vertex_offset; // NOLINT
    }

    static const Vertex_Attrib_Float_List vertex_map{ Vertex_Attrib_Floats(2, false),
                                                      Vertex_Attrib_Floats(2, false),
                                                      Vertex_Attrib_Floats(4, false) };

    m_vertex_array = std::make_unique<Vertex_Array_Object>();
    m_vertex_array->create(true);

    m_vertex_buffer = std::make_shared<Vertex_Buffer_Object>();
    m_vertex_buffer->create(sizeof(Sprite_Batch_Vertex) * m_max_batch_size * QUAD_VERTEX_COUNT);

    m_index_buffer = std::make_shared<Index_Buffer_Object>();
    m_index_buffer->create(sizeof(f32) * static_cast<u32>(indices.size()), indices.data());

    m_vertex_array->set_vertex_buffer(m_vertex_buffer);
    m_vertex_array->set_index_buffer(m_index_buffer);
    m_vertex_array->set_attrib_ptr_list(vertex_map);
    m_vertex_array->unbind();
}

}
