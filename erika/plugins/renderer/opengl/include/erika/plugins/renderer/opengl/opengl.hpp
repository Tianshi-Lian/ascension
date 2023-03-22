#pragma once

#include "erika/plugins/plugin_manager.hpp"
#include "erika/plugins/renderer.hpp"

namespace erika::plugins::renderer::opengl {
class OpenGL_Renderer : public erika::plugins::Renderer {
  public:
    OpenGL_Renderer() = default;
    ~OpenGL_Renderer() override = default;

    void initialize(const std::shared_ptr<yuki::platform::Platform_State>& platform_state) override;
    void shutdown() override;

    void begin_scene() override;
    void end_scene() override;

    OpenGL_Renderer(const OpenGL_Renderer&) = default;
    OpenGL_Renderer(OpenGL_Renderer&&) = delete;
    OpenGL_Renderer& operator=(const OpenGL_Renderer&) = default;
    OpenGL_Renderer& operator=(OpenGL_Renderer&&) = delete;
};

}

extern "C" void
registerPlugin(erika::plugins::Plugin_Manager& plugin_manager);
