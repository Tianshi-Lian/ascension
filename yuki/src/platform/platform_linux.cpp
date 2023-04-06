#include "platform/platform.hpp"

#ifdef __linux__

#include <X11/Xlib-xcb.h>
#include <dlfcn.h>
#include <math.h>
#include <time.h>

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

namespace {

struct Internal_State {
    Display* display;
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
};

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
    yuki::debug::Logger::info("yuki", "Initializing Windows platform layer...");

    platform_state->internal_state = std::make_shared<Internal_State>();
    const auto& state{ std::static_pointer_cast<Internal_State>(platform_state->internal_state) };

    state->display = XOpenDisplay(nullptr);
    XAutoRepeatOff(state->display);

    int screen_p = 0;
    state->connection = XGetXCBConnection(state->display);

    if (xcb_connection_has_error(state->connection)) {
        yuki::debug::Logger::critical("yuki", "Failed to connect to X server via XCB.");
        return false;
    }

    const xcb_setup_t* setup = xcb_get_setup(state->connection);

    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    for (i32 screen = screen_p; screen > 0; --screen) {
        xcb_screen_next(&it);
    }

    state->screen = it.data;
    state->window = xcb_generate_id(state->connection);

    // Register events to listen to
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS |
                       XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    u32 value_list[] = { state->screen->black_pixel, event_values };

    xcb_create_window(
        state->connection,
        XCB_COPY_FROM_PARENT,
        state->window,
        state->screen->root,
        static_cast<i16>(pos_x),
        static_cast<i16>(pos_y),
        static_cast<u16>(width),
        static_cast<u16>(height),
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        state->screen->root_visual,
        event_mask,
        value_list
    );

    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        static_cast<u32>(application_name.size()),
        application_name.c_str()
    );

    // Tell the server to notify when the window manager attempts to destroy the window
    static const std::string wm_delete_window{ "WM_DELETE_WINDOW" };
    static const std::string wm_protocols{ "WM_PROTOCOLS" };
    xcb_intern_atom_cookie_t wm_delete_cookie =
        xcb_intern_atom(state->connection, 0, static_cast<u16>(wm_delete_window.size()), wm_delete_window.c_str());
    xcb_intern_atom_cookie_t wm_protocols_cookie =
        xcb_intern_atom(state->connection, 0, static_cast<u16>(wm_protocols.size()), wm_protocols.c_str());
    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(state->connection, wm_delete_cookie, NULL);
    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(state->connection, wm_protocols_cookie, NULL);
    state->wm_delete_win = wm_delete_reply->atom;
    state->wm_protocols = wm_protocols_reply->atom;

    xcb_change_property(
        state->connection, XCB_PROP_MODE_REPLACE, state->window, wm_protocols_reply->atom, 4, 32, 1, &wm_delete_reply->atom
    );

    // Map the window to the screen
    xcb_map_window(state->connection, state->window);

    // Flush the stream
    i32 stream_result = xcb_flush(state->connection);
    if (stream_result <= 0) {
        yuki::debug::Logger::critical("yuki", "An error occurred when flushing the stream: {}", stream_result);
        return false;
    }

    yuki::debug::Logger::notice("yuki", "Windows platform layer initialized.");

    return true;
}

void
Platform::shutdown(const std::shared_ptr<Platform_State>& platform_state)
{
    const auto& state{ std::static_pointer_cast<Internal_State>(platform_state->internal_state) };

    XAutoRepeatOn(state->display);

    xcb_destroy_window(state->connection, state->window);
}

bool
Platform::process_messages(const std::shared_ptr<Platform_State>& platform_state)
{
    const auto& state{ std::static_pointer_cast<Internal_State>(platform_state->internal_state) };

    xcb_generic_event_t* event;
    xcb_client_message_event_t* client_message;

    bool quit_flagged = false;

    // Poll for events until null is returned.
    do {
        event = xcb_poll_for_event(state->connection);
        if (event == 0) {
            break;
        }

        // Input events
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                // TODO: Process input.
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                // TODO: Process input.
            } break;
            case XCB_MOTION_NOTIFY:
                // TODO: Process input.
                break;

            case XCB_CONFIGURE_NOTIFY: {
                // TODO: Fire an event to resize the window.
            } break;

            case XCB_CLIENT_MESSAGE: {
                client_message = reinterpret_cast<xcb_client_message_event_t*>(event);

                // Window close
                if (client_message->data.data32[0] == state->wm_delete_win) {
                    quit_flagged = true;
                }
            } break;
            default:
                // Something else
                break;
        }

        free(event);
    } while (event != 0);

    return !quit_flagged;
}

f64
Platform::get_platform_time(const std::shared_ptr<Platform_State>& platform_state)
{
    (void)platform_state;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    return static_cast<f64>(now.tv_nsec) / 1.0e6;
}

void
Platform::sleep(u32 milliseconds)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
#else
    if (milliseconds >= 1000) {
        sleep(milliseconds / 1000);
    }
    usleep((milliseconds % 1000) * 1000);
#endif
}

Library_Handle
Platform::load_shared_library(const std::string& filepath)
{
    Library_Handle result;
    result.internal_state = nullptr;

    auto* const handle = dlopen(filepath.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        yuki::debug::Logger::error("yuki", "Failed to load handle to library {}", filepath);
        return result;
    }

    dlerror();

    result.internal_state = std::make_shared<void*>(handle);
    result.filepath = filepath;

    return result;
}

void
Platform::free_shared_library(const Library_Handle& library_handle)
{
    dlclose(library_handle.internal_state.get());
}

std::shared_ptr<void (*)()>
Platform::load_library_function_internal(const Library_Handle& library_handle, const std::string& function_name)
{
    if (function_name.empty()) {
        yuki::debug::Logger::error("yuki", "Attempting to load unnamed function from library handle");
        return nullptr;
    }

    if (library_handle.internal_state == nullptr) {
        yuki::debug::Logger::error("yuki", "Trying to load function {} from null library handle", function_name);
        return nullptr;
    }

    const auto library{ library_handle.internal_state };

    const auto proc_address{ dlsym(library.get(), function_name.c_str()) };
    if (proc_address == nullptr) {
        yuki::debug::Logger::error(
            "yuki", "Failed to load handle to function {} from library {}", function_name, library_handle.filepath
        );
        return nullptr;
    }

    const auto function{ std::make_shared<void*>(proc_address) };

    return std::reinterpret_pointer_cast<void (*)()>(function);
}

}

#endif
