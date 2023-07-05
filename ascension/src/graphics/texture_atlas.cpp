/**
 * File: texture_atlas.cpp
 * Project: ascension
 * File Created: 2023-07-05 18:55:49
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-05 20:30:20
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
Texture_Atlas::create(const std::shared_ptr<Texture_2D>& texture, const std::unordered_map<std::string, v4f>& sub_textures)
{
    if (texture == nullptr) {
        core::log::error("Texture_Atlas::create() attempting to create a texture_atlas from a null texture");
        return;
    }

    m_texture_coords.reserve(sub_textures.size());
    m_coord_ids.reserve(sub_textures.size());

    m_coord_ids[""] = 0;
    m_texture_coords.emplace_back(0, 0, 0, 0);

    for (const auto& sub_texture : sub_textures) {
        m_coord_ids[sub_texture.first] = static_cast<u32>(m_texture_coords.size());
        m_texture_coords.push_back(sub_texture.second);
    }

    m_texture = texture;
}

const v4f&
Texture_Atlas::get_texture_coords(const std::string& name) const
{
    if (m_coord_ids.count(name) == 0) {
        core::log::warn("Texture_Atlas::get_texture_coords() Attempting to get non-existant sub_texture: '{}'", name);
        return m_texture_coords.at(0);
    }

    return m_texture_coords.at(m_coord_ids.at(name));
}

const v4f&
Texture_Atlas::get_texture_coords(u32 coords_id) const
{
    if (coords_id >= m_texture_coords.size()) {
        core::log::warn("Texture_Atlas::get_texture_coords() Attempting to get non-existant sub_texture: {}", coords_id);
        return m_texture_coords.at(0);
    }

    return m_texture_coords.at(coords_id);
}

const std::shared_ptr<Texture_2D>&
Texture_Atlas::get_texture() const
{
    return m_texture;
}

}
