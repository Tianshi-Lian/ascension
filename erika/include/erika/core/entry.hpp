/**
 * File: entry.hpp
 * Project: erika
 * File Created: 2023-03-03 18:54:10
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-26 15:20:01
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

#include "erika/core/engine.hpp"
#include "erika/core/game.hpp"

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include "yuki/platform/platform.hpp"

int
main(i32 /*argc*/, char** /*args*/) // NOLINT
{
    yuki::debug::Logger::initialize("logs/app.log", yuki::debug::Severity::LOG_DEBUG, true, true);
    PROFILE_BEGIN_SESSION("erika_profiling", "logs/timings.json");

    erika::Engine engine;
    engine.initialize();

    auto game = erika::game::create();

    constexpr static u32 default_screen_pos = 128;
    constexpr static u32 default_screen_width = 1600;
    constexpr static u32 default_screen_height = 900;

    const auto state = std::make_shared<yuki::Platform_State>();
    if (!yuki::Platform::initialize(
            state, game->get_window_title(), default_screen_pos, default_screen_pos, default_screen_width, default_screen_height
        )) {

        yuki::debug::Logger::critical("Failed to initialize platform layer for game %s", game->get_window_title());
        return 1;
    }

    engine.run(state, game);

    PROFILE_END_SESSION();
    return 0;
}
