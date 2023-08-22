/**
 * File: serialize_assets.hpp
 * Project: ascension
 * File Created: 2023-08-16 10:53:12
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-22 20:30:22
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

#include <iostream>
#include <yaml-cpp/yaml.h>

// IWYU pragma: export
namespace YAML {

template<>
struct convert<v4u> {
    static Node encode(const v4u& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, v4u& rhs)
    {
        if (node.size() != 4) {
            ascension::core::log::error("Failed to decode v4u: {}", node.Tag());
            return false;
        }

        rhs.x = node[0].as<u32>();
        rhs.y = node[1].as<u32>();
        rhs.z = node[2].as<u32>();
        rhs.w = node[3].as<u32>();

        return true;
    }
};

template<>
struct convert<ascension::assets::Asset_File> {
    static Node encode(const ascension::assets::Asset_File& rhs)
    {
        Node root;

        Node data;
        if (!rhs.filepath.empty()) {
            data["filepath"] = rhs.filepath;
        }
        data["type"] = magic_enum::enum_name(rhs.type);

        root[rhs.name] = data;
        return root;
    }

    static bool decode(const Node& root, ascension::assets::Asset_File& rhs)
    {
        rhs.name = root.begin()->first.Scalar();
        const auto node = root[rhs.name];

        if (!node["type"]) {
            ascension::core::log::error(
                "Failed to decode Asset_File unknown asset type. Name: {} Filepath: {}",
                rhs.name,
                node["filepath"].as<std::string>()
            );
            return false;
        }
        rhs.type = magic_enum::enum_cast<ascension::assets::Asset_Type>(node["type"].as<std::string>())
                       .value_or(ascension::assets::Asset_Type::Unknown);

        // NOTE: Root asset_lists won't need to have a filepath as they aren't "pointing" at any asset themselves, they are just
        // /n    holding a list of others, so we can avoid validating this for Asset_Lists.
        // TODO: We may want to explicitly set and check this, such as 'root: true', to avoid accidents.
        if (!node["filepath"] && rhs.type != ascension::assets::Asset_Type::Asset_List) {
            ascension::core::log::error(
                "Failed to decode Asset_File missing filepath. Name: {} Type: {}", rhs.name, magic_enum::enum_name(rhs.type)
            );
            return false;
        }
        if (node["filepath"]) {
            rhs.filepath = node["filepath"].as<std::string>();
        }

        if (rhs.type == ascension::assets::Asset_Type::Unknown) {
            ascension::core::log::error(
                "Failed to decode Asset_File. Unknown Asset_Type {}. Name: {} Filepath: {}",
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
struct convert<ascension::assets::Asset_List_File::Asset_List_Item> {
    static Node encode(const ascension::assets::Asset_List_File::Asset_List_Item& rhs)
    {
        Node root = convert<ascension::assets::Asset_File>::encode(rhs);

        Node data = root[rhs.name];
        data["list"] = magic_enum::enum_name(rhs.list);

        return root;
    }

    static bool decode(const Node& root, ascension::assets::Asset_List_File::Asset_List_Item& rhs)
    {
        convert<ascension::assets::Asset_File>::decode(root, rhs);
        const auto node = root[rhs.name];

        if (!node["list"]) {
            ascension::core::log::error(
                "Failed to decode Asset_List_File::Asset_List. Name: {} Filepath: {}. Invalid data.", rhs.name, rhs.filepath
            );
            return false;
        }

        rhs.list = magic_enum::enum_cast<ascension::assets::Asset_Type>(node["list"].as<std::string>())
                       .value_or(ascension::assets::Asset_Type::Unknown);

        return true;
    }
};

template<>
struct convert<ascension::assets::Asset_List_File> {
    static Node encode(const ascension::assets::Asset_List_File& rhs)
    {
        Node root = convert<ascension::assets::Asset_File>::encode(rhs);

        Node data = root[rhs.name];
        data["assets"] = rhs.assets;

        return root;
    }

    static bool decode(const Node& root, ascension::assets::Asset_List_File& rhs)
    {
        convert<ascension::assets::Asset_File>::decode(root, rhs);
        const auto node = root[rhs.name];

        rhs.assets = node["assets"].as<std::vector<ascension::assets::Asset_List_File::Asset_List_Item>>();

        return true;
    }
};

template<>
struct convert<ascension::assets::Texture_File> {
    static Node encode(const ascension::assets::Texture_File& rhs)
    {
        Node root = convert<ascension::assets::Asset_File>::encode(rhs);

        Node data = root[rhs.name];
        data["scale"] = rhs.scale;
        data["flip_on_load"] = rhs.flip_on_load;

        return root;
    }

    static bool decode(const Node& root, ascension::assets::Texture_File& rhs)
    {
        convert<ascension::assets::Asset_File>::decode(root, rhs);
        const auto node = root[rhs.name];

        if (node["scale"]) {
            rhs.scale = node["scale"].as<f32>();
        }
        if (node["flip_on_load"]) {
            rhs.flip_on_load = node["flip_on_load"].as<bool>();
        }

        return true;
    }
};

template<>
struct convert<ascension::assets::Texture_Atlas_File> {
    static Node encode(const ascension::assets::Texture_Atlas_File& rhs)
    {
        Node root = convert<ascension::assets::Asset_File>::encode(rhs);

        Node data = root[rhs.name];
        data["texture_name"] = rhs.texture_name;
        data["sub_textures"] = rhs.sub_textures;

        return root;
    }

    static bool decode(const Node& root, ascension::assets::Texture_Atlas_File& rhs)
    {
        convert<ascension::assets::Asset_File>::decode(root, rhs);
        const auto node = root[rhs.name];

        if (!node["texture_name"] || !node["sub_textures"]) {
            ascension::core::log::error(
                "Failed to decode Texture_Atlas_File. Name: {} Filepath: {}. Invalid data.", rhs.name, rhs.filepath
            );
            return false;
        }

        rhs.texture_name = node["texture_name"].as<std::string>();
        rhs.sub_textures = node["sub_textures"].as<std::unordered_map<std::string, v4u>>();

        return true;
    }
};

}

#endif // ASCENSION_ASSETS_SERIALIZE_ASSETS_HPP
