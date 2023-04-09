/**
 * File: input.cpp
 * Project: yuki
 * File Created: 2023-04-04 18:37:03
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:07:10
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

#include "input/input.hpp"

namespace yuki::input {

std::array<bool, static_cast<size_t>(Key::MAX_KEYS)> Input::m_key_state{ false };
std::array<bool, static_cast<size_t>(Key::MAX_KEYS)> Input::m_previous_key_state{ false };
std::array<bool, static_cast<size_t>(Mouse_Button::MAX_BUTTONS)> Input::m_mouse_button_state{ false };
std::array<bool, static_cast<size_t>(Mouse_Button::MAX_BUTTONS)> Input::m_previous_mouse_button_state{ false };
i32 Input::m_mouse_x_position = 0;
i32 Input::m_mouse_y_position = 0;
i32 Input::m_mouse_previous_x_delta = 0;
i32 Input::m_mouse_previous_y_delta = 0;

void
Input::clear_state()
{
    m_previous_key_state = m_key_state;
    m_key_state = { false };

    m_previous_mouse_button_state = m_mouse_button_state;
    m_mouse_button_state = { false };

    m_mouse_previous_x_delta = 0;
    m_mouse_previous_y_delta = 0;
}

bool
Input::is_key_down(Key key)
{
    return m_key_state.at(static_cast<u32>(key));
}

bool
Input::was_key_down(Key key)
{
    return m_previous_key_state.at(static_cast<u32>(key));
}

bool
Input::was_key_up(Key key)
{
    return !m_previous_key_state.at(static_cast<u32>(key));
}

bool
Input::is_mouse_button_down(Mouse_Button button)
{
    return m_mouse_button_state.at(static_cast<u32>(button));
}

bool
Input::was_mouse_button_down(Mouse_Button button)
{
    return m_previous_mouse_button_state.at(static_cast<u32>(button));
}

bool
Input::was_mouse_button_up(Mouse_Button button)
{
    return !m_previous_mouse_button_state.at(static_cast<u32>(button));
}

i32
Input::get_mouse_x_position()
{
    return m_mouse_x_position;
}

i32
Input::get_mouse_y_position()
{
    return m_mouse_y_position;
}

i32
Input::get_mouse_x_delta()
{
    return m_mouse_previous_x_delta;
}

i32
Input::get_mouse_y_delta()
{
    return m_mouse_previous_y_delta;
}

void
Input::process_key(Key key, bool is_down)
{
    m_key_state.at(static_cast<u32>(key)) = is_down;
}

void
Input::process_mouse_move(i32 new_x_position, i32 new_y_position)
{
    m_mouse_previous_x_delta = new_x_position - m_mouse_x_position;
    m_mouse_x_position = new_x_position;

    m_mouse_previous_y_delta = new_y_position - m_mouse_y_position;
    m_mouse_y_position = new_y_position;
}

void
Input::process_mouse_scroll(i32 scroll_delta)
{
    // TODO: scroll wheel.
    (void)scroll_delta;
}

void
Input::process_mouse_button(Mouse_Button button, bool is_down)
{
    m_mouse_button_state.at(static_cast<u32>(button)) = is_down;
}

}
