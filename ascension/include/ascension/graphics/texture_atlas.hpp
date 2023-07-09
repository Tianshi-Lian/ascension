/**
 * File: texture_atlas.hpp
 * Project: ascension
 * File Created: 2023-07-05 18:49:32
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-09 17:32:26
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

#ifndef ASCENSION_GRAPHICS_TEXTURE_ATLAS_HPP
#define ASCENSION_GRAPHICS_TEXTURE_ATLAS_HPP

#include "graphics/texture_2d.hpp"

#include <unordered_map>

namespace ascension::graphics {

class Texture_Atlas {
public:
    Texture_Atlas() = default;

    void create(const std::shared_ptr<Texture_2D>& texture, const std::unordered_map<std::string, v4u>& sub_textures);

    const Texture_2D& get_sub_texture(const std::string& name) const;
    const Texture_2D& get_sub_texture(u32 coords_id) const;

    const std::shared_ptr<Texture_2D>& get_texture() const;

private:
    std::shared_ptr<Texture_2D> m_texture;

    std::unordered_map<std::string, u32> m_coord_ids;
    std::vector<Texture_2D> m_sub_textures;
};

}

#endif // ASCENSION_GRAPHICS_TEXTURE_ATLAS_HPP
