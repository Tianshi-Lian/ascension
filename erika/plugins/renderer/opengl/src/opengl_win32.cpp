/**
 * File: opengl_win32.cpp
 * Project: erika
 * File Created: 2023-03-27 19:45:50
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

#include "opengl_platform.hpp"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <glad/gl.h>
#include <glad/wgl.h>

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include "yuki/platform/platform_types.hpp"

namespace {
struct Platform_Internal_State {
    HINSTANCE instance;
    HWND window;

    f64 clock_frequency;
    LARGE_INTEGER start_time;
};

struct OpenGL_Internal_State {
    HDC device_context;
    HGLRC render_context;
};

}

namespace erika::plugins::renderer::opengl {

bool
OpenGL_Platform::create_context(
    const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state,
    const std::shared_ptr<yuki::Platform_State>& app_platform_state
)
{
    PROFILE_FUNCTION();
    yuki::debug::Logger::info("erika", "Creating Win32 OpenGL context...");

    opengl_platform_state->internal_state = std::make_shared<OpenGL_Internal_State>();
    const auto& opengl_state{ std::static_pointer_cast<OpenGL_Internal_State>(opengl_platform_state->internal_state) };

    const auto& platform_state{ std::static_pointer_cast<Platform_Internal_State>(app_platform_state->internal_state) };

    opengl_state->device_context = GetDC(platform_state->window);

    // Set the pixel format for the device context:
    // Double buffering, opengl window, 32 bits of color.
    PIXELFORMATDESCRIPTOR pixel_format_descriptor = {};
    pixel_format_descriptor.nSize = sizeof(pixel_format_descriptor);
    pixel_format_descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixel_format_descriptor.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
    pixel_format_descriptor.cColorBits = 32;
    pixel_format_descriptor.cDepthBits = 32;
    pixel_format_descriptor.iLayerType = PFD_MAIN_PLANE;

    i32 format = ChoosePixelFormat(opengl_state->device_context, &pixel_format_descriptor);
    if (format == 0 || SetPixelFormat(opengl_state->device_context, format, &pixel_format_descriptor) == FALSE) {
        ReleaseDC(platform_state->window, opengl_state->device_context);
        DestroyWindow(platform_state->window);
        yuki::debug::Logger::critical("erika", "Failed to set a compatible pixel format!");
        return false;
    }
    // Create and enable a temporary (helper) opengl context:
    HGLRC temp_context = wglCreateContext(opengl_state->device_context);
    if (temp_context == nullptr) {
        ReleaseDC(platform_state->window, opengl_state->device_context);
        DestroyWindow(platform_state->window);
        yuki::debug::Logger::critical("erika", "Failed to create the initial rendering context!");
        return false;
    }
    wglMakeCurrent(opengl_state->device_context, temp_context);

    // Load WGL Extensions:
    gladLoaderLoadWGL(opengl_state->device_context);

    i32 attributes[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        4, // OpenGL MAJOR 4
        WGL_CONTEXT_MINOR_VERSION_ARB,
        6, // OpenGL MINOR 6
        WGL_CONTEXT_FLAGS_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0 // Core profile
    };

    // Create the final opengl context and get rid of the temporary one:
    HGLRC opengl_context = wglCreateContextAttribsARB(opengl_state->device_context, nullptr, attributes);
    if (opengl_context == nullptr) {
        wglDeleteContext(temp_context);
        ReleaseDC(platform_state->window, opengl_state->device_context);
        DestroyWindow(platform_state->window);
        yuki::debug::Logger::critical("erika", "Failed to create the final rendering context!");
        return false;
    }
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(temp_context);
    wglMakeCurrent(opengl_state->device_context, opengl_context);

    // Finally initialize glad.
    if (!gladLoaderLoadGL()) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(opengl_context);
        ReleaseDC(platform_state->window, opengl_state->device_context);
        DestroyWindow(platform_state->window);
        yuki::debug::Logger::critical("erika", "Glad Loader failed!");
        return false;
    }

    // Show & Update the main window:
    ShowWindow(platform_state->window, SW_SHOW);
    UpdateWindow(platform_state->window);

    return true;
}

void
OpenGL_Platform::swap_buffers(const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state)
{
    const auto& opengl_state{ std::static_pointer_cast<OpenGL_Internal_State>(opengl_platform_state->internal_state) };

    SwapBuffers(opengl_state->device_context);
}

}

#endif
