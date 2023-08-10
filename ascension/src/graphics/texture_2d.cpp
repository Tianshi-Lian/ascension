/**
 * File: texture_2d.cpp
 * Project: ascension
 * File Created: 2023-04-11 19:41:46
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-10 13:33:38
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

#include "core/log.hpp"

namespace {

constexpr int
format_to_gl_format(ascension::graphics::Texture_2D::Format format)
{
    switch (format) {
        case ascension::graphics::Texture_2D::Format::RGB:
            return GL_RGB;
        case ascension::graphics::Texture_2D::Format::RGBA:
            return GL_RGBA;
        case ascension::graphics::Texture_2D::Format::RED:
            return GL_RED;
        default:
            return GL_RGBA;
    }
    return GL_RGBA;
}

}
namespace ascension::graphics {

Texture_2D::Texture_2D()
  : m_id(0)
  , m_width(0)
  , m_height(0)
  , m_format(Format::RGBA)
  , m_texture_coords(0.0f, 0.0f, 1.0f, 1.0f)
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
Texture_2D::create(u32 width, u32 height, u8* data, Format format)
{
    create(width, height, { 0.0f, 0.0f, 1.0f, 1.0f }, data, format);
}

void
Texture_2D::create(u32 width, u32 height, v4f texture_coords, u8* data, Format format)
{
    m_width = width;
    m_height = height;

    m_format = format;

    m_texture_coords = texture_coords;

    glGenTextures(1, &m_id);

    if (m_id == 0) {
        core::log::error("Failed to create texture with error {}", glGetError());
        return;
    }

    i32 previous_pixel_store = 4;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previous_pixel_store);

    const i32 gl_format = format_to_gl_format(format);
    // if (format == Format::RED) {
    //  NOTE: We generate some textures on the fly such as texture atlases for fonts, the font data loaded by
    //  /n    TrueType is stored in single alignment in the red channel, which is then filter later in the shader.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //}

    bind();

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        gl_format,
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height),
        0,
        static_cast<GLenum>(gl_format),
        GL_UNSIGNED_BYTE,
        data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unbind();

    // if (format == Format::RED) {
    //     glPixelStorei(GL_UNPACK_ALIGNMENT, previous_pixel_store);
    // }
}

void
Texture_2D::bind() const
{
    glActiveTexture(GL_TEXTURE0);
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

v2u
Texture_2D::size() const
{
    return { m_width, m_height };
}

v4f
Texture_2D::texture_coords() const
{
    return m_texture_coords;
}

bool
operator==(const Texture_2D& tex_1, const Texture_2D& tex_2)
{
    return tex_1.m_id == tex_2.m_id;
}

bool
operator!=(const Texture_2D& tex_1, const Texture_2D& tex_2)
{
    return tex_1.m_id != tex_2.m_id;
}

bool
operator<(const Texture_2D& tex_1, const Texture_2D& tex_2)
{
    return tex_1.m_id < tex_2.m_id;
}

bool
operator>(const Texture_2D& tex_1, const Texture_2D& tex_2)
{
    return tex_1.m_id > tex_2.m_id;
}

}
