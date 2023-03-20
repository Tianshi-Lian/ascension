#include "d3d11.hpp"

#include "yuki/debug/logger.hpp"

namespace erika::plugins::renderer::d3d11 {

class D3D11_Renderer_Factory : public Renderer_Plugin_Factory {
  public:
    D3D11_Renderer_Factory() = default;
    ~D3D11_Renderer_Factory() override = default;

    std::shared_ptr<Renderer> create() override;

    D3D11_Renderer_Factory(const D3D11_Renderer_Factory&) = delete;
    D3D11_Renderer_Factory& operator=(const D3D11_Renderer_Factory&) = delete;
    D3D11_Renderer_Factory(D3D11_Renderer_Factory&&) = delete;
    D3D11_Renderer_Factory& operator=(D3D11_Renderer_Factory&&) = delete;
};

D3D11_Renderer::D3D11_Renderer()
  : erika::plugins::Renderer("D3D11_Renderer")
{
}

void
D3D11_Renderer::initialize()
{
    yuki::debug::Logger::debug("erika > D3D11_Renderer::initialize()");
}

void
D3D11_Renderer::shutdown()
{
    yuki::debug::Logger::debug("erika > D3D11_Renderer::shutdown()");
}

void
D3D11_Renderer::begin_scene()
{
    yuki::debug::Logger::debug("erika > D3D11_Renderer::begin_scene()");
}

void
D3D11_Renderer::end_scene()
{
    yuki::debug::Logger::debug("erika > D3D11_Renderer::end_scene()");
}

std::shared_ptr<Renderer>
D3D11_Renderer_Factory::create()
{
    yuki::debug::Logger::initialize("logs/renderer.log", yuki::debug::Log_Level::DEBUG, true, true);

    yuki::debug::Logger::debug("erika > D3D11_Renderer_Factory::create()");
    return std::make_shared<D3D11_Renderer>();
}

}

void
registerPlugin(erika::plugins::Plugin_Manager& plugin_manager)
{
    plugin_manager.register_renderer(
        "D3D11_Renderer", std::make_shared<erika::plugins::renderer::d3d11::D3D11_Renderer_Factory>()
    );
}
