/**
 * File: asset_manager.hpp
 * Project: ascension
 * File Created: 2023-04-13 14:45:21
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-22 16:05:51
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

#include <unordered_map>

#include "assets/asset_types.hpp"

namespace ascension::graphics {
class Shader;
class Sprite_Font;
class Texture_2D;
class Texture_Atlas;
}

namespace ascension::assets {

// TODO: Look at registering loaders for asset types through templates so we can move internal implementation
// /t    details of asset loading away from the asset manager to specific loader classes.
// /t    register_loader<Font_Asset, Sprite_Font>(std::shared_ptr<AssetLoader<>());
// /t    We can also have the loaders look after usage counts for deleting assets for when we want to share
// /t    assets between assets, like texture & texture_atlas or fonts & shaders
class Asset_Manager {
public:
    Asset_Manager() = default;
    ~Asset_Manager();

    void clear();

    void load_asset_file(const std::string& asset_file);

    std::shared_ptr<graphics::Texture_2D> load_texture_2d(const std::string& asset_name);
    std::shared_ptr<graphics::Texture_2D> get_texture_2d(const std::string& asset_name);
    void unload_texture_2d(const std::string& asset_name);

    std::shared_ptr<graphics::Texture_Atlas> load_texture_atlas(const std::string& asset_name);
    std::shared_ptr<graphics::Texture_Atlas> get_texture_atlas(const std::string& asset_name);
    void unload_texture_atlas(const std::string& asset_name);

    std::shared_ptr<graphics::Shader> load_shader(const std::string& asset_name);
    std::shared_ptr<graphics::Shader> get_shader(const std::string& asset_name);
    void unload_shader(const std::string& asset_name);

    std::shared_ptr<graphics::Sprite_Font> load_font(const std::string& asset_name);
    std::shared_ptr<graphics::Sprite_Font> get_font(const std::string& asset_name);
    void unload_font(const std::string& asset_name);

    Asset_Manager(const Asset_Manager&) = default;
    Asset_Manager(Asset_Manager&&) = delete;
    Asset_Manager& operator=(const Asset_Manager&) = default;
    Asset_Manager& operator=(Asset_Manager&&) = delete;

private:
    void parse_asset_document(const std::string& document_filepath, const std::string& root_name);

    std::unordered_map<std::string, Font_Asset> m_font_filepaths;
    std::unordered_map<std::string, Shader_Asset> m_shader_filepaths;
    std::unordered_map<std::string, Texture_Asset> m_texture_filepaths;
    std::unordered_map<std::string, Texture_Atlas_Asset> m_texture_atlas_filepaths;

    std::unordered_map<std::string, std::shared_ptr<graphics::Shader>> m_loaded_shaders;
    std::unordered_map<std::string, std::shared_ptr<graphics::Sprite_Font>> m_loaded_fonts;
    std::unordered_map<std::string, std::shared_ptr<graphics::Texture_2D>> m_loaded_textures;
    std::unordered_map<std::string, std::shared_ptr<graphics::Texture_Atlas>> m_loaded_texture_atlas;
};

}
