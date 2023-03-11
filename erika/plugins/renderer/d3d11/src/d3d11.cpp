#include "d3d11.hpp"

#include "yuki/debug/logger.hpp"

namespace erika::plugins::renderer::d3d11 {

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
    yuki::debug::Logger::initialize("logs/plugins.log", yuki::debug::Log_Level::DEBUG, true, true);
    yuki::debug::Logger::debug("erika > D3D11_Renderer_Factory::create()");
    return std::make_shared<D3D11_Renderer>();
}

}
