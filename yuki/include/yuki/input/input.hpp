#pragma once

#include <array>

#include "yuki/input/input_types.hpp"

namespace yuki::platform {
class Platform;
}

namespace yuki::input {

class Input {
    friend class yuki::platform::Platform;

  public:
    static void clear_state();

    static bool is_key_down(Key key);
    static bool was_key_down(Key key);
    static bool was_key_up(Key key);

    static bool is_mouse_button_down(Mouse_Button button);
    static bool was_mouse_button_down(Mouse_Button button);
    static bool was_mouse_button_up(Mouse_Button button);

    static i32 get_mouse_x_position();
    static i32 get_mouse_y_position();

    static i32 get_mouse_x_delta();
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
