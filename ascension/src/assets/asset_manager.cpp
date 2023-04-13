/**
 * File: asset_manager.cpp
 * Project: ascension
 * File Created: 2023-04-13 15:04:17
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-13 18:06:19
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

#include <magic_enum/magic_enum.hpp>
#include <pugixml.hpp>

#include "yuki/debug/logger.hpp"

namespace {

std::unordered_map<std::string, std::string>
parse_asset_document(const pugi::xml_document& document, const std::string& root) // NOLINT
{
    std::unordered_map<std::string, std::string> loaded_assets = {};

    for (const auto& node : document.child("Assets").children("Asset")) {
        const std::string name = node.attribute("Name").value();
        const std::string type_str = node.attribute("Type").value();
        const auto type = magic_enum::enum_cast<ascension::assets::Asset_Type>(type_str);
        const std::string filepath = node.attribute("Filepath").value();

        std::string asset_base_path;
        if (!root.empty()) {
            asset_base_path = root + "/";
        }
        else {
            asset_base_path = type_str + "/";
        }

        if (type == ascension::assets::Asset_Type::Asset_List) {
            pugi::xml_document sub_document;
            pugi::xml_parse_result result = sub_document.load_file(filepath.c_str());

            if (!result) {
                yuki::debug::Logger::error(
                    "ascension", "Failed to load asset file {}. Error {}", filepath, result.description()
                );
                continue;
            }
            const auto sub_assets = parse_asset_document(sub_document, asset_base_path + name);
            loaded_assets.insert(sub_assets.begin(), sub_assets.end());
        }
        else {
            loaded_assets[(asset_base_path + name)] = filepath;
        }
    }

    return loaded_assets;
}

}

namespace ascension::assets {

void
Asset_Manager::load_asset_file(const std::string& asset_file)
{
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(asset_file.c_str());

    if (!result) {
        yuki::debug::Logger::error("ascension", "Failed to load asset file {}. Error {}", asset_file, result.description());
        return;
    }

    m_asset_filepaths = parse_asset_document(document, "");
}

}
