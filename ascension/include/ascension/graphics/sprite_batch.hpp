/**
 * File: sprite_batch.hpp
 * Project: ascension
 * File Created: 2023-04-15 14:54:37
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-19 20:19:30
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

#include "graphics/shader.hpp"
#include "graphics/texture_2d.hpp"
#include "graphics/vertex_array_object.hpp"

namespace ascension::graphics {

class Vertex_Buffer_Object;
class Index_Buffer_Object;

struct Batch_Config {
    Batch_Config()
      : texture(nullptr)
      , shader(nullptr)

    {
    }

    Batch_Config(
        u32 _size,
        const std::shared_ptr<Texture_2D>& _texture,
        const std::shared_ptr<Shader>& _shader,
        bool _is_static = false
    )
      : max_size(_size)
      , texture(_texture)
      , shader(_shader)
      , is_static(_is_static)
    {
    }

    u32 max_size{};
    std::shared_ptr<Texture_2D> texture;
    std::shared_ptr<Shader> shader;
    bool is_static{};
};

class Batch {
  public:
    Batch();

    void create(const Batch_Config& config);

    // TODO: Implement some recognition of texture atlases or sub-textures and remove manually specifying tex_coords.
    void add(v2f position, v2f size, v4f tex_coords = { 0, 0, 1, 1 });
    void add(const std::shared_ptr<Texture_2D>& texture, v2f position, v2f size, v4f tex_coords = { 0, 0, 1, 1 });

    void draw();
    void empty();

    [[nodiscard]] u32 current_texture_id() const;
    [[nodiscard]] bool has_space() const;

  private:
    Batch_Config m_config;
    u32 m_current_size;

    std::unique_ptr<Vertex_Array_Object> m_vao;
    std::shared_ptr<Vertex_Buffer_Object> m_vbo;
    std::shared_ptr<Vertex_Buffer_Object> m_ubo;
    std::shared_ptr<Index_Buffer_Object> m_ibo;

    std::vector<f32> m_vertex_positions;
    std::vector<f32> m_texture_coords;
};

class Sprite_Batch {
  public:
    Sprite_Batch(u32 max_batches, u32 batch_size, std::shared_ptr<Shader> shader);

    void draw(const std::shared_ptr<Texture_2D>& texture, v2f position, v4f tex_coords = { 0, 0, 1, 1 });

    void draw_all();

  private:
    std::vector<Batch> m_batches;
};

}
