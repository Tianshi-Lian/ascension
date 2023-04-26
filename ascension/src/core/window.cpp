/**
 * File: window.cpp
 * Project: ascension
 * File Created: 2023-04-18 18:53:27
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-26 15:20:01
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
#include "core/log.hpp"

#include <GL/glew.h>
#include <SDL.h>

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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OpenGL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OpenGL_MINOR);

    m_internal_window = SDL_CreateWindow(title.c_str(), pos_x, pos_y, width, height, SDL_WINDOW_OPENGL);
    m_internal_context = SDL_GL_CreateContext(static_cast<SDL_Window*>(m_internal_window));

    if (glewInit() != GLEW_OK) {
        core::log::critical("Failed to initialise glew! Error {}", SDL_GetError());
        return false;
    }

    if (SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_internal_window), m_internal_context) != 0) {
        core::log::critical("Failed to set window OpenGL context! Error {}", SDL_GetError());
        return false;
    }

    m_pos_x = static_cast<u32>(pos_x);
    m_pos_y = static_cast<u32>(pos_y);
    m_width = static_cast<u32>(width);
    m_height = static_cast<u32>(height);

    // TODO: Double check we got (near) our requested OpenGL attributes.

    // TODO: Allow the application (user) to set these.
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void
Window::clear() // NOLINT
{
    // TODO: Allow setting the clear properties and or color.
    glClear(GL_COLOR_BUFFER_BIT);
}

void
Window::flip()
{
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
