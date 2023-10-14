/**
 * File: sprite_batch.cpp
 * Project: ascension
 * File Created: 2023-04-15 14:54:44
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-10-14 16:00:13
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

#include <GL/glew.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "debug/instrumentor.hpp"

#include "core/log.hpp"
#include "graphics/buffer_object.hpp"
#include "graphics/shader.hpp"
#include "graphics/sprite_font.hpp"
#include "graphics/texture_2d.hpp"
#include "graphics/vertex_array_object.hpp"

namespace ascension::graphics {

static constexpr u32 QUAD_VERTEX_COUNT = 4;
static constexpr u32 QUAD_VERTEX_COMPONENT_COUNT = QUAD_VERTEX_COUNT * 2;
static constexpr u32 QUAD_INDEX_COUNT = 6;
static constexpr u32 PIXEL_BIT_SHIFT = 6;

// Batch
Batch::Batch()
  : m_current_size(0)
{
}

Batch::Batch(const Batch_Config& config)
  : m_current_size(0)
{
    create(config);
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
Batch::set_texture(const std::shared_ptr<Texture_2D>& texture)
{
    if (m_config.texture != nullptr) {
        flush();
        clear();
    }

    m_config.texture = texture;
}

void
Batch::set_is_static(bool is_static)
{
    m_config.is_static = is_static;
}

void
Batch::add(const v2f& position, const v2u& size, const v4f& tex_coords)
{
    PROFILE_FUNCTION();

    if (m_config.max_size == 0 || m_config.texture == nullptr) {
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
    m_vertex_positions.emplace_back(position.y + static_cast<f32>(size.y));

    m_vertex_positions.emplace_back(position.x + static_cast<f32>(size.x));
    m_vertex_positions.emplace_back(position.y + static_cast<f32>(size.y));

    m_vertex_positions.emplace_back(position.x + static_cast<f32>(size.x));
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
Batch::add(const std::shared_ptr<Texture_2D>& texture, const v2f& position)
{
    add(position, texture->size(), texture->texture_coords());
}

void
Batch::add(const Texture_2D& sub_texture, const v2f& position)
{
    add(position, sub_texture.size(), sub_texture.texture_coords());
}

void
Batch::flush()
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

    if (!m_config.is_static) {
        clear();
    }
}

void
Batch::clear()
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

bool
Batch::is_empty() const
{
    return m_current_size == 0;
}

bool
Batch::is_static() const
{
    return m_config.is_static;
}

// Sprite_Batch
Sprite_Batch::Sprite_Batch()
  : m_max_batches(0)
  , m_batch_size(0)
  , m_default_shader(nullptr)
{
}

Sprite_Batch::Sprite_Batch(u32 max_batches, u32 batch_size, const std::shared_ptr<Shader>& default_shader)
  : m_max_batches(0)
  , m_batch_size(0)
{
    create(max_batches, batch_size, default_shader);
}

void
Sprite_Batch::create(u32 max_batches, u32 batch_size, const std::shared_ptr<Shader>& default_shader)
{
    m_max_batches = max_batches;
    m_batch_size = batch_size;
    m_default_shader = default_shader;

    m_batches.reserve(max_batches);
}

void
Sprite_Batch::add_batch(const std::shared_ptr<Batch>& batch)
{
    // TODO: Check if we have an empty batch and replace that?
    if (m_batches.size() >= m_max_batches) {
        core::log::error("Sprite_Batch::add_batch() attempting to add batch to full Sprite_Batch");
        return;
    }

    m_batches.push_back(batch);
}

void
Sprite_Batch::create_batch(const Batch_Config& config)
{
    if (m_batches.size() >= m_max_batches) {
        core::log::error("Sprite_Batch::create_batch() attempting to create batch for full Sprite_Batch");
        return;
    }

    m_batches.emplace_back(std::make_shared<Batch>(config));
}

void
Sprite_Batch::flush()
{
    // TODO: Sort the batches by a priority index for drawing order.
    for (auto& batch : m_batches) {
        if (!batch->is_empty()) {
            batch->flush();
        }
    }
}

void
Sprite_Batch::draw_texture(const std::shared_ptr<Texture_2D>& texture, const v2f& position, bool is_static)
{
    draw_texture_internal(texture, position, texture->size(), texture->texture_coords(), is_static);
}

void
Sprite_Batch::draw_texture(
    const std::shared_ptr<Texture_2D>& texture,
    const Texture_2D& sub_texture,
    const v2f& position,
    bool is_static
)
{
    draw_texture_internal(texture, position, sub_texture.size(), sub_texture.texture_coords(), is_static);
}

void
Sprite_Batch::draw_string(
    const std::shared_ptr<Sprite_Font>& font,
    u16 font_size,
    const v2f& position,
    const std::string& value,
    bool is_static
)
{
    auto current_position = position;
    for (const auto& character : value) {
        const auto& glyph = font->get_glyph(static_cast<u32>(character), font_size);

        const auto offset_x = glyph.bearing.x;
        const auto offset_y = (static_cast<f32>(glyph.sub_texture.height()) - glyph.bearing.y);
        const auto glyph_position = v2f{ current_position.x + offset_x, current_position.y - offset_y };

        draw_texture_internal(
            glyph.texture, glyph_position, glyph.sub_texture.size(), glyph.sub_texture.texture_coords(), is_static
        );

        current_position.x += static_cast<f32>((glyph.advance >> PIXEL_BIT_SHIFT));
    }
}

void
Sprite_Batch::draw_texture_internal(
    const std::shared_ptr<Texture_2D>& texture,
    const v2f& position,
    const v2u& size,
    const v4f& texture_coords,
    bool is_static
)
{
    for (auto& batch : m_batches) {
        if (!batch->has_space()) {
            continue;
        }

        if (batch->current_texture_id() == 0) {
            batch->set_texture(texture);
            batch->set_is_static(is_static);

            batch->add(position, size, texture_coords);
            return;
        }
        else if (batch->current_texture_id() == texture->id() && batch->is_static() == is_static) {
            batch->add(position, size, texture_coords);
            return;
        }
    }

    // We didn't find a batch to put it in, check if we can make a new batch,
    // else we've got no space!
    if (m_batches.size() < m_max_batches) {
        Batch_Config config(m_batch_size, texture, m_default_shader, is_static);
        create_batch(config);

        m_batches.back()->add(position, size, texture_coords);
    }
    else {
        // TODO: Consider if we should try and empty the fullest batch?
        core::log::error("Sprite_Batch::draw() trying to draw new texture when all batches are full!");
    }
}

}
