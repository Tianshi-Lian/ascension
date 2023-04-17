/**
 * File: application.hpp
 * Project: ascension
 * File Created: 2023-04-08 15:39:40
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-17 19:52:18
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

#include "assets/asset_manager.hpp"
namespace ascension::core {

class Application {
  public:
    Application();
    virtual ~Application();

    void setup(const std::string& app_name, v2i window_position, v2i window_size);
    i32 run();
    void quit();

    Application(const Application&) = default;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = default;
    Application& operator=(Application&&) = delete;

  protected:
    virtual void on_initialize() = 0;
    virtual void on_update(f64 delta_time) = 0;
    virtual void on_render(f32 interpolation) = 0;

    assets::Asset_Manager m_asset_manager;

  private:
    void initialize();
    void update(f64 delta_time);
    void render(f32 interpolation);

    bool m_should_quit;

    std::string m_window_title;
    v2i m_window_position;
    v2i m_window_size;

    // TODO: Move back to smart pointers once we work out context issues.
    void* m_window;
    void* m_gl_context;
};

}
