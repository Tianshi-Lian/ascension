/**
 * File: texture_atlas.cpp
 * Project: ascension
 * File Created: 2023-07-05 18:55:49
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-09 17:34:29
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

#include "graphics/texture_atlas.hpp"

#include "core/log.hpp"

namespace ascension::graphics {

void
Texture_Atlas::create(const std::shared_ptr<Texture_2D>& texture, const std::unordered_map<std::string, v4u>& sub_textures)
{
    if (texture == nullptr) {
        core::log::error("Texture_Atlas::create() attempting to create a texture_atlas from a null texture");
        return;
    }

    m_sub_textures.reserve(sub_textures.size());
    m_coord_ids.reserve(sub_textures.size());

    Texture_2D null_texture;
    null_texture.create(0, 0, nullptr);

    m_coord_ids[""] = 0;
    m_sub_textures.push_back(null_texture);

    for (const auto& sub_texture_data : sub_textures) {
        m_coord_ids[sub_texture_data.first] = static_cast<u32>(m_sub_textures.size());

        const v4u& image_dims = sub_texture_data.second;
        const u32 sub_texture_width = image_dims.z - image_dims.x;
        const u32 sub_texture_height = image_dims.w - image_dims.y;

        const v4f sub_texture_coords = {
            static_cast<f32>(image_dims.x) / static_cast<f32>(texture->width()),
            static_cast<f32>(image_dims.y) / static_cast<f32>(texture->height()),
            static_cast<f32>(image_dims.z) / static_cast<f32>(texture->width()),
            static_cast<f32>(image_dims.w) / static_cast<f32>(texture->height()),
        };

        Texture_2D sub_texture;
        sub_texture.create(sub_texture_width, sub_texture_height, sub_texture_coords, nullptr);

        m_sub_textures.push_back(sub_texture);
    }

    m_texture = texture;
}

const Texture_2D&
Texture_Atlas::get_sub_texture(const std::string& name) const
{
    if (m_coord_ids.count(name) == 0) {
        core::log::warn("Texture_Atlas::get_texture_coords() Attempting to get non-existant sub_texture: '{}'", name);
        return m_sub_textures.at(0);
    }

    return m_sub_textures.at(m_coord_ids.at(name));
}

const Texture_2D&
Texture_Atlas::get_sub_texture(u32 coords_id) const
{
    if (coords_id >= m_sub_textures.size()) {
        core::log::warn("Texture_Atlas::get_texture_coords() Attempting to get non-existant sub_texture: {}", coords_id);
        return m_sub_textures.at(0);
    }

    return m_sub_textures.at(coords_id);
}

const std::shared_ptr<Texture_2D>&
Texture_Atlas::get_texture() const
{
    return m_texture;
}

}
