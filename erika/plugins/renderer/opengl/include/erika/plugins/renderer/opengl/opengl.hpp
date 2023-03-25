#pragma once

#include "erika/plugins/plugin_manager.hpp"
#include "erika/plugins/renderer.hpp"

namespace erika::plugins::renderer::opengl {

struct OpenGL_Platform_State;

class OpenGL_Renderer : public erika::plugins::Renderer {
  public:
    OpenGL_Renderer() = default;
    ~OpenGL_Renderer() override = default;

    void initialize(const std::shared_ptr<yuki::platform::Platform_State>& platform_state) override;
    void shutdown() override;

    void begin_scene(const Clear_Color& clear_color) override;
    void end_scene() override;

    OpenGL_Renderer(const OpenGL_Renderer&) = default;
    OpenGL_Renderer(OpenGL_Renderer&&) = delete;
    OpenGL_Renderer& operator=(const OpenGL_Renderer&) = default;
    OpenGL_Renderer& operator=(OpenGL_Renderer&&) = delete;

  protected:
    void on_resize(const u32 width, const u32 height) override;

  private:
    std::shared_ptr<OpenGL_Platform_State> m_opengl_platform_state;
};

}

extern "C" void
registerPlugin(erika::plugins::Plugin_Manager& plugin_manager);
