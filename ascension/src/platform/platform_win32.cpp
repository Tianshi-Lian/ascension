/**
 * File: platform_win32.cpp
 * Project: ascension
 * File Created: 2023-03-12 21:20:07
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-10-14 16:24:22
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

#include "platform/platform.hpp"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "debug/instrumentor.hpp"
#include "debug/logger.hpp"
#include "input/input.hpp"

namespace {

constexpr i64 ms_per_second = 1000; // milliseconds

struct Internal_State {
    HINSTANCE instance{ nullptr };
    HWND window_handle{ nullptr };

    f64 clock_frequency{ 0.0 };
    f64 start_time{ 0.0 };
};

LRESULT CALLBACK
win32_process_messages(HWND window_handle, u32 message, WPARAM w_param, LPARAM l_param)
{
    switch (message) {
        case WM_ERASEBKGND:
            // Notify windows that erasing will be handled by the application.
            // This is to prevent flickering with our windows.
            return 1;
        case WM_CLOSE:
            // TODO: Fire an event to quit the application.
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            RECT client_rect{};
            GetClientRect(window_handle, &client_rect);
            [[maybe_unused]] u32 width = static_cast<u32>(client_rect.right - client_rect.left);
            [[maybe_unused]] u32 height = static_cast<u32>(client_rect.bottom - client_rect.top);
            // TODO: Fire an event to resize the window.
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            const bool pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            auto key = static_cast<ascension::input::Key>(static_cast<u32>(w_param));

            const bool is_extended = (HIWORD(l_param) & KF_EXTENDED) == KF_EXTENDED;

            // Keypress only determines if _any_ alt/ctrl/shift key is pressed. Determine which one if so.
            if (w_param == VK_MENU) {
                key = is_extended ? ascension::input::Key::RALT : ascension::input::Key::LALT;
            }
            else if (w_param == VK_SHIFT) {
                // Annoyingly, KF_EXTENDED is not set for shift keys.
                u32 left_shift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
                u32 scancode = ((l_param & (0xFF << 16)) >> 16);
                key = scancode == left_shift ? ascension::input::Key::LSHIFT : ascension::input::Key::RSHIFT;
            }
            else if (w_param == VK_CONTROL) {
                key = is_extended ? ascension::input::Key::RCONTROL : ascension::input::Key::LCONTROL;
            }

            ascension::input::Input::process_key(key, pressed);

            // Return 0 to prevent default window behaviour for some keypresses, such as alt.
            return 0;
        } break;
        case WM_MOUSEMOVE: {
            i32 x_position = GET_X_LPARAM(l_param);
            i32 y_position = GET_Y_LPARAM(l_param);

            ascension::input::Input::process_mouse_move(x_position, y_position);
        } break;
        case WM_MOUSEHWHEEL: {
            i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0) {
                // Flatten the input to an OS-independent (-1, 1)
                z_delta = (z_delta < 0) ? -1 : 1;
                ascension::input::Input::process_mouse_scroll(z_delta);
            }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            bool pressed = (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN);
            ascension::input::Mouse_Button button = ascension::input::Mouse_Button::MAX_BUTTONS;
            switch (message) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    button = ascension::input::Mouse_Button::LEFT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    button = ascension::input::Mouse_Button::MIDDLE;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    button = ascension::input::Mouse_Button::RIGHT;
                    break;
                default:
                    break;
            }

            // Pass over to the input subsystem.
            if (button != ascension::input::Mouse_Button::MAX_BUTTONS) {
                ascension::input::Input::process_mouse_button(button, pressed);
            }
        } break;
        default:
            return DefWindowProc(window_handle, message, w_param, l_param);
    }

    return DefWindowProc(window_handle, message, w_param, l_param);
}

}

namespace ascension::platform {

bool
Platform::initialize(
    const std::shared_ptr<Platform_State>& platform_state,
    const std::string& application_name,
    u32 pos_x,
    u32 pos_y,
    u32 width,
    u32 height
)
{
    PROFILE_FUNCTION();
    ascension::debug::Logger::info("ascension", "Initializing Windows platform layer...");

    if (platform_state->internal_state == nullptr) {
        initialize_state(platform_state);
    }

    const auto& state{ std::static_pointer_cast<Internal_State>(platform_state->internal_state) };

    state->instance = GetModuleHandleA(nullptr);

    const std::string wnd_class_name = "ascension_platform_win32_wnd_class";

    WNDCLASSEX wnd_class_ex = {};
    wnd_class_ex.cbSize = sizeof(WNDCLASSEX);
    wnd_class_ex.style = CS_HREDRAW | CS_VREDRAW;
    wnd_class_ex.lpfnWndProc = win32_process_messages;
    wnd_class_ex.hInstance = state->instance;
    wnd_class_ex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd_class_ex.hbrBackground = nullptr; // (HBRUSH) (COLOR_WINDOW + 1);
    wnd_class_ex.lpszClassName = wnd_class_name.c_str();

    if (RegisterClassEx(&wnd_class_ex) == 0) {
        MessageBox(nullptr, "Window registration failed.", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // Window size will include the border sizes.
    // Windows using ints but we use uint to guard against silly window sizing.
    i32 window_x{ static_cast<i32>(pos_x) };
    i32 window_y{ static_cast<i32>(pos_y) };
    i32 window_width{ static_cast<i32>(width) };
    i32 window_height{ static_cast<i32>(height) };

    const i32 window_style{ WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME };
    const i32 window_ex_style{ WS_EX_APPWINDOW };

    // Obtain the size of the border for offsetting actual window size & position.
    RECT border_rect{ 0, 0, 0, 0 };
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowEx(
        window_ex_style,
        wnd_class_name.c_str(),
        application_name.c_str(),
        window_style,
        window_x,
        window_y,
        window_width,
        window_height,
        nullptr,
        nullptr,
        state->instance,
        nullptr
    );

    if (handle == nullptr) {
        MessageBox(nullptr, "Window creation failed.", "Error", MB_ICONEXCLAMATION | MB_OK);

        ascension::debug::Logger::critical("ascension", "Platform::initialize() failed for win32");
        return false;
    }
    state->window_handle = handle;

    ascension::debug::Logger::notice("ascension", "Windows platform layer initialized.");

    return true;
}

void
Platform::initialize_state(const std::shared_ptr<Platform_State>& platform_state)
{
    platform_state->internal_state = std::make_shared<Internal_State>();
}

void
Platform::shutdown(const std::shared_ptr<Platform_State>& platform_state)
{
    const auto& state{ std::static_pointer_cast<Internal_State>(platform_state->internal_state) };

    if (state->window_handle != nullptr) {
        DestroyWindow(state->window_handle);
        state->window_handle = nullptr;
    }
}

bool
Platform::process_messages(const std::shared_ptr<Platform_State>& /*platform_state*/)
{
    ascension::input::Input::clear_state();

    MSG message;
    while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return true;
}

void
setup_win32_clock(const std::shared_ptr<Internal_State>& state)
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    state->clock_frequency = 1.0 / static_cast<f64>(frequency.QuadPart);

    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    now_time.QuadPart *= ms_per_second;
    const auto time_ms = static_cast<f64>(now_time.QuadPart) * state->clock_frequency;
    state->start_time = time_ms;
}

f64
Platform::get_time(const std::shared_ptr<Platform_State>& platform_state)
{
    const auto& state{ std::static_pointer_cast<Internal_State>(platform_state->internal_state) };

    if (state->clock_frequency == 0) {
        setup_win32_clock(state);
    }

    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    now_time.QuadPart *= ms_per_second;
    const auto time_ms = static_cast<f64>(now_time.QuadPart) * state->clock_frequency;
    return time_ms;
}

void
Platform::sleep(u32 milliseconds)
{
    Sleep(milliseconds);
}

Library_Handle
Platform::load_shared_library(const std::string& filepath)
{
    Library_Handle result;
    result.internal_state = nullptr;

    auto* const h_instance{ LoadLibrary(TEXT(filepath.c_str())) };
    if (h_instance == nullptr) {
        const auto err = GetLastError();
        ascension::debug::Logger::error("ascension", "Failed to load handle to library {} {}", filepath, err);
        return result;
    }

    const auto state{ std::make_shared<HINSTANCE>(h_instance) };
    result.internal_state = std::static_pointer_cast<void>(state);
    result.filepath = filepath;

    return result;
}

void
Platform::free_shared_library(const Library_Handle& library_handle)
{
    const auto library{ std::static_pointer_cast<HINSTANCE>(library_handle.internal_state) };
    FreeLibrary(*library);
}

std::shared_ptr<void (*)()>
Platform::load_library_function_internal(const Library_Handle& library_handle, const std::string& function_name)
{
    if (function_name.empty()) {
        ascension::debug::Logger::error("ascension", "Attempting to load unnamed function from library handle", function_name);
        return nullptr;
    }

    if (library_handle.internal_state == nullptr) {
        ascension::debug::Logger::error("ascension", "Trying to load function {} from null library handle", function_name);
        return nullptr;
    }

    const auto library{ std::static_pointer_cast<HINSTANCE>(library_handle.internal_state) };

    const auto proc_address{ GetProcAddress(*library, function_name.c_str()) };
    if (proc_address == nullptr) {
        ascension::debug::Logger::error(
            "ascension", "Failed to load handle to function {} from library {}", function_name, library_handle.filepath
        );
        return nullptr;
    }

    const auto function{ std::make_shared<FARPROC>(proc_address) };

    return std::reinterpret_pointer_cast<void (*)()>(function);
}

}

#endif
