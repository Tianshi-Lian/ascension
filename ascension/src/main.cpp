/**
 * File: main.cpp
 * Project: ascension
 * File Created: 2023-04-06 21:17:10
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-08 15:54:52
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

#include <SDL.h>

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include "ascension.hpp"

#define UNUSED_PARAM(a) (void)a

constexpr u32 WIN_DEFAULT_X = 200;
constexpr u32 WIN_DEFAULT_Y = 200;
constexpr u32 WIN_DEFAULT_WIDTH = 1600;
constexpr u32 WIN_DEFAULT_HEIGHT = 900;

int
main(i32 argc, char** argv)
{
    using namespace ascension;

    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);

    yuki::debug::Logger::initialize("logs/app.log", yuki::debug::Severity::LOG_DEBUG, true, true);
    PROFILE_BEGIN_SESSION("ascension", "logs/timings.json");

    Ascension game;
    if (game.initialize("Ascension", WIN_DEFAULT_X, WIN_DEFAULT_Y, WIN_DEFAULT_WIDTH, WIN_DEFAULT_HEIGHT)) {
        const auto result = game.run();

        PROFILE_END_SESSION();
        return result;
    }

    PROFILE_END_SESSION();
    return 1;
}
