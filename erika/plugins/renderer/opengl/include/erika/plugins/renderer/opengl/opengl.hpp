/**
 * File: opengl.hpp
 * Project: erika
 * File Created: 2023-03-27 19:45:50
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:04:16
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
