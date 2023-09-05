/**
 * File: texture_handler.hpp
 * Project: ascension
 * File Created: 2023-08-15 20:09:20
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-09-04 21:17:13
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

#include <yaml-cpp/node/node.h>

#include "assets/asset_types.hpp"

namespace ascension::graphics {
class Texture_2D;
class Texture_Atlas;
}

namespace ascension::assets {

class Texture_Handler {
public:
    void register_texture_file(const std::string& asset_id, const YAML::Node& file_node);
    void register_texture_atlas_file(const std::string& asset_id, const YAML::Node& file_node);

    std::shared_ptr<graphics::Texture_2D> load_texture(const std::string& asset_name);
    std::shared_ptr<graphics::Texture_2D> get_texture(const std::string& asset_name);
    void unload_texture(const std::string& asset_name);

    std::shared_ptr<graphics::Texture_Atlas> load_texture_atlas(const std::string& asset_name);
    std::shared_ptr<graphics::Texture_Atlas> get_texture_atlas(const std::string& asset_name);
    void unload_texture_atlas(const std::string& asset_name);

private:
    std::unordered_map<std::string, Texture_File> m_texture_files;
    std::unordered_map<std::string, std::shared_ptr<graphics::Texture_2D>> m_loaded_textures;

    std::unordered_map<std::string, Texture_Atlas_File> m_texture_atlas_files;
    std::unordered_map<std::string, std::shared_ptr<graphics::Texture_Atlas>> m_loaded_texture_atlas;
};

}

#endif // ASCENSION_ASSETS_TEXTURE_HANDLER_HPP
