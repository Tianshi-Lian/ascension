/**
 * File: input.hpp
 * Project: yuki
 * File Created: 2023-04-04 18:37:03
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:05:06
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

#include "yuki/input/input_types.hpp"

namespace yuki::platform {
class Platform;
}

namespace yuki::input {

/**
 * @class Input
 *
 * @brief Class which handles basic input state.
 */
class Input {
    friend class yuki::platform::Platform;

  public:
    /**
     * @brief Clears the current state, assigning it to the previous state.
     */
    static void clear_state();

    /**
     * @brief Check if a key is currently down.
     *
     * @param     key   the key to check
     *
     * @return    true is returned if the specified key is currently pressed down
     */
    static bool is_key_down(Key key);
    /**
     * @brief Check if a key was previously down.
     *
     * @param     key   the key to check
     *
     * @return    true is returned if the specified key was previously pressed down
     */
    static bool was_key_down(Key key);
    /**
     * @brief Check if a key was previously up.
     *
     * @param     key   the key to check
     *
     * @return    true is returned if the specified key was previously not pressed down
     */
    static bool was_key_up(Key key);

    /**
     * @brief Check if a mouse button is currently down.
     *
     * @param     button   the mouse button to check
     *
     * @return    true is returned if the specified mouse button is currently pressed down
     */
    static bool is_mouse_button_down(Mouse_Button button);
    /**
     * @brief Check if a mouse button was previously down.
     *
     * @param     button   the mouse button to check
     *
     * @return    true is returned if the specified mouse button was previously pressed down
     */
    static bool was_mouse_button_down(Mouse_Button button);
    /**
     * @brief Check if a mouse button was previously up.
     *
     * @param     button   the mouse button to check
     *
     * @return    true is returned if the specified mouse button was previously not pressed down
     */
    static bool was_mouse_button_up(Mouse_Button button);

    /**
     * @brief Get the current window-relative x position of the mouse cursor.
     *
     * @return    i32 specifying the current x position
     */
    static i32 get_mouse_x_position();
    /**
     * @brief Get the current window-relative y position of the mouse cursor.
     *
     * @return    i32 specifying the current y position
     */
    static i32 get_mouse_y_position();

    /**
     * @brief Get the previous window-relative x movement of the mouse cursor.
     *
     * @return    i32 specifying the difference in x position between now and the previous clear.
     */
    static i32 get_mouse_x_delta();
    /**
     * @brief Get the previous window-relative y movement of the mouse cursor.
     *
     * @return    i32 specifying the difference in y position between now and the previous clear.
     */
    static i32 get_mouse_y_delta();

    static void process_key(Key key, bool is_down);
    static void process_mouse_move(i32 new_x_position, i32 new_y_position);
    static void process_mouse_scroll(i32 scroll_delta);
    static void process_mouse_button(Mouse_Button button, bool is_down);

  private:
    static std::array<bool, static_cast<size_t>(Key::MAX_KEYS)> m_key_state;
    static std::array<bool, static_cast<size_t>(Key::MAX_KEYS)> m_previous_key_state;
    static std::array<bool, static_cast<size_t>(Mouse_Button::MAX_BUTTONS)> m_mouse_button_state;
    static std::array<bool, static_cast<size_t>(Mouse_Button::MAX_BUTTONS)> m_previous_mouse_button_state;
    static i32 m_mouse_x_position;
    static i32 m_mouse_y_position;
    static i32 m_mouse_previous_x_delta;
    static i32 m_mouse_previous_y_delta;
};

}
