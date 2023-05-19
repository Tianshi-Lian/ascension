/**
 * File: sprite_batch.cpp
 * Project: ascension
 * File Created: 2023-04-15 14:54:44
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-19 20:21:04
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
#include <limits>

#include <glm/ext/matrix_clip_space.hpp>

#include "yuki/debug/instrumentor.hpp"

#include "core/log.hpp"
#include "graphics/buffer_object.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture_2d.hpp"
#include "graphics/vertex_array_object.hpp"

#include <GL/glew.h>

namespace ascension::graphics {

static constexpr u32 QUAD_VERTEX_COUNT = 4;
static constexpr u32 QUAD_VERTEX_COMPONENT_COUNT = QUAD_VERTEX_COUNT * 2;
static constexpr u32 QUAD_INDEX_COUNT = 6;

// Batch
Batch::Batch()
  : m_current_size(0)
{
}

void
Batch::create(const Batch_Config& config)
{
    m_config = config;

    m_vertex_positions.reserve(static_cast<size_t>(m_config.max_size) * QUAD_VERTEX_COMPONENT_COUNT);
    m_texture_coords.reserve(static_cast<size_t>(m_config.max_size) * QUAD_VERTEX_COMPONENT_COUNT);

    m_vao = std::make_unique<Vertex_Array_Object>();
    m_vao->create(true);

    m_vbo = std::make_shared<Vertex_Buffer_Object>();
    m_vbo->create(sizeof(f32) * m_config.max_size * QUAD_VERTEX_COMPONENT_COUNT);
    m_vbo->set_layout({ { Shader_Data_Type::Float, 2, false } });
    m_vao->add_vertex_buffer(m_vbo);

    m_ubo = std::make_shared<Vertex_Buffer_Object>();
    m_ubo->create(sizeof(f32) * m_config.max_size * QUAD_VERTEX_COMPONENT_COUNT);
    m_ubo->set_layout({ { Shader_Data_Type::Float, 2, true } });
    m_vao->add_vertex_buffer(m_ubo);

    static const std::array<u32, 6> indices_template{ 0, 1, 2, 2, 3, 0 };
    std::vector<u32> indices(static_cast<size_t>(QUAD_INDEX_COUNT) * m_config.max_size);

    for (u32 i = 0; i < m_config.max_size; ++i) {
        const u32 offset = i * QUAD_INDEX_COUNT;
        const u32 vertex_offset = i * QUAD_VERTEX_COUNT;

        indices.at(offset + 0) = (indices_template.at(0) + vertex_offset);
        indices.at(offset + 1) = (indices_template.at(1) + vertex_offset);
        indices.at(offset + 2) = (indices_template.at(2) + vertex_offset);
        indices.at(offset + 3) = (indices_template.at(3) + vertex_offset);
        indices.at(offset + 4) = (indices_template.at(4) + vertex_offset);
        indices.at(offset + 5) = (indices_template.at(5) + vertex_offset); // NOLINT
    }

    m_ibo = std::make_shared<Index_Buffer_Object>();
    m_ibo->create(sizeof(u32) * m_config.max_size * QUAD_INDEX_COUNT, indices.data());
    m_vao->set_index_buffer(m_ibo);

    m_vao->unbind();
}

void
Batch::add(v2f position, v2f size, v4f tex_coords)
{
    PROFILE_FUNCTION();

    if (m_config.max_size == 0) {
        core::log::error("Attempting to add texture to uninitialized batch");
        return;
    }

    // TODO: Work out if we just want to immediately draw here instead.
    if (m_current_size >= m_config.max_size) {
        core::log::warn("Attempting to add texture to full batch.");
        return;
    }

    m_vertex_positions.emplace_back(position.x);
    m_vertex_positions.emplace_back(position.y);

    m_vertex_positions.emplace_back(position.x);
    m_vertex_positions.emplace_back(position.y + size.y);

    m_vertex_positions.emplace_back(position.x + size.x);
    m_vertex_positions.emplace_back(position.y + size.y);

    m_vertex_positions.emplace_back(position.x + size.x);
    m_vertex_positions.emplace_back(position.y);

    m_texture_coords.emplace_back(tex_coords.x);
    m_texture_coords.emplace_back(tex_coords.y);

    m_texture_coords.emplace_back(tex_coords.x);
    m_texture_coords.emplace_back(tex_coords.w);

    m_texture_coords.emplace_back(tex_coords.z);
    m_texture_coords.emplace_back(tex_coords.w);

    m_texture_coords.emplace_back(tex_coords.z);
    m_texture_coords.emplace_back(tex_coords.y);

    ++m_current_size;
}

void
Batch::add(const std::shared_ptr<Texture_2D>& texture, v2f position, v2f size, v4f tex_coords)
{
    if (m_config.texture != nullptr) {
        if (m_config.texture != texture) {
            core::log::warn("Attempting to add texture which does not match active.");
            return;
        }
    }
    else {
        m_config.texture = texture;
    }

    add(position, size, tex_coords);
}

void
Batch::draw()
{
    PROFILE_FUNCTION();

    assert(m_vao != nullptr);
    assert(m_config.shader != nullptr);
    assert(m_config.texture != nullptr);

    m_config.shader->bind();
    m_config.texture->bind();

    m_vao->bind();

    m_vbo->buffer_data(static_cast<u32>(m_vertex_positions.size() * sizeof(f32)), m_vertex_positions.data());
    m_ubo->buffer_data(static_cast<u32>(m_texture_coords.size() * sizeof(f32)), m_texture_coords.data());

    m_ibo->draw_elements(static_cast<i32>(m_current_size * QUAD_INDEX_COUNT), Draw_Mode::Triangles);
    m_vao->unbind();
}

void
Batch::empty()
{
    PROFILE_FUNCTION();

    m_vertex_positions.clear();
    m_texture_coords.clear();
    m_current_size = 0;
    m_config.texture = nullptr;
}

u32
Batch::current_texture_id() const
{
    if (m_config.texture) {
        return m_config.texture->id();
    }

    return 0;
}

bool
Batch::has_space() const
{
    return m_current_size < m_config.max_size;
}

}
