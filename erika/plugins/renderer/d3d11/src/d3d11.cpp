/**
 * File: d3d11.cpp
 * Project: erika
 * File Created: 2023-03-11 20:05:16
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-29 19:05:56
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

void
D3D11_Renderer::initialize(const std::shared_ptr<yuki::platform::Platform_State>& platform_state)
{
    yuki::debug::Logger::debug("erika", "D3D11_Renderer::initialize()");
    (void)platform_state;
}

void
D3D11_Renderer::shutdown()
{
    yuki::debug::Logger::debug("erika", "D3D11_Renderer::shutdown()");
}

void
D3D11_Renderer::begin_scene(const Clear_Color& clear_color)
{
    yuki::debug::Logger::debug("erika", "D3D11_Renderer::begin_scene()");
    (void)clear_color;
}

void
D3D11_Renderer::end_scene()
{
    yuki::debug::Logger::debug("erika", "D3D11_Renderer::end_scene()");
}

void
D3D11_Renderer::on_resize(const u32 width, const u32 height)
{
    (void)&width;
    (void)&height;
    throw std::runtime_error("Method not implemented.");
}

std::shared_ptr<Renderer>
D3D11_Renderer_Factory::create()
{
    yuki::debug::Logger::initialize("logs/renderer.log", yuki::debug::Severity::LOG_DEBUG, true, true);

    yuki::debug::Logger::debug("erika", "D3D11_Renderer_Factory::create()");
    return std::make_shared<D3D11_Renderer>();
}

}

void
register_plugin(erika::plugins::Plugin_Manager& plugin_manager)
{
    plugin_manager.register_renderer(
        "D3D11_Renderer", std::make_shared<erika::plugins::renderer::d3d11::D3D11_Renderer_Factory>()
    );
}
