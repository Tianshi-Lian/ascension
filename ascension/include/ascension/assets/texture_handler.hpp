/**
 * File: texture_handler.hpp
 * Project: ascension
 * File Created: 2023-08-15 20:09:20
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-15 20:45:43
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

#ifndef ASCENSION_ASSETS_TEXTURE_HANDLER_HPP
#define ASCENSION_ASSETS_TEXTURE_HANDLER_HPP

#include <unordered_map>

#include "assets/asset_types.hpp"

namespace ascension::graphics {
class Texture_2D;
}

namespace ascension::assets {

struct Texture_File : public Asset_File {
    f32 scale{ 1.0 };
    bool flip_on_load{ true };
};

class Texture_Handler {
public:
    // TODO: When we swap to yaml we should just pass the asset.yaml path here and parse the file attributes here.
    // Texture_File parse_texture_file()

    std::shared_ptr<graphics::Texture_2D> load_texture(const std::string& asset_name);
    std::shared_ptr<graphics::Texture_2D> get_texture(const std::string& asset_name);
    void unload_texture(const std::string& asset_name);

private:
    std::unordered_map<std::string, Texture_File> m_texture_files;
    std::unordered_map<std::string, std::shared_ptr<graphics::Texture_2D>> m_loaded_textures;
};

}

#endif // ASCENSION_ASSETS_TEXTURE_HANDLER_HPP
