/**
 * File: input_state.cpp
 * Project: ascension
 * File Created: 2023-04-30 15:56:01
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-30 15:59:09
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

#include "input/input_state.hpp"

namespace ascension::input {

bool
Mouse_State::is_down(Mouse_Button button) const
{
    return buttons_down.at(static_cast<size_t>(button));
}

bool
Mouse_State::is_up(Mouse_Button button) const
{
    return !buttons_down.at(static_cast<size_t>(button));
}

bool
Keyboard_State::is_down(Key key) const
{
    return keys_down.at(static_cast<size_t>(key));
}

bool
Keyboard_State::is_up(Key key) const
{
    return !keys_down.at(static_cast<size_t>(key));
}

}
