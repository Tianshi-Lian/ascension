/**
 * File: texture_2d.hpp
 * Project: erika
 * File Created: 2023-04-24 13:50:51
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-24 14:12:44
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

class Texture_2D {
public:
    Texture_2D();
    ~Texture_2D();

    // TODO: Add and check create result.
    void create(u32 width, u32 height, u8* data);
    void bind() const;
    static void unbind();

    [[nodiscard]] u32 id() const;
    [[nodiscard]] u32 width() const;
    [[nodiscard]] u32 height() const;

    friend bool operator==(const Texture_2D& tex_1, const Texture_2D& tex_2);
    friend bool operator!=(const Texture_2D& tex_1, const Texture_2D& tex_2);

    friend bool operator<(const Texture_2D& tex_1, const Texture_2D& tex_2);
    friend bool operator>(const Texture_2D& tex_1, const Texture_2D& tex_2);

    Texture_2D(const Texture_2D&) = default;
    Texture_2D(Texture_2D&&) = delete;
    Texture_2D& operator=(const Texture_2D&) = default;
    Texture_2D& operator=(Texture_2D&&) = delete;

private:
    u32 m_id;
    u32 m_width;
    u32 m_height;
};

}
