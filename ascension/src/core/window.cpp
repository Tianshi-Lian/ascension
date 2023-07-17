/**
 * File: window.cpp
 * Project: ascension
 * File Created: 2023-04-18 18:53:27
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-17 21:22:54
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

#include "core/window.hpp"

#include <SDL.h>

#include "yuki/debug/instrumentor.hpp"

#include "core/log.hpp"
#include "graphics/renderer_2d.hpp"

namespace ascension::core {

constexpr static i32 OpenGL_MAJOR = 4;
constexpr static i32 OpenGL_MINOR = 3;

Window::Window()
  : m_internal_window(nullptr)
  , m_internal_context(nullptr)
  , m_pos_x(0)
  , m_pos_y(0)
  , m_width(0)
  , m_height(0)
{
}

Window::~Window()
{
    if (m_internal_context != nullptr) {
        SDL_GL_DeleteContext(m_internal_context);
        m_internal_context = nullptr;
    }

    if (m_internal_window != nullptr) {
        SDL_DestroyWindow(static_cast<SDL_Window*>(m_internal_window));
        m_internal_context = nullptr;
    }
}

bool
Window::create(const std::string& title, i32 pos_x, i32 pos_y, i32 width, i32 height)
{
    // TODO: Move all of this to some sort of larger render manager/system to remove the need for
    // /t    window to require awareness of renderer and it's order of initialization etc. as this
    // /t    is dependent on the underlying libraries. The top-level render system can check which
    // /t    libraries we want to use and then control the creation & initialization itself.

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OpenGL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OpenGL_MINOR);

    m_internal_window = SDL_CreateWindow(title.c_str(), pos_x, pos_y, width, height, SDL_WINDOW_OPENGL);
    m_internal_context = SDL_GL_CreateContext(static_cast<SDL_Window*>(m_internal_window));

    if (!graphics::Renderer_2D::initialize()) {
        core::log::critical("Failed to initialize renderer for window!");
        return false;
    }

    if (SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_internal_window), m_internal_context) != 0) {
        core::log::critical("Failed to set window OpenGL context! Error {}", SDL_GetError());
        return false;
    }

    SDL_GL_SetSwapInterval(0);

    m_pos_x = static_cast<u32>(pos_x);
    m_pos_y = static_cast<u32>(pos_y);
    m_width = static_cast<u32>(width);
    m_height = static_cast<u32>(height);

    // TODO: Double check we got (near) our requested OpenGL attributes.

    return true;
}

void
Window::clear() // NOLINT
{
    PROFILE_FUNCTION();
    graphics::Renderer_2D::clear();
}

void
Window::flip()
{
    PROFILE_FUNCTION();
    SDL_GL_SwapWindow(static_cast<SDL_Window*>(m_internal_window));
}

void
Window::resize(u32 width, u32 height) // NOLINT
{
    // TODO: Implement window (renderer) resizing.
    (void)width;
    (void)height;
}

[[nodiscard]] u32
Window::get_pos_x() const
{
    return m_pos_x;
}
[[nodiscard]] u32
Window::get_pos_y() const
{
    return m_pos_y;
}

[[nodiscard]] u32
Window::get_width() const
{
    return m_width;
}
[[nodiscard]] u32
Window::get_height() const
{
    return m_height;
}

}
