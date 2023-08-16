/**
 * File: serialize_assets.hpp
 * Project: ascension
 * File Created: 2023-08-16 10:53:12
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-16 11:24:01
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

#ifndef ASCENSION_ASSETS_SERIALIZE_ASSETS_HPP
#define ASCENSION_ASSETS_SERIALIZE_ASSETS_HPP

#include "assets/asset_types.hpp"
#include "core/log.hpp"

#include <yaml-cpp/yaml.h>

// IWYU pragma: export
namespace YAML {

template<>
struct convert<ascension::assets::Asset_File> {
    static Node encode(const ascension::assets::Asset_File& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["filepath"] = rhs.filepath;
        node["type"] = magic_enum::enum_name(rhs.type);
        return node;
    }

    static bool decode(const Node& node, ascension::assets::Asset_File& rhs)
    {
        if (!node["name"] || !node["filepath"] || !node["type"]) {
            ascension::core::log::error(
                "Failed to decode Asset_File: {}. Name: {} Filepath: {}",
                node.Tag(),
                node["name"].as<std::string>(),
                node["filepath"].as<std::string>()
            );
            return false;
        }

        rhs.name = node["name"].as<std::string>();
        rhs.filepath = node["filepath"].as<std::string>();
        rhs.type = magic_enum::enum_cast<ascension::assets::Asset_Type>(node["type"].as<std::string>())
                       .value_or(ascension::assets::Asset_Type::Unknown);

        if (rhs.type == ascension::assets::Asset_Type::Unknown) {
            ascension::core::log::error(
                "Failed to decode Asset_File: {}, unknown Asset_Type {}. Name: {} Filepath: {}",
                node.Tag(),
                node["type"].as<std::string>(),
                rhs.name,
                rhs.filepath
            );
            return false;
        }

        return true;
    }
};

template<>
struct convert<ascension::assets::Texture_File> {
    static Node encode(const ascension::assets::Texture_File& rhs)
    {
        Node node = convert<ascension::assets::Asset_File>::encode(rhs);
        node["scale"] = rhs.scale;
        node["flip_on_load"] = rhs.flip_on_load;
        return node;
    }

    static bool decode(const Node& node, ascension::assets::Texture_File& rhs)
    {
        convert<ascension::assets::Asset_File>::decode(node, rhs);

        if (node["scale"]) {
            rhs.scale = node["scale"].as<f32>();
        }
        if (node["flip_on_load"]) {
            rhs.flip_on_load = node["flip_on_load"].as<bool>();
        }

        return true;
    }
};
}

#endif // ASCENSION_ASSETS_SERIALIZE_ASSETS_HPP
