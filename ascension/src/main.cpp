/**
 * File: main.cpp
 * Project: ascension
 * File Created: 2023-04-06 21:17:10
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:03:20
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

#define UNUSED_PARAM(a) (void)a

constexpr glm::vec2 Default_Window_Position{ 200, 200 };
constexpr glm::vec2 Default_Window_Size{ 1600, 900 };

int
main(int argc, char** argv)
{
    using namespace ascension;

    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);

    core::Application app;
    app.setup("Ascension", Default_Window_Position, Default_Window_Size);
    app.run();

    return 0;
}
