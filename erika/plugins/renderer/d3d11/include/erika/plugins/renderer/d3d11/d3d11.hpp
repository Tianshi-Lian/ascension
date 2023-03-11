#pragma once

#include "erika/core/defines.hpp"
#include "erika/plugins/renderer.hpp"

namespace erika::plugins::renderer::d3d11 {

class ERIKA_API D3D11_Renderer : public erika::plugins::Renderer {
  public:
    D3D11_Renderer();
    ~D3D11_Renderer() override = default;

    void initialize() override;
    void shutdown() override;

    void begin_scene() override;
    void end_scene() override;

    D3D11_Renderer(const D3D11_Renderer&) = default;
    D3D11_Renderer(D3D11_Renderer&&) = delete;
    D3D11_Renderer& operator=(const D3D11_Renderer&) = default;
    D3D11_Renderer& operator=(D3D11_Renderer&&) = delete;
};

class ERIKA_API D3D11_Renderer_Factory : public Renderer_Plugin_Factory {
  public:
    D3D11_Renderer_Factory() = default;
    ~D3D11_Renderer_Factory() override = default;

    std::shared_ptr<Renderer> create() override;

    D3D11_Renderer_Factory(const D3D11_Renderer_Factory&) = delete;
    D3D11_Renderer_Factory& operator=(const D3D11_Renderer_Factory&) = delete;
    D3D11_Renderer_Factory(D3D11_Renderer_Factory&&) = delete;
    D3D11_Renderer_Factory& operator=(D3D11_Renderer_Factory&&) = delete;
};

}
