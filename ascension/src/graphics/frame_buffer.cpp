/**
 * File: frame_buffer.cpp
 * Project: ascension
 * File Created: 2023-07-18 20:35:33
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-07 14:49:40
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

#include "graphics/frame_buffer.hpp"

#include <GL/glew.h>

#include "core/log.hpp"
#include "graphics/texture_2d.hpp"

namespace ascension::graphics {

Frame_Buffer::Frame_Buffer()
  : m_id(0)
  , m_target(nullptr)
  , m_window_width(0)
  , m_window_height(0)
{
}

Frame_Buffer::~Frame_Buffer()
{
    destroy();
}

void
Frame_Buffer::destroy()
{
    if (m_id != 0) {
        glDeleteFramebuffers(1, &m_id);
        m_id = 0;
    }
}

void
Frame_Buffer::start(u32 window_width, u32 window_height, const std::shared_ptr<Texture_2D>& target)
{
    if (m_target != nullptr) {
        core::log::error("Attempting start frame buffer which already has a target");
        return;
    }

    if (window_width == 0 || window_height == 0) {
        core::log::warn("Attempting to start frame buffer with unexpected width {} or height {}", window_width, window_height);
    }

    m_target = target;
    m_window_width = window_width;
    m_window_height = window_height;

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    glBindTexture(GL_TEXTURE_2D, target->id());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target->id(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        core::log::error("Failed to create frame buffer");
    }

    glViewport(0, 0, static_cast<i32>(m_target->width()), static_cast<i32>(m_target->height()));

    // TODO: If/once we support z positions in our rendering we should attach a depth component & renderbuffer
}

void
Frame_Buffer::end()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // TODO: We're almost certainly going to want to be able to get this in a static way so we can avoid passing
    // /t    it through objects such as Sprite_Font and so we can grab it right before use in case it has changed.
    glViewport(0, 0, static_cast<i32>(m_window_width), static_cast<i32>(m_window_height));
    destroy();
}

}
