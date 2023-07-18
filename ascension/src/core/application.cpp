/**
 * File: application.cpp
 * Project: ascension
 * File Created: 2023-04-08 15:43:49
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-18 20:54:40
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

#include "core/application.hpp"

#include <SDL.h>

#include "yuki/debug/instrumentor.hpp"
#include "yuki/platform/platform.hpp"

#include "core/log.hpp"

namespace {

constexpr f64 millisecond_per_second = 1000.0;
constexpr i32 updates_per_second = 60;
constexpr f64 skip_update_ms = millisecond_per_second / updates_per_second;
constexpr i32 max_skipped_frames = 5;

}

namespace ascension::core {

Application::Application()
  : m_window(nullptr)
  , m_should_quit(false)
{
}

bool
Application::initialize(const std::string& app_name, i32 pos_x, i32 pos_y, i32 width, i32 height)
{
    // TODO: Potentially make a secondary application abstraction which wraps the 3rd-party library we are using
    // /t    such as SDL, so that Application & especially the event loop will be all our own code and our events
    // /t    which we can tie in to our own event & messaging systems without coupling to SDL.

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0) {
        core::log::critical("Failed to initialise SDL! Error {}", SDL_GetError());
        return false;
    }

    m_window = std::make_shared<Window>();
    return m_window->create(app_name, pos_x, pos_y, width, height);
}

i32
Application::run()
{
    const auto platform_state = std::make_shared<yuki::Platform_State>();
    yuki::Platform::initialize_state(platform_state);

    initialize();

    f64 start_time = yuki::Platform::get_platform_time(platform_state);
    f64 next_game_tick = start_time;
    i32 loops = 0;

    i64 update_frames = 0;
    i64 render_frames = 0;
    f64 elapsed_time = 0.0;

    while (!m_should_quit) {
        PROFILE_SCOPE("Application::run update_loop");

        start_time = yuki::Platform::get_platform_time(platform_state);
        loops = 0;
        while (yuki::Platform::get_platform_time(platform_state) > next_game_tick && loops < max_skipped_frames) {
            SDL_Event event;
            while (SDL_PollEvent(&event) != 0) {
                switch (event.type) {
                    case SDL_WINDOWEVENT: {
                        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                            quit();
                        }
                    } break;

                    case SDL_QUIT: {
                        quit();
                    } break;

                    case SDL_KEYDOWN:
                    case SDL_KEYUP: {
                        m_input_manager.process_key(
                            static_cast<input::Key>(event.key.keysym.scancode), event.key.state == SDL_PRESSED, skip_update_ms
                        );
                    } break;

                    case SDL_MOUSEMOTION: {
                        m_input_manager.process_mouse_move(event.motion.x, event.motion.y, skip_update_ms);
                    } break;
                    case SDL_MOUSEWHEEL: {
                        m_input_manager.process_mouse_scroll(event.wheel.y, skip_update_ms);
                    } break;
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP: {
                        m_input_manager.process_mouse_button(
                            static_cast<input::Mouse_Button>(event.button.button),
                            event.button.state == SDL_PRESSED,
                            skip_update_ms
                        );
                    } break;
                }
            }

            update(skip_update_ms);

            next_game_tick += skip_update_ms;
            loops++;

            ++update_frames;
        }

        f32 interpolation = static_cast<f32>(
            (yuki::Platform::get_platform_time(platform_state) + skip_update_ms - next_game_tick) / skip_update_ms
        );

        render(interpolation);

        ++render_frames;

        elapsed_time += (yuki::Platform::get_platform_time(platform_state) - start_time);
        if (elapsed_time >= millisecond_per_second) {
            core::log::debug("Update fps: {}  Render fps: {}", update_frames, render_frames);
            elapsed_time = 0;
            update_frames = 0;
            render_frames = 0;
        }
    }

    return 0;
}

void
Application::quit()
{
    m_should_quit = true;
}

void
Application::initialize()
{
    on_initialize();
}

void
Application::update(f64 delta_time)
{
    PROFILE_FUNCTION();
    m_input_manager.clear_state();
    on_update(delta_time);
}

void
Application::render(f32 interpolation)
{
    PROFILE_FUNCTION();
    assert(m_window != nullptr);

    m_window->clear();
    on_render(interpolation);
    m_window->flip();
}
}
