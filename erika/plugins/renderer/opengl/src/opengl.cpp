/**
 * File: opengl.cpp
 * Project: erika
 * File Created: 2023-04-06 16:09:06
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:04:23
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

#include "opengl.hpp"

#include <climits>

#include <glad/gl.h>

#include <mraylib/rlgl.hpp>
#include <mraylib/rlmath.hpp>
#include <mraylib/rlshapes.hpp>
#include <mraylib/rltext.hpp>

#include "yuki/debug/logger.hpp"

#include "opengl_platform.hpp"

// #define GL_SANDBOX

namespace {
#ifdef GL_SANDBOX
[[maybe_unused]] void
sandbox()
{
    constexpr auto vertex_shader_source = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main()
    {
       gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
        )";

    u32 vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    constexpr auto fragment_shader_source = R"(
    #version 330 core
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
        )";

    u32 fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    u32 shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // bl
        0.5f,  -0.5f, 0.0f, // br
        0.5f,  0.5f,  0.0f, // tr
        -0.5f, 0.5f,  0.0f, // tl
    };

    unsigned int indices[] = {
        0, 1, 2, // br
        2, 3, 0  // tl
    };

    u32 vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    u32 vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    u32 element_buffer;
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(shader_program);
    glBindVertexArray(vertex_array);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
#endif
}

namespace erika::plugins::renderer::opengl {

class OpenGL_Renderer_Factory : public Renderer_Plugin_Factory {
  public:
    OpenGL_Renderer_Factory() = default;
    ~OpenGL_Renderer_Factory() override = default;

    std::shared_ptr<Renderer> create() override;

    OpenGL_Renderer_Factory(const OpenGL_Renderer_Factory&) = delete;
    OpenGL_Renderer_Factory& operator=(const OpenGL_Renderer_Factory&) = delete;
    OpenGL_Renderer_Factory(OpenGL_Renderer_Factory&&) = delete;
    OpenGL_Renderer_Factory& operator=(OpenGL_Renderer_Factory&&) = delete;
};

void
OpenGL_Renderer::initialize(const std::shared_ptr<yuki::platform::Platform_State>& platform_state)
{
    yuki::debug::Logger::debug("erika", "OpenGL_Renderer::initialize()");

    m_opengl_platform_state = std::make_shared<OpenGL_Platform_State>();

    OpenGL_Platform::create_context(m_opengl_platform_state, platform_state);

    // Setup OpenGL through RayLib now we have a context.
    rlLoadExtensions(nullptr);
    rlglInit(1600, 900);

    yuki::debug::Logger::debug("erika", "GL: OpenGL device information:");
    // NOLINTNEXTLINE - I hate opengl
    yuki::debug::Logger::debug("     ", "Vendor:   {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    yuki::debug::Logger::debug("     ", "Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER))); // NOLINT
    yuki::debug::Logger::debug("     ", "Version:  {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));  // NOLINT
    yuki::debug::Logger::debug(
        "     ", "GLSL:     {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)) // NOLINT
    );

    rlClearColor(
        static_cast<u8>(Renderer::Default_Clear_Color.r * 255),
        static_cast<u8>(Renderer::Default_Clear_Color.g * 255),
        static_cast<u8>(Renderer::Default_Clear_Color.b * 255),
        static_cast<u8>(Renderer::Default_Clear_Color.a * 255)
    );
    rlEnableDepthTest();

    // Initialize viewport and internal projection/modelview matrices
    rlViewport(0, 0, 1600, 900);
    rlMatrixMode(RL_PROJECTION);        // Switch to PROJECTION matrix
    rlLoadIdentity();                   // Reset current matrix (PROJECTION)
    rlOrtho(0, 1600, 900, 0, 0.0, 1.0); // Orthographic projection with top-left corner at (0,0)
    rlMatrixMode(RL_MODELVIEW);         // Switch back to MODELVIEW matrix
    rlLoadIdentity();                   // Reset current matrix (MODELVIEW)

    rlLoadFontDefault();
}

void
OpenGL_Renderer::shutdown()
{
    yuki::debug::Logger::debug("erika", "OpenGL_Renderer::shutdown()");
}

void
OpenGL_Renderer::begin_scene(const Clear_Color& clear_color)
{
    (void)clear_color;

    rlClearScreenBuffers();
    rlLoadIdentity(); // Reset internal modelview matrix
}

void
OpenGL_Renderer::end_scene()
{
    rlDrawRenderBatchActive();
    OpenGL_Platform::swap_buffers(m_opengl_platform_state);
}

void
OpenGL_Renderer::on_resize(const u32 width, const u32 height)
{
    assert(width <= INT_MAX);
    assert(height <= INT_MAX);
    rlViewport(0, 0, static_cast<i32>(width), static_cast<i32>(height));
    rlMatrixMode(RL_PROJECTION);            // Switch to PROJECTION matrix
    rlLoadIdentity();                       // Reset current matrix (PROJECTION)
    rlOrtho(0, width, height, 0, 0.0, 1.0); // Orthographic projection with top-left corner at (0,0)
    rlMatrixMode(RL_MODELVIEW);             // Switch back to MODELVIEW matrix
    rlLoadIdentity();                       // Reset current matrix (MODELVIEW)
}

std::shared_ptr<Renderer>
OpenGL_Renderer_Factory::create()
{
    yuki::debug::Logger::initialize("logs/renderer.log", yuki::debug::Severity::LOG_DEBUG, true, true);

    yuki::debug::Logger::debug("erika", "OpenGL_Renderer_Factory::create()");
    return std::make_shared<OpenGL_Renderer>();
}

}

void
registerPlugin(erika::plugins::Plugin_Manager& plugin_manager)
{
    plugin_manager.register_renderer(
        "OpenGL_Renderer", std::make_shared<erika::plugins::renderer::opengl::OpenGL_Renderer_Factory>()
    );
}
