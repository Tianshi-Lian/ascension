#include "opengl.hpp"

#include "glad/glad.h"

#include "yuki/debug/logger.hpp"

#include "opengl_platform.hpp"

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
    yuki::debug::Logger::debug("erika > OpenGL_Renderer::initialize()");

    m_opengl_platform_state = std::make_shared<OpenGL_Platform_State>();

    OpenGL_Platform::create_context(m_opengl_platform_state, platform_state);

    // Setup OpenGL now we have a context.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void
OpenGL_Renderer::shutdown()
{
    yuki::debug::Logger::debug("erika > OpenGL_Renderer::shutdown()");
}

void
OpenGL_Renderer::begin_scene(const Clear_Colour& clear_color)
{
    yuki::debug::Logger::debug("erika > OpenGL_Renderer::begin_scene()");

    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    // NOLINTNEXTLINE - old opengl shenanigans.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void
OpenGL_Renderer::end_scene()
{
    yuki::debug::Logger::debug("erika > OpenGL_Renderer::end_scene()");

    OpenGL_Platform::swap_buffers(m_opengl_platform_state);
    glFinish();
}

void
OpenGL_Renderer::on_resize(const u32 width, const u32 height)
{
    assert(width <= INT_MAX);
    assert(height <= INT_MAX);
    glViewport(0, 0, static_cast<i32>(width), static_cast<i32>(height));
}

std::shared_ptr<Renderer>
OpenGL_Renderer_Factory::create()
{
    yuki::debug::Logger::initialize("logs/renderer.log", yuki::debug::Severity::LOG_DEBUG, true, true);

    yuki::debug::Logger::debug("erika > OpenGL_Renderer_Factory::create()");
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
