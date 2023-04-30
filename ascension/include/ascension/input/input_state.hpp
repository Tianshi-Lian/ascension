/**
 * File: input_state.hpp
 * Project: ascension
 * File Created: 2023-04-30 15:30:46
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-30 15:57:57
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

#include <array>

#include "input/input_types.hpp"

namespace ascension::input {

struct Mouse_State {
    v2 position;
    std::array<bool, static_cast<size_t>(Mouse_Button::MAX_BUTTONS)> buttons_down;

    [[nodiscard]] bool is_down(Mouse_Button button) const;
    [[nodiscard]] bool is_up(Mouse_Button button) const;
};

struct Keyboard_State {
    std::array<bool, static_cast<size_t>(Key::MAX_KEYS)> keys_down;

    [[nodiscard]] bool is_down(Key key) const;
    [[nodiscard]] bool is_up(Key key) const;
};

}
