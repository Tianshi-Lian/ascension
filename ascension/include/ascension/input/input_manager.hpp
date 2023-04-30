/**
 * File: input_manager.hpp
 * Project: ascension
 * File Created: 2023-04-30 15:30:08
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-30 19:07:30
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

#include "input/input_listener.hpp"

namespace ascension::input {

class Input_Manager {
  public:
    Input_Manager();
    ~Input_Manager();

    void clear_state();

    void register_mouse_listener(Mouse_Listener* mouse_listener);
    void register_keyboard_listener(Keyboard_Listener* keyboard_listener);

    void remove_mouse_listener(const Mouse_Listener* mouse_listener);
    void remove_keyboard_listener(const Keyboard_Listener* keyboard_listener);

    [[nodiscard]] const Mouse_State& get_current_mouse_state() const;
    [[nodiscard]] const Mouse_State& get_previous_mouse_state() const;
    [[nodiscard]] const Keyboard_State& get_current_keyboard_state() const;
    [[nodiscard]] const Keyboard_State& get_previous_keyboard_state() const;

    [[nodiscard]] bool is_key_down(Key key) const;
    [[nodiscard]] bool is_key_up(Key key) const;
    [[nodiscard]] bool was_key_down(Key key) const;
    [[nodiscard]] bool was_key_up(Key key) const;

    [[nodiscard]] bool is_mouse_button_down(Mouse_Button mouse_button) const;
    [[nodiscard]] bool is_mouse_button_up(Mouse_Button mouse_button) const;
    [[nodiscard]] bool was_mouse_button_down(Mouse_Button mouse_button) const;
    [[nodiscard]] bool was_mouse_button_up(Mouse_Button mouse_button) const;

    [[nodiscard]] v2 get_mouse_position() const;

    void process_key(Key key, bool is_down, f64 delta_time);
    void process_mouse_move(i32 new_x_position, i32 new_y_position, f64 delta_time);
    void process_mouse_scroll(i32 scroll_delta, f64 delta_time);
    void process_mouse_button(Mouse_Button button, bool is_down, f64 delta_time);

    Input_Manager(const Input_Manager&) = default;
    Input_Manager(Input_Manager&&) = delete;
    Input_Manager& operator=(const Input_Manager&) = default;
    Input_Manager& operator=(Input_Manager&&) = delete;

  private:
    Mouse_State m_current_mouse_state;
    Mouse_State m_previous_mouse_state;
    Keyboard_State m_current_keyboard_state;
    Keyboard_State m_previous_keyboard_state;

    std::vector<Mouse_Listener*> m_mouse_listeners;
    std::vector<Keyboard_Listener*> m_keyboard_listeners;
};
}
