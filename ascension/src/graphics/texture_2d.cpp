/**
 * File: texture_2d.cpp
 * Project: ascension
 * File Created: 2023-04-11 19:41:46
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-11 20:11:54
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

#include "graphics/texture_2d.hpp"

#include <GL/glew.h>

#include "yuki/debug/logger.hpp"

namespace ascension::graphics {

Texture_2D::Texture_2D()
  : m_id(0)
  , m_width(0)
  , m_height(0)
{
}

Texture_2D::~Texture_2D()
{
    if (m_id > 0) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

void
Texture_2D::create(u32 width, u32 height, u8* data)
{
    glGenTextures(1, &m_id);

    if (m_id == 0) {
        yuki::debug::Logger::error("ascension", "Failed to create texture");
        return;
    }

    m_width = width;
    m_height = height;

    bind();

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unbind();
}

void
Texture_2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void
Texture_2D::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

u32
Texture_2D::id() const
{
    return m_id;
}

u32
Texture_2D::width() const
{
    return m_width;
}

u32
Texture_2D::height() const
{
    return m_height;
}

}
