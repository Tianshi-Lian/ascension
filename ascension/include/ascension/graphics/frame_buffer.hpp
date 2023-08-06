/**
 * File: frame_buffer.hpp
 * Project: ascension
 * File Created: 2023-07-18 20:26:21
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-06 17:34:11
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

#ifndef ASCENSION_GRAPHICS_FRAME_BUFFER_HPP
#define ASCENSION_GRAPHICS_FRAME_BUFFER_HPP

namespace ascension::graphics {

class Texture_2D;

class Frame_Buffer {
public:
    Frame_Buffer();
    ~Frame_Buffer();

    void destroy();

    void start(u32 window_width, u32 window_height, const std::shared_ptr<Texture_2D>& target);
    void end();

    Frame_Buffer(const Frame_Buffer&) = default;
    Frame_Buffer(Frame_Buffer&&) = delete;
    Frame_Buffer& operator=(const Frame_Buffer&) = default;
    Frame_Buffer& operator=(Frame_Buffer&&) = delete;

private:
    u32 m_id;

    std::shared_ptr<Texture_2D> m_target;

    u32 m_window_width;
    u32 m_window_height;

    bool m_depth_enabled;
    u32 m_depth_buffer_id;
};

}

#endif // ASCENSION_GRAPHICS_FRAME_BUFFER_HPP
