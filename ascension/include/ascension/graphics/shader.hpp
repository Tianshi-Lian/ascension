/**
 * File: shader.hpp
 * Project: ascension
 * File Created: 2023-04-11 20:33:16
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-11 20:35:59
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

namespace ascension::graphics {

class Shader {
  public:
    Shader();
    ~Shader();

    void create(const std::string& vertex_source, const std::string& fragment_source);
    void bind() const;
    static void unbind();

    Shader(const Shader&) = default;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = default;
    Shader& operator=(Shader&&) = delete;

  private:
    u32 m_id;
};

}
