#pragma once

#include "erika/plugins/plugin_manager.hpp"
#include "erika/plugins/renderer.hpp"

namespace erika::plugins::renderer::d3d11 {
class D3D11_Renderer : public erika::plugins::Renderer {
  public:
    D3D11_Renderer() = default;
    ~D3D11_Renderer() override = default;

    void initialize(const std::shared_ptr<yuki::platform::Platform_State>& platform_state) override;
    void shutdown() override;

    void begin_scene() override;
    void end_scene() override;

    D3D11_Renderer(const D3D11_Renderer&) = default;
    D3D11_Renderer(D3D11_Renderer&&) = delete;
    D3D11_Renderer& operator=(const D3D11_Renderer&) = default;
    D3D11_Renderer& operator=(D3D11_Renderer&&) = delete;
};

}

extern "C" void
registerPlugin(erika::plugins::Plugin_Manager& plugin_manager);
