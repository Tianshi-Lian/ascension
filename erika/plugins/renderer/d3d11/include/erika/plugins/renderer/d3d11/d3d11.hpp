/**
 * File: d3d11.hpp
 * Project: erika
 * File Created: 2023-03-11 20:05:16
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-29 19:05:50
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

    void begin_scene(const Clear_Color& clear_color) override;
    void end_scene() override;

    D3D11_Renderer(const D3D11_Renderer&) = default;
    D3D11_Renderer(D3D11_Renderer&&) = delete;
    D3D11_Renderer& operator=(const D3D11_Renderer&) = default;
    D3D11_Renderer& operator=(D3D11_Renderer&&) = delete;

  protected:
    void on_resize(const u32 width, const u32 height) override;
};

}

extern "C" void
register_plugin(erika::plugins::Plugin_Manager& plugin_manager);
