/**
 * File: asset_manager.cpp
 * Project: ascension
 * File Created: 2023-04-13 15:04:17
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-22 16:27:47
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

#include "core/log.hpp"
#include "graphics/shader.hpp"
#include "graphics/sprite_font.hpp"
#include "graphics/texture_2d.hpp"
#include "graphics/texture_atlas.hpp"

namespace {

ascension::assets::Texture_Asset
parse_texture_asset(const pugi::xml_node& node, const std::string& name, const std::string& filepath)
{
    ascension::assets::Texture_Asset asset;
    if (!node.child("scale").empty()) {
        asset.scale = std::strtof(node.child("scale").child_value(), nullptr);
    }
    if (!node.child("flip").empty()) {
        asset.flip_on_load = (std::stoi(node.child("flip").child_value()) != 0);
    }
    asset.name = name;
    asset.filepath = filepath;
    asset.type = ascension::assets::Asset_Type::Texture;

    return asset;
}

}

namespace ascension::assets {

Asset_Manager::~Asset_Manager()
{
    clear();
}

void
Asset_Manager::clear()
{
    m_texture_filepaths.clear();
    m_texture_atlas_filepaths.clear();
    m_shader_filepaths.clear();

    m_loaded_textures.clear();
    m_loaded_texture_atlas.clear();
    m_loaded_shaders.clear();
}

void
// NOLINTNEXTLINE - this needs to be recursive.
Asset_Manager::parse_asset_document(const std::string& document_filepath, const std::string& root_name)
{
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(document_filepath.c_str());

    if (!result) {
        core::log::error("Failed to load asset file {}. Error {}", document_filepath, result.description());
        return;
    }

    for (const auto& node : document.child("assets").children("asset")) {
        const std::string name = node.attribute("name").value();
        const std::string type_str = node.attribute("type").value();
        const std::string filepath = node.attribute("filepath").value();

        const auto type = magic_enum::enum_cast<Asset_Type>(type_str);
        if (!type.has_value()) {
            core::log::error("Unknown asset type {}", type_str);
            return;
        }

        std::string asset_base_path;
        if (!root_name.empty()) {
            asset_base_path = root_name + "/";
        }
        else if (type != assets::Asset_Type::Asset_List) {
            asset_base_path = type_str + "/";
        }

        if (type == Asset_Type::Asset_List) {
            parse_asset_document(filepath, asset_base_path + name);
        }
        else {
            switch (*type) {
                case Asset_Type::Texture: {
                    Texture_Asset asset = parse_texture_asset(node, name, filepath);
                    // TODO: Should probably check we're not overwriting these...
                    m_texture_filepaths[(asset_base_path + name)] = asset;
                } break;
                case Asset_Type::Texture_Atlas: {
                    Texture_Atlas_Asset asset;

                    if (node.child("asset").empty()) {
                        core::log::error("Trying to load texture atlas {} ({}) without respective texture", name, filepath);
                        continue;
                    }

                    const auto texture_node = node.child("asset");

                    const std::string child_type_value = texture_node.attribute("type").value();
                    const auto child_type = magic_enum::enum_cast<Asset_Type>(child_type_value);
                    if (!child_type.has_value()) {
                        core::log::error(
                            "Unknown asset type {} loading texture atlas {} ({})", child_type_value, name, filepath
                        );
                        continue;
                    }

                    if (*child_type != Asset_Type::Texture) {
                        core::log::error(
                            "Unexpected child asset type {} loading texture atlas {} ({})", child_type_value, name, filepath
                        );
                        continue;
                    }

                    const std::string texture_name = texture_node.attribute("name").value();
                    const std::string texture_filepath = texture_node.attribute("filepath").value();
                    Texture_Asset texture_asset = parse_texture_asset(texture_node, texture_name, texture_filepath);

                    std::string sub_texture_id = asset_base_path + texture_name;
                    m_texture_filepaths[sub_texture_id] = texture_asset;

                    asset.name = name;
                    asset.filepath = filepath;
                    asset.type = Asset_Type::Texture_Atlas;
                    asset.sub_texture_id = sub_texture_id;
                    m_texture_atlas_filepaths[(asset_base_path + name)] = asset;
                } break;
                case Asset_Type::Shader: {
                    Shader_Asset asset;
                    if (node.child("vertex").empty() || node.child("fragment").empty()) {
                        core::log::error("Trying to load shader {} ({}) without fragment or vertex source.", name, filepath);
                        continue;
                    }
                    asset.vertex_src_file = node.child("vertex").child_value();
                    asset.fragment_src_file = node.child("fragment").child_value();
                    asset.name = name;
                    asset.filepath = filepath;
                    asset.type = Asset_Type::Shader;
                    m_shader_filepaths[(asset_base_path + name)] = asset;
                } break;
                case Asset_Type::Font: {
                    Font_Asset asset;
                    asset.name = name;
                    asset.filepath = filepath;
                    asset.type = Asset_Type::Font;
                    m_font_filepaths[(asset_base_path + name)] = asset;
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

    core::log::info("Loaded {} textures", m_texture_filepaths.size());
    core::log::info("Loaded {} texture atlas", m_texture_atlas_filepaths.size());
    core::log::info("Loaded {} shaders", m_shader_filepaths.size());
    core::log::info("Loaded {} fonts", m_font_filepaths.size());
}

std::shared_ptr<graphics::Texture_2D>
Asset_Manager::load_texture_2d(const std::string& asset_name)
{
    auto texture = get_texture_2d(asset_name);
    if (texture) {
        return texture;
    }

    if (m_texture_filepaths.count(asset_name) == 0u) {
        core::log::warn("Attempting to load unrecognized texture {}", asset_name);
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

std::shared_ptr<graphics::Texture_Atlas>
Asset_Manager::load_texture_atlas(const std::string& asset_name)
{
    auto texture_atlas = get_texture_atlas(asset_name);
    if (texture_atlas) {
        return texture_atlas;
    }

    if (m_texture_atlas_filepaths.count(asset_name) == 0u) {
        core::log::warn("Attempting to load unrecognized texture atlas {}", asset_name);
        return nullptr;
    }

    Texture_Atlas_Asset asset = m_texture_atlas_filepaths[asset_name];

    const auto texture = load_texture_2d(asset.sub_texture_id);
    if (!texture) {
        core::log::error("Asset_Manager::load_texture_atlas() failed to load internal texture {}", asset.sub_texture_id);
        return nullptr;
    }

    std::ifstream file(asset.filepath, std::ifstream::in);
    if (!file.is_open()) {
        core::log::error("Asset_Manager::load_texture_atlas() failed to open file {}", asset.filepath);
        return nullptr;
    }

    std::unordered_map<std::string, v4u> sub_textures;

    std::string sub_texture_details;
    while (file.peek() != EOF) {
        std::getline(file, sub_texture_details);

        v4u image_dims{ 0 };
        std::string texture_name;
        std::stringstream sub_texture_details_stream(sub_texture_details);
        sub_texture_details_stream >> texture_name;
        sub_texture_details_stream >> image_dims.x >> image_dims.y >> image_dims.z >> image_dims.w;

        sub_textures.emplace(texture_name, image_dims);
    }

    auto new_texture_atlas = std::make_shared<graphics::Texture_Atlas>();
    new_texture_atlas->create(texture, sub_textures);

    m_loaded_texture_atlas.insert({ asset_name, new_texture_atlas });
    return new_texture_atlas;
}

std::shared_ptr<graphics::Texture_Atlas>
Asset_Manager::get_texture_atlas(const std::string& asset_name)
{
    if (m_loaded_texture_atlas.count(asset_name) == 0) {
        return nullptr;
    }

    return m_loaded_texture_atlas[asset_name];
}

void
Asset_Manager::unload_texture_atlas(const std::string& asset_name)
{
    const auto texture_atlas = get_texture_atlas(asset_name);
    if (!texture_atlas) {
        return;
    }

    const auto texture_name = m_texture_atlas_filepaths.at(asset_name).sub_texture_id;
    unload_texture_2d(texture_name);

    m_loaded_texture_atlas.erase(asset_name);
}

std::shared_ptr<graphics::Shader>
Asset_Manager::load_shader(const std::string& asset_name)
{
    auto shader = get_shader(asset_name);
    if (shader) {
        return shader;
    }

    if (m_shader_filepaths.count(asset_name) == 0u) {
        core::log::warn("Attempting to load unrecognized shader {}", asset_name);
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

std::shared_ptr<graphics::Sprite_Font>
Asset_Manager::load_font(const std::string& asset_name)
{
    auto font = get_font(asset_name);
    if (font) {
        return font;
    }

    if (m_font_filepaths.count(asset_name) == 0u) {
        core::log::warn("Attempting to load unrecognized font {}", asset_name);
        return nullptr;
    }

    Font_Asset asset = m_font_filepaths[asset_name];

    auto new_font = std::make_shared<graphics::Sprite_Font>();
    // TODO: Specify the shader in the font xml file
    new_font->create(asset.filepath, get_shader("shaders/spritefont"));

    m_loaded_fonts.insert({ asset_name, new_font });
    return new_font;
}

std::shared_ptr<graphics::Sprite_Font>
Asset_Manager::get_font(const std::string& asset_name)
{
    if (m_loaded_fonts.count(asset_name) == 0) {
        return nullptr;
    }

    return m_loaded_fonts[asset_name];
}

void
Asset_Manager::unload_font(const std::string& asset_name)
{
    const auto font = get_font(asset_name);
    if (!font) {
        return;
    }

    m_loaded_fonts.erase(asset_name);
}

}
