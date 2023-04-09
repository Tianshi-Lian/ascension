/**
 * File: engine.cpp
 * Project: erika
 * File Created: 2023-03-03 18:35:33
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:04:30
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

#include "core/engine.hpp"

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include "yuki/input/input.hpp"
#include "yuki/platform/platform.hpp"

#include "core/game.hpp"

namespace erika {

void
Engine::initialize()
{
    PROFILE_FUNCTION();
    yuki::debug::Logger::info("erika", "Initializing game engine...");

    yuki::debug::Logger::notice("erika", "Initialized game engine.");
}

void
Engine::run(const std::shared_ptr<yuki::Platform_State>& platform_state, const std::shared_ptr<Game>& game)
{
    using yuki::Platform;

    PROFILE_FUNCTION();
    if (!game) {
        yuki::debug::Logger::critical("erika", "Engine::run() failed to run: invalid game!");
    }
    yuki::debug::Logger::debug("erika", "Engine::run() starting game {}", game->get_window_title());

    game->set_platform_state(platform_state);
    game->initialize();

    constexpr f64 millisecond = 1000.0;
    constexpr int updates_per_second = 60;
    constexpr f64 skip_update_ms = millisecond / updates_per_second;
    constexpr int max_skipped_frames = 5;

    f64 start_time = Platform::get_platform_time(platform_state);
    f64 next_game_tick = start_time;
    int loops = 0;
    float interpolation = 0.0f;

    i64 update_frames = 0;
    i64 render_frames = 0;
    f64 elapsed_time = 0.0;

    bool game_is_running = true;
    while (game_is_running) {
        start_time = Platform::get_platform_time(platform_state);
        loops = 0;
        while (Platform::get_platform_time(platform_state) > next_game_tick && loops < max_skipped_frames) {
            Platform::process_messages(platform_state);
            game->update(static_cast<f32>(skip_update_ms));

            if (yuki::input::Input::was_key_down(yuki::input::Key::ESCAPE)) {
                game_is_running = false;
            }

            next_game_tick += skip_update_ms;
            loops++;

            ++update_frames;
        }

        interpolation = static_cast<float>(Platform::get_platform_time(platform_state) + skip_update_ms - next_game_tick) /
                        static_cast<float>(skip_update_ms);
        game->render(interpolation);
        ++render_frames;

        elapsed_time += (Platform::get_platform_time(platform_state) - start_time);
        if (elapsed_time >= millisecond) {
            yuki::debug::Logger::debug("erika", "Update fps: {}  Render fps: {}", update_frames, render_frames);
            elapsed_time = 0;
            update_frames = 0;
            render_frames = 0;
        }
    }
    yuki::Platform::shutdown(platform_state);
}

}
