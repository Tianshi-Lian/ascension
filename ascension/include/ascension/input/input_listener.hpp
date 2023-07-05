/**
 * File: input_listener.hpp
 * Project: ascension
 * File Created: 2023-04-30 15:50:06
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-30 16:22:27
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

#include "input/input_state.hpp"

namespace ascension::input {

class Input_Listener {
public:
    Input_Listener();

    [[nodiscard]] u32 get_id() const;

    friend bool operator==(const Input_Listener& listener_1, const Input_Listener& listener_2);
    friend bool operator!=(const Input_Listener& listener_1, const Input_Listener& listener_2);

private:
    u32 m_id;
};

class Keyboard_Listener : public Input_Listener {
public:
    Keyboard_Listener() = default;
    virtual ~Keyboard_Listener() = 0;

    virtual void handle_key_press(Key key, const Keyboard_State& keyboard_state, f64 delta_time) = 0;
    virtual void handle_key_down(Key key, const Keyboard_State& keyboard_state, f64 delta_time) = 0;
    virtual void handle_key_up(Key key, const Keyboard_State& keyboard_state, f64 delta_time) = 0;

    Keyboard_Listener(const Keyboard_Listener&) = default;
    Keyboard_Listener(Keyboard_Listener&&) = delete;
    Keyboard_Listener& operator=(const Keyboard_Listener&) = default;
    Keyboard_Listener& operator=(Keyboard_Listener&&) = delete;
};

class Mouse_Listener : public Input_Listener {
public:
    Mouse_Listener() = default;
    virtual ~Mouse_Listener() = 0;

    virtual void handle_mouse_move(i32 delta_x, i32 delta_y, const Mouse_State& mouse_state, f64 delta_time) = 0;
    virtual void handle_mouse_scroll(i32 delta, const Mouse_State& mouse_state, f64 delta_time) = 0;
    virtual void handle_button_press(Mouse_Button button, const Mouse_State& mouse_state, f64 delta_time) = 0;
    virtual void handle_button_down(Mouse_Button button, const Mouse_State& mouse_state, f64 delta_time) = 0;
    virtual void handle_button_up(Mouse_Button button, const Mouse_State& mouse_state, f64 delta_time) = 0;

    Mouse_Listener(const Mouse_Listener&) = default;
    Mouse_Listener(Mouse_Listener&&) = delete;
    Mouse_Listener& operator=(const Mouse_Listener&) = default;
    Mouse_Listener& operator=(Mouse_Listener&&) = delete;
};

}
