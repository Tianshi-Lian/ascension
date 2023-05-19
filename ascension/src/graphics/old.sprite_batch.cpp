/**
 * File: sprite_batch.cpp
 * Project: ascension
 * File Created: 2023-04-15 14:54:44
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-19 19:28:41
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

#include "graphics/old.sprite_batch.hpp"

#include <limits>

#include <glm/ext/matrix_clip_space.hpp>

#include "yuki/debug/instrumentor.hpp"

#include "core/log.hpp"
#include "graphics/buffer_object.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture_2d.hpp"
#include "graphics/vertex_array_object.hpp"

namespace ascension::graphics {

static constexpr u32 QUAD_VERTEX_COUNT = 4;
static constexpr u32 QUAD_INDEX_COUNT = 6;

Sprite_Batch::Sprite_Batch()
  : m_max_batch_size(0)
  , m_projection{ 1.0f }
  , m_batch_active(false)
  , m_active_transform{ 1.0f }
{
}

Sprite_Batch::~Sprite_Batch()
{
    clear();
}

void
Sprite_Batch::initialize(u32 screen_width, u32 screen_height, const std::shared_ptr<Shader>& sprite_shader, u32 max_batch_size)
{
    if (m_max_batch_size > 0) {
        core::log::error("Sprite_Batch should only be initialized once!");
        return;
    }

    m_sprite_shader = sprite_shader;
    m_max_batch_size = max_batch_size;

    m_sprite_shader->bind();
    m_sprite_shader->set_int("u_texture", 0);

    std::array indices_template{ 0u, 1u, 2u, 2u, 3u, 0u };
    std::vector<u32> indices(static_cast<size_t>(QUAD_INDEX_COUNT) * m_max_batch_size);

    for (u32 i = 0; i < max_batch_size; ++i) {
        const u32 offset = i * QUAD_INDEX_COUNT;
        const u32 vertex_offset = i * QUAD_VERTEX_COUNT;

        indices.at(offset + 0) = (indices_template.at(0) + vertex_offset);
        indices.at(offset + 1) = (indices_template.at(1) + vertex_offset);
        indices.at(offset + 2) = (indices_template.at(2) + vertex_offset);

        indices.at(offset + 3) = (indices_template.at(3) + vertex_offset);
        indices.at(offset + 4) = (indices_template.at(4) + vertex_offset);
        indices.at(offset + 5) = (indices_template.at(5) + vertex_offset); // NOLINT
    }

    static const Vertex_Buffer_Layout buffer_layout = { { Shader_Data_Type::Float, 2, false },
                                                        { Shader_Data_Type::Float, 2, false },
                                                        { Shader_Data_Type::Float, 4, false } };

    m_vertex_array = std::make_unique<Vertex_Array_Object>();
    m_vertex_array->create(true);

    m_vertex_buffer = std::make_shared<Vertex_Buffer_Object>();
    m_vertex_buffer->create(sizeof(Sprite_Batch_Vertex) * m_max_batch_size * QUAD_VERTEX_COUNT);
    m_vertex_buffer->set_layout(buffer_layout);
    m_vertex_array->add_vertex_buffer(m_vertex_buffer);

    m_index_buffer = std::make_shared<Index_Buffer_Object>();
    m_index_buffer->create(sizeof(f32) * static_cast<u32>(indices.size()), indices.data());
    m_vertex_array->set_index_buffer(m_index_buffer);

    m_vertex_array->unbind();

    m_current_batch.reserve(m_max_batch_size);
    m_current_vertices.reserve(static_cast<size_t>(QUAD_VERTEX_COUNT) * max_batch_size);

    on_resize(screen_width, screen_height);
}

void
Sprite_Batch::begin(m4 transform)
{
    PROFILE_FUNCTION();
    if (m_batch_active) {
        core::log::warn("Sprite_Batch::begin() has already been called. Call Sprite_Batch::end() first.");
        return;
    }

    m_batch_active = true;
    m_active_transform = transform;
}

void
Sprite_Batch::end()
{
    PROFILE_FUNCTION();
    if (!m_batch_active) {
        core::log::warn("Sprite_Batch::end() has already been called without an active batch.");
        return;
    }

    std::sort(
        m_current_batch.begin(),
        m_current_batch.end(),
        [](const Sprite_Batch_Item& item_a, const Sprite_Batch_Item& item_b) { return *item_a.texture > *item_b.texture; }
    );

    m_sprite_shader->bind();
    m_sprite_shader->set_mat4f("m_projection_view", m_projection * m_active_transform);

    std::shared_ptr<Texture_2D> current_texture = m_current_batch.front().texture;

    for (const auto& batch_item : m_current_batch) {
        if (batch_item.texture != current_texture) {
            flush(current_texture);
            current_texture = batch_item.texture;
        }

        generate_quad_vertices(batch_item);

        if ((m_current_vertices.size() / QUAD_VERTEX_COUNT) >= m_max_batch_size) {
            flush(current_texture);
        }
    }

    flush(current_texture);
    clear();
    m_batch_active = false;
}

void
Sprite_Batch::clear()
{
    PROFILE_FUNCTION();
    m_current_batch.clear();
    m_current_vertices.clear();
}

void
Sprite_Batch::on_resize(u32 screen_width, u32 screen_height)
{
#ifdef AS_DEBUG
    static const auto float_max = static_cast<u32>(std::numeric_limits<f32>::infinity());
    assert(float_max >= screen_width);
    assert(float_max >= screen_height);
#endif
    m_projection = glm::ortho(0.0f, static_cast<f32>(screen_width), 0.0f, static_cast<f32>(screen_height), -1.0f, 1.0f);
}

void
Sprite_Batch::draw(const std::shared_ptr<Texture_2D>& texture, v2f position)
{
    PROFILE_FUNCTION();
    if (!m_batch_active) {
        core::log::warn("Sprite_Batch::draw() has already been called without an active batch.");
        return;
    }

    m_current_batch.emplace_back(texture, position);
}

void
Sprite_Batch::generate_quad_vertices(const Sprite_Batch_Item& item)
{
    PROFILE_FUNCTION();
    const auto texture = item.texture;
    const auto position = item.position;
    const auto width = static_cast<f32>(texture->width());
    const auto height = static_cast<f32>(texture->height());
    static const auto sprite_colour = v4f{ 1.0f, 1.0f, 1.0f, 1.0f };

    m_current_vertices.emplace_back(v2{ position.x, position.y }, v2{ 0, 0 }, sprite_colour);
    m_current_vertices.emplace_back(v2{ position.x, position.y + height }, v2{ 0, 1 }, sprite_colour);
    m_current_vertices.emplace_back(v2{ position.x + height, position.y + height }, v2{ 1, 1 }, sprite_colour);
    m_current_vertices.emplace_back(v2{ position.x + width, position.y }, v2{ 1, 0 }, sprite_colour);
}

void
Sprite_Batch::flush(const std::shared_ptr<Texture_2D>& texture)
{
    PROFILE_FUNCTION();
    if (!texture) {
        return;
    }

    m_vertex_array->bind();
    texture->bind();
    m_vertex_buffer->buffer_data(
        static_cast<u32>(sizeof(Sprite_Batch_Vertex) * m_current_vertices.size()), m_current_vertices.data()
    );
    m_index_buffer->draw_elements(
        static_cast<i32>((m_current_vertices.size() / QUAD_VERTEX_COUNT) * QUAD_INDEX_COUNT), Draw_Mode::Triangles
    );

    m_vertex_array->unbind();
    m_current_vertices.clear();
}

}
