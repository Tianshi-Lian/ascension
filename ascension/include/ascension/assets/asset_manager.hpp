/**
 * File: asset_manager.hpp
 * Project: ascension
 * File Created: 2023-04-13 14:45:21
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-13 17:43:06
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

namespace ascension::graphics {
class Texture_2D;
class Shader;
}

namespace ascension::assets {

enum class Asset_Type {
    Asset_List,
    Texture,
    Shader,
};

class Asset_Manager {
  public:
    ~Asset_Manager();

    void load_asset_file(const std::string& asset_file);

    std::shared_ptr<graphics::Texture_2D> load_texture_2d(const std::string& asset_name);

    Asset_Manager(const Asset_Manager&) = default;
    Asset_Manager(Asset_Manager&&) = delete;
    Asset_Manager& operator=(const Asset_Manager&) = default;
    Asset_Manager& operator=(Asset_Manager&&) = delete;

  private:
    std::unordered_map<std::string, std::string> m_asset_filepaths;
};

}
