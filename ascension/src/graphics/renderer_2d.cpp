/**
 * File: renderer_2d.cpp
 * Project: ascension
 * File Created: 2023-04-29 17:02:08
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-29 17:13:22
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

namespace ascension::graphics {

bool
Renderer_2D::initialize()
{
    const u32 result = glewInit();
    if (result != GLEW_OK) {
        core::log::critical("Failed to initialise glew! Error {}", result);
        return false;
    }

    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void
Renderer_2D::set_clear_color(v4f color)
{
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

}
