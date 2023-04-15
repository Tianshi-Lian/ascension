/**
 * File: sprite_batch.hpp
 * Project: ascension
 * File Created: 2023-04-15 14:54:37
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-15 15:29:51
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

class Texture_2D;
class Shader;
class Vertex_Array_Object;
class Vertex_Buffer_Object;
class Index_Buffer_Object;

class Sprite_Batch {
    static constexpr u32 DEFAULT_BATCH_SIZE = 2048;

  public:
    Sprite_Batch();
    ~Sprite_Batch();

    void initialize(u32 screen_width, u32 screen_height, u32 max_batch_size = DEFAULT_BATCH_SIZE);

    void begin(m4 transform);
    void end();
    void clear();

    void draw(const std::shared_ptr<Texture_2D>& texture, v2f position);

    Sprite_Batch(const Sprite_Batch&) = delete;
    Sprite_Batch(Sprite_Batch&&) = delete;
    Sprite_Batch& operator=(const Sprite_Batch&) = delete;
    Sprite_Batch& operator=(Sprite_Batch&&) = delete;

  private:
    void flush(const std::shared_ptr<Texture_2D>& texture);

    u32 m_screen_width;
    u32 m_screen_height;

    u32 m_max_batch_size;

    std::shared_ptr<Shader> m_sprite_shader;

    std::unique_ptr<Vertex_Array_Object> m_vertex_array;
    std::shared_ptr<Vertex_Buffer_Object> m_vertex_buffer;
    std::shared_ptr<Index_Buffer_Object> m_index_buffer;
};

}
