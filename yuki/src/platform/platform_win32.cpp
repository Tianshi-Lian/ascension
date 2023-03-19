#include "platform/platform.hpp"

#ifdef _WIN32

#define NOGDI

#include <windows.h>
#include <windowsx.h>

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

namespace {

struct Internal_State {
    HINSTANCE h_instance;
    HWND window_handle;
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
            [[maybe_unused]] bool pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            // TODO: Process input.
        } break;
        case WM_MOUSEMOVE: {
            [[maybe_unused]] i32 mouse_x = GET_X_LPARAM(l_param);
            [[maybe_unused]] i32 mouse_y = GET_Y_LPARAM(l_param);
            // TODO: Process input.
        } break;
        case WM_MOUSEHWHEEL: {
            [[maybe_unused]] i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0) {
                z_delta = (z_delta < 0) ? -1 : 1;
                // TODO: Process input.
            }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            [[maybe_unused]] bool pressed =
                (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN);
            // TODO: Process input.
        } break;
        default:
            return DefWindowProc(window_handle, message, w_param, l_param);
    }

    return DefWindowProc(window_handle, message, w_param, l_param);
}

}

namespace yuki::platform {

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
    yuki::debug::Logger::info("yuki > Initializing Windows platform layer...");

    platform_state->internal_state = std::make_shared<Internal_State>();
    const auto& state{ std::static_pointer_cast<Internal_State>(platform_state->internal_state) };

    state->h_instance = GetModuleHandleA(nullptr);

    const std::string wnd_class_name = "yuki_platform_win32_wnd_class";

    auto* const icon{ LoadIcon(state->h_instance, IDI_APPLICATION) };
    WNDCLASS wnd_class{};
    wnd_class.style = CS_DBLCLKS;
    wnd_class.lpfnWndProc = win32_process_messages;
    wnd_class.hInstance = state->h_instance;
    wnd_class.hIcon = icon;
    wnd_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    wnd_class.hbrBackground = nullptr;
    wnd_class.lpszClassName = wnd_class_name.c_str();

    if (!RegisterClass(&wnd_class)) {
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
        state->h_instance,
        nullptr
    );

    if (handle == nullptr) {
        MessageBox(nullptr, "Window creation failed.", "Error", MB_ICONEXCLAMATION | MB_OK);

        yuki::debug::Logger::critical("yuki > Platform::initialize() failed for win32");
        return false;
    }
    state->window_handle = handle;

    ShowWindow(state->window_handle, SW_SHOW);

    yuki::debug::Logger::notice("yuki > Windows platform layer initialized.");

    return true;
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
    MSG message;
    while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return true;
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
        yuki::debug::Logger::error("Failed to load handle to library %s", filepath.c_str());
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
        yuki::debug::Logger::error("Attempting to load unnamed function from library handle", function_name.c_str());
        return nullptr;
    }

    if (library_handle.internal_state == nullptr) {
        yuki::debug::Logger::error("Trying to load function %s from null library handle", function_name.c_str());
        return nullptr;
    }

    const auto library{ std::static_pointer_cast<HINSTANCE>(library_handle.internal_state) };

    const auto proc_address{ GetProcAddress(*library, function_name.c_str()) };
    if (proc_address == nullptr) {
        yuki::debug::Logger::error(
            "Failed to load handle to function %s from library %s", function_name.c_str(), library_handle.filepath.c_str()
        );
        return nullptr;
    }

    const auto function{ std::make_shared<FARPROC>(proc_address) };

    return std::reinterpret_pointer_cast<void (*)()>(function);
}
}

#endif
