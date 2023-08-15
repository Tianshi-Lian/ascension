/**
 * File: asset_types.hpp
 * Project: ascension
 * File Created: 2023-04-14 13:56:09
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-15 20:42:01
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

namespace ascension::assets {

enum class Asset_Type {
    Asset_List,
    Font,
    Shader,
    Texture_Atlas,
    Texture,
};

struct Asset_File {
    std::string name;
    std::string filepath;
    Asset_Type type;
};

struct Texture_Atlas_File : public Asset_File {
    std::string sub_texture_id;
};

struct Shader_File : public Asset_File {
    std::string vertex_src_file;
    std::string fragment_src_file;
};

struct Font_File : public Asset_File {};

}
