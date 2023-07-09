/**
 * File: sprite_batch.hpp
 * Project: ascension
 * File Created: 2023-04-15 14:54:37
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-09 18:43:14
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
    // TODO: Consider removing this and just having things which need static drawing keep their own filled out Batch
    // /t objects which are added to the sprite batch every frame - would require "removing" empty batches to avoid max_size
    bool is_static{};
};

class Batch {
public:
    Batch();
    explicit Batch(const Batch_Config& config);

    void create(const Batch_Config& config);

    void set_texture(const std::shared_ptr<Texture_2D>& texture);
    void set_is_static(bool is_static);

    void add(const v2f& position, const v2u& size, const v4f& tex_coords);
    void add(const Texture_2D& sub_texture, const v2f& position);
    void add(const std::shared_ptr<Texture_2D>& texture, const v2f& position);

    void flush();
    void clear();

    [[nodiscard]] u32 current_texture_id() const;
    [[nodiscard]] bool has_space() const;
    [[nodiscard]] bool is_empty() const;
    [[nodiscard]] bool is_static() const;

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
    Sprite_Batch();
    Sprite_Batch(u32 max_batches, u32 batch_size, const std::shared_ptr<Shader>& default_shader);

    void create(u32 max_batches, u32 batch_size, const std::shared_ptr<Shader>& default_shader);

    void add_batch(const std::shared_ptr<Batch>& batch);
    void create_batch(const Batch_Config& config);

    void draw(const std::shared_ptr<Texture_2D>& texture, const v2f& position, bool is_static = false);
    void draw(
        const std::shared_ptr<Texture_2D>& texture,
        const Texture_2D& sub_texture,
        const v2f& position,
        bool is_static = false
    );

    void flush();

private:
    std::vector<std::shared_ptr<Batch>> m_batches;
    u32 m_max_batches;

    u32 m_batch_size;
    std::shared_ptr<Shader> m_default_shader;

    void draw(
        const std::shared_ptr<Texture_2D>& texture,
        const v2f& position,
        const v2u& size,
        const v4f& texture_coords,
        bool is_static
    );
};

}
