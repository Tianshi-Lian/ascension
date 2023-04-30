/**
 * File: input_manager.cpp
 * Project: ascension
 * File Created: 2023-04-30 16:13:02
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-30 19:08:02
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

#include "input/input_manager.hpp"

#include <algorithm>

#include "core/log.hpp"

namespace ascension::input {

Input_Manager::Input_Manager()
  : m_current_mouse_state{}
  , m_previous_mouse_state{}
  , m_current_keyboard_state{}
  , m_previous_keyboard_state{}
{
}

Input_Manager::~Input_Manager()
{
    m_mouse_listeners.clear();
    m_keyboard_listeners.clear();
}

void
Input_Manager::clear_state()
{
    m_previous_mouse_state = m_current_mouse_state;
    m_previous_keyboard_state = m_current_keyboard_state;
}

void
Input_Manager::register_mouse_listener(Mouse_Listener* mouse_listener)
{
    const auto listener = std::find(m_mouse_listeners.begin(), m_mouse_listeners.end(), mouse_listener);
    if (listener != m_mouse_listeners.end()) {
        core::log::warn("Attempting to register mouse listener multiple times.");
        return;
    }
    m_mouse_listeners.push_back(mouse_listener);
}

void
Input_Manager::register_keyboard_listener(Keyboard_Listener* keyboard_listener)
{
    const auto listener = std::find(m_keyboard_listeners.begin(), m_keyboard_listeners.end(), keyboard_listener);
    if (listener != m_keyboard_listeners.end()) {
        core::log::warn("Attempting to register keyboard listener multiple times.");
        return;
    }
    m_keyboard_listeners.push_back(keyboard_listener);
}

void
Input_Manager::remove_mouse_listener(const Mouse_Listener* mouse_listener)
{
    const auto listener = std::find(m_mouse_listeners.begin(), m_mouse_listeners.end(), mouse_listener);
    if (listener != m_mouse_listeners.end()) {
        m_mouse_listeners.erase(listener);
    }
}

void
Input_Manager::remove_keyboard_listener(const Keyboard_Listener* keyboard_listener)
{
    const auto listener = std::find(m_keyboard_listeners.begin(), m_keyboard_listeners.end(), keyboard_listener);
    if (listener != m_keyboard_listeners.end()) {
        m_keyboard_listeners.erase(listener);
    }
}

const Mouse_State&
Input_Manager::get_current_mouse_state() const
{
    return m_current_mouse_state;
}

const Mouse_State&
Input_Manager::get_previous_mouse_state() const
{
    return m_previous_mouse_state;
}

const Keyboard_State&
Input_Manager::get_current_keyboard_state() const
{
    return m_current_keyboard_state;
}

const Keyboard_State&
Input_Manager::get_previous_keyboard_state() const
{
    return m_previous_keyboard_state;
}

bool
Input_Manager::is_key_down(Key key) const
{
    return m_current_keyboard_state.is_down(key);
}

bool
Input_Manager::is_key_up(Key key) const
{
    return m_current_keyboard_state.is_up(key);
}

bool
Input_Manager::was_key_down(Key key) const
{
    return m_previous_keyboard_state.is_down(key);
}

bool
Input_Manager::was_key_up(Key key) const
{
    return m_previous_keyboard_state.is_up(key);
}

bool
Input_Manager::is_mouse_button_down(Mouse_Button mouse_button) const
{
    return m_current_mouse_state.is_down(mouse_button);
}

bool
Input_Manager::is_mouse_button_up(Mouse_Button mouse_button) const
{
    return m_current_mouse_state.is_up(mouse_button);
}

bool
Input_Manager::was_mouse_button_down(Mouse_Button mouse_button) const
{
    return m_previous_mouse_state.is_down(mouse_button);
}
bool
Input_Manager::was_mouse_button_up(Mouse_Button mouse_button) const
{
    return m_previous_mouse_state.is_up(mouse_button);
}

v2
Input_Manager::get_mouse_position() const
{
    return m_current_mouse_state.position;
}

void
Input_Manager::process_key(Key key, bool is_down, f64 delta_time)
{
    m_current_keyboard_state.keys_down.at(static_cast<size_t>(key)) = is_down;

    for (auto& listener : m_keyboard_listeners) {
        if (is_down) {
            listener->handle_key_down(key, m_current_keyboard_state, delta_time);
        }
        else {
            if (m_previous_keyboard_state.is_down(key)) {
                listener->handle_key_press(key, m_current_keyboard_state, delta_time);
            }
            else {
                listener->handle_key_up(key, m_current_keyboard_state, delta_time);
            }
        }
    }
}

void
Input_Manager::process_mouse_move(i32 new_x_position, i32 new_y_position, f64 delta_time)
{
    const i32 delta_x = new_x_position - m_current_mouse_state.position.x;
    const i32 delta_y = new_y_position - m_current_mouse_state.position.y;

    m_current_mouse_state.position.x = new_x_position;
    m_current_mouse_state.position.y = new_y_position;

    for (auto& listener : m_mouse_listeners) {
        listener->handle_mouse_move(delta_x, delta_y, m_current_mouse_state, delta_time);
    }
}

void
Input_Manager::process_mouse_scroll(i32 scroll_delta, f64 delta_time)
{
    for (auto& listener : m_mouse_listeners) {
        listener->handle_mouse_scroll(scroll_delta, m_current_mouse_state, delta_time);
    }
}

void
Input_Manager::process_mouse_button(Mouse_Button button, bool is_down, f64 delta_time)
{
    m_current_mouse_state.buttons_down.at(static_cast<size_t>(button)) = is_down;

    for (auto& listener : m_mouse_listeners) {
        if (is_down) {
            listener->handle_button_down(button, m_current_mouse_state, delta_time);
        }
        else {
            if (m_previous_mouse_state.is_down(button)) {
                listener->handle_button_press(button, m_current_mouse_state, delta_time);
            }
            else {
                listener->handle_button_up(button, m_current_mouse_state, delta_time);
            }
        }
    }
}

}
