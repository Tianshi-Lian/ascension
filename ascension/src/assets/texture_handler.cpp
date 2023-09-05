/**
 * File: texture_handler.cpp
 * Project: ascension
 * File Created: 2023-08-15 20:12:47
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-09-04 21:17:43
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

#include "assets/texture_handler.hpp"

#define STBI_NO_THREAD_LOCALS
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "assets/asset_types.hpp"
#include "assets/serialize_assets.hpp"
#include "core/log.hpp"
#include "graphics/texture_2d.hpp"

namespace ascension::assets {

void
Texture_Handler::register_texture_file(const std::string& asset_id, const YAML::Node& file_node)
{
    const auto texture_file = file_node.as<Texture_File>();
    m_texture_files[asset_id] = texture_file;
}

std::shared_ptr<graphics::Texture_2D>
Texture_Handler::load_texture(const std::string& asset_name)
{
    auto texture = get_texture(asset_name);
    if (texture) {
        return texture;
    }

    if (m_texture_files.count(asset_name) == 0u) {
        core::log::warn("Attempting to load unrecognized texture {}", asset_name);
        return nullptr;
    }

    Texture_File asset_file = m_texture_files[asset_name];

    if (asset_file.flip_on_load) {
        stbi_set_flip_vertically_on_load(1);
    }

    i32 width = 0;
    i32 height = 0;
    i32 channels = 0;
    auto* const data = stbi_load(asset_file.filepath.c_str(), &width, &height, &channels, 0);

    auto new_texture = std::make_shared<graphics::Texture_2D>();
    new_texture->create(static_cast<u32>(width), static_cast<u32>(height), data);

    stbi_image_free(data);

    if (asset_file.scale != 1.0f) {
        // TODO: Implement texture scaling.
    }

    if (asset_file.flip_on_load) {
        stbi_set_flip_vertically_on_load(0);
    }

    m_loaded_textures.insert({ asset_name, new_texture });
    return new_texture;
}

std::shared_ptr<graphics::Texture_2D>
Texture_Handler::get_texture(const std::string& asset_name)
{
    if (m_loaded_textures.count(asset_name) == 0) {
        return nullptr;
    }

    return m_loaded_textures[asset_name];
}

void
Texture_Handler::unload_texture(const std::string& asset_name)
{
    const auto texture = get_texture(asset_name);
    if (!texture) {
        return;
    }

    m_loaded_textures.erase(asset_name);
}

void
Texture_Handler::register_texture_atlas_file(const std::string& asset_id, const YAML::Node& file_node)
{
    const auto texture_atlas_file = file_node.as<Texture_Atlas_File>();
    m_texture_atlas_files[asset_id] = texture_atlas_file;
}

}
