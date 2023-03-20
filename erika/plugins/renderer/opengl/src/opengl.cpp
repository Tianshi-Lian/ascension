#include "opengl.hpp"

#include "yuki/debug/logger.hpp"

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
OpenGL_Renderer::initialize()
{
    yuki::debug::Logger::debug("erika > OpenGL_Renderer::initialize()");
}

void
OpenGL_Renderer::shutdown()
{
    yuki::debug::Logger::debug("erika > OpenGL_Renderer::shutdown()");
}

void
OpenGL_Renderer::begin_scene()
{
    yuki::debug::Logger::debug("erika > OpenGL_Renderer::begin_scene()");
}

void
OpenGL_Renderer::end_scene()
{
    yuki::debug::Logger::debug("erika > OpenGL_Renderer::end_scene()");
}

std::shared_ptr<Renderer>
OpenGL_Renderer_Factory::create()
{
    yuki::debug::Logger::initialize("logs/renderer.log", yuki::debug::Log_Level::DEBUG, true, true);

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
