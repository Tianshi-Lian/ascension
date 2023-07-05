/**
 * File: texture_atlas.hpp
 * Project: ascension
 * File Created: 2023-07-05 18:49:32
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-05 20:31:55
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

// TODO: Create a sub_texture struct with v2i: size, v4f: coords or alternatively just add
// /t    a texture_coords field to the Texture_2D which defaults to 0, 0, 1, 1 unless used as
// /t    a sub_texture and then store a list of textures here - preferable?

class Texture_Atlas {
public:
    Texture_Atlas() = default;

    void create(const std::shared_ptr<Texture_2D>& texture, const std::unordered_map<std::string, v4f>& sub_textures);

    const v4f& get_texture_coords(const std::string& name) const;
    const v4f& get_texture_coords(u32 coords_id) const;

    const std::shared_ptr<Texture_2D>& get_texture() const;

private:
    std::shared_ptr<Texture_2D> m_texture;

    std::unordered_map<std::string, u32> m_coord_ids;
    std::vector<v4f> m_texture_coords;
};

}

#endif // ASCENSION_GRAPHICS_TEXTURE_ATLAS_HPP
