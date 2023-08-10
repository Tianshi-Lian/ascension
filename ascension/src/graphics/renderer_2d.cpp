/**
 * File: renderer_2d.cpp
 * Project: ascension
 * File Created: 2023-04-29 17:02:08
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-10 12:04:25
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

#include "graphics/renderer_2d.hpp"

#include <GL/glew.h>

#include "core/log.hpp"

#include "graphics/sprite_font.hpp"

namespace ascension::graphics {

bool Renderer_2D::s_initialized = false;

bool
Renderer_2D::initialize()
{
    if (s_initialized) {
        core::log::error("Attempting to initialize static Renderer_2D more than once");
        return true;
    }

    const u32 result = glewInit();
    if (result != GLEW_OK) {
        core::log::critical("Failed to initialise glew! Error {}", result);
        return false;
    }

    if (!Sprite_Font::initialize()) {
        return false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // and if it didn't work, then disable depth testing by uncommenting this:
    glDisable(GL_DEPTH_TEST);

    s_initialized = true;

    return true;
}

void
Renderer_2D::set_clear_color(v4f color)
{
    assert(s_initialized);
    glClearColor(color.r, color.g, color.b, color.a);
}

void
Renderer_2D::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void
Renderer_2D::enable_blending(Blend_Function blend_func)
{
    glEnable(GL_BLEND);
    switch (blend_func) {
        case Blend_Function::SRC_COLOR:
            glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
            break;
        case Blend_Function::DST_COLOR:
            glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR);
            break;
        case Blend_Function::SRC_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case Blend_Function::DST_ALPHA:
            glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
            break;
    }
}

bool
Renderer_2D::is_initialized()
{
    return s_initialized;
}

}
