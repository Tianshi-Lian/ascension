/**
 * File: asset_manager.cpp
 * Project: ascension
 * File Created: 2023-04-13 15:04:17
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-14 20:38:01
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

#include "assets/asset_manager.hpp"

#define STBI_NO_THREAD_LOCALS
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <magic_enum/magic_enum.hpp>
#include <pugixml.hpp>

#include "yuki/debug/logger.hpp"

#include "graphics/shader.hpp"
#include "graphics/texture_2d.hpp"

namespace ascension::assets {

Asset_Manager::~Asset_Manager()
{
    clear();
}

void
Asset_Manager::clear()
{
    m_texture_filepaths.clear();
    m_shader_filepaths.clear();

    m_loaded_textures.clear();
    m_loaded_shaders.clear();
}

void
// NOLINTNEXTLINE
Asset_Manager::parse_asset_document(const std::string& document_filepath, const std::string& root_name)
{
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(document_filepath.c_str());

    if (!result) {
        yuki::debug::Logger::error(
            "ascension", "Failed to load asset file {}. Error {}", document_filepath, result.description()
        );
        return;
    }

    for (const auto& node : document.child("assets").children("asset")) {
        const std::string name = node.attribute("name").value();
        const std::string type_str = node.attribute("type").value();
        const std::string filepath = node.attribute("filepath").value();

        const auto type = magic_enum::enum_cast<Asset_Type>(type_str);
        if (!type.has_value()) {
            yuki::debug::Logger::error("ascension", "Unknown asset type {}", type_str);
            return;
        }

        std::string asset_base_path;
        if (!root_name.empty()) {
            asset_base_path = root_name + "/";
        }
        else if (type != ascension::assets::Asset_Type::Asset_List) {
            asset_base_path = type_str + "/";
        }

        if (type == Asset_Type::Asset_List) {
            parse_asset_document(filepath, asset_base_path + name);
        }
        else {
            switch (*type) {
                case Asset_Type::Texture: {
                    Texture_Asset asset;
                    if (!node.child("scale").empty()) {
                        asset.scale = std::strtof(node.child("scale").child_value(), nullptr);
                    }
                    if (!node.child("flip").empty()) {
                        asset.flip_on_load = (std::stoi(node.child("flip").child_value()) != 0);
                    }
                    asset.name = name;
                    asset.filepath = filepath;
                    asset.type = Asset_Type::Texture;
                    m_texture_filepaths[(asset_base_path + name)] = asset;
                } break;
                case Asset_Type::Shader: {
                    Shader_Asset asset;
                    if (node.child("vertex").empty() || node.child("fragment").empty()) {
                        yuki::debug::Logger::error(
                            "ascension", "Trying to load shader {} ({}) without fragment or vertex source.", name, filepath
                        );
                    }
                    asset.vertex_src_file = node.child("vertex").child_value();
                    asset.fragment_src_file = node.child("fragment").child_value();
                    asset.name = name;
                    asset.filepath = filepath;
                    asset.type = Asset_Type::Shader;
                    m_shader_filepaths[(asset_base_path + name)] = asset;
                } break;
                default:
                    break;
            };
        }
    }
}

void
Asset_Manager::load_asset_file(const std::string& asset_file)
{
    parse_asset_document(asset_file, "");

    yuki::debug::Logger::info("ascension", "Loaded {} textures", m_texture_filepaths.size());
    yuki::debug::Logger::info("ascension", "Loaded {} shaders", m_shader_filepaths.size());
}

std::shared_ptr<graphics::Texture_2D>
Asset_Manager::load_texture_2d(const std::string& asset_name)
{
    auto texture = get_texture_2d(asset_name);
    if (texture) {
        return texture;
    }

    if (m_texture_filepaths.count(asset_name) == 0u) {
        yuki::debug::Logger::warn("ascension", "Attempting to load unrecognized texture {}", asset_name);
        return nullptr;
    }

    Texture_Asset asset = m_texture_filepaths[asset_name];

    if (asset.flip_on_load) {
        stbi_set_flip_vertically_on_load(1);
    }

    i32 width = 0;
    i32 height = 0;
    i32 channels = 0;
    auto* const data = stbi_load(asset.filepath.c_str(), &width, &height, &channels, 0);

    auto new_texture = std::make_shared<graphics::Texture_2D>();
    new_texture->create(static_cast<u32>(width), static_cast<u32>(height), data);

    stbi_image_free(data);

    if (asset.scale != 1.0f) {
        // TODO: Implement texture scaling.
    }

    if (asset.flip_on_load) {
        stbi_set_flip_vertically_on_load(0);
    }

    m_loaded_textures.insert({ asset_name, new_texture });
    return new_texture;
}

std::shared_ptr<graphics::Texture_2D>
Asset_Manager::get_texture_2d(const std::string& asset_name)
{
    if (m_loaded_textures.count(asset_name) == 0) {
        return nullptr;
    }

    return m_loaded_textures[asset_name];
}

void
Asset_Manager::unload_texture_2d(const std::string& asset_name)
{
    const auto texture = get_texture_2d(asset_name);
    if (!texture) {
        return;
    }

    m_loaded_textures.erase(asset_name);
}

std::shared_ptr<graphics::Shader>
Asset_Manager::load_shader(const std::string& asset_name)
{
    auto shader = get_shader(asset_name);
    if (shader) {
        return shader;
    }

    if (m_shader_filepaths.count(asset_name) == 0u) {
        yuki::debug::Logger::warn("ascension", "Attempting to load unrecognized shader {}", asset_name);
        return nullptr;
    }

    Shader_Asset asset = m_shader_filepaths[asset_name];

    const auto vertex_filepath = asset.filepath + asset.vertex_src_file;
    const auto fragment_filepath = asset.filepath + asset.fragment_src_file;

    std::ifstream vertex_file_stream(vertex_filepath);
    std::ifstream fragment_file_stream(fragment_filepath);

    std::stringstream vertex_string_stream;
    std::stringstream fragment_string_stream;

    vertex_string_stream << vertex_file_stream.rdbuf();
    fragment_string_stream << fragment_file_stream.rdbuf();

    vertex_file_stream.close();
    fragment_file_stream.close();

    auto new_shader = std::make_shared<graphics::Shader>();
    new_shader->create(vertex_string_stream.str(), fragment_string_stream.str());

    m_loaded_shaders.insert({ asset_name, new_shader });
    return new_shader;
}

std::shared_ptr<graphics::Shader>
Asset_Manager::get_shader(const std::string& asset_name)
{
    if (m_loaded_shaders.count(asset_name) == 0) {
        return nullptr;
    }

    return m_loaded_shaders[asset_name];
}

void
Asset_Manager::unload_shader(const std::string& asset_name)
{
    const auto shader = get_shader(asset_name);
    if (!shader) {
        return;
    }

    m_loaded_shaders.erase(asset_name);
}

}
