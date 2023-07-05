/**
 * File: window.hpp
 * Project: ascension
 * File Created: 2023-04-18 17:50:10
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-18 19:47:49
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

namespace ascension::core {

class Window {
public:
    Window();
    ~Window();

    // TODO: Make pos & size types consistent.
    bool create(const std::string& title, i32 pos_X, i32 pos_y, i32 width, i32 height);

    void clear();
    void flip();

    void resize(u32 width, u32 height);

    [[nodiscard]] u32 get_pos_x() const;
    [[nodiscard]] u32 get_pos_y() const;
    [[nodiscard]] u32 get_width() const;
    [[nodiscard]] u32 get_height() const;

    Window(const Window&) = default;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = default;
    Window& operator=(Window&&) = delete;

private:
    void* m_internal_window;
    void* m_internal_context;

    u32 m_pos_x;
    u32 m_pos_y;
    u32 m_width;
    u32 m_height;
};

}
