#include "opengl_platform.hpp"

#ifdef __linux__

#include <glad/gl.h>

#include <GL/glx.h>

#include <X11/Xlib-xcb.h>

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include "yuki/platform/platform_types.hpp"

namespace {
struct Platform_Internal_State {
    Display* display;
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
};

struct OpenGL_Internal_State {
    Display* display;
    GLXContext context;
    GLXDrawable drawable;
    GLXWindow window;
};

}

namespace erika::plugins::renderer::opengl {

bool
OpenGL_Platform::create_context(
    const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state,
    const std::shared_ptr<yuki::platform::Platform_State>& app_platform_state
)
{
    PROFILE_FUNCTION();
    yuki::debug::Logger::info("erika", "Creating Linux OpenGL context...");

    opengl_platform_state->internal_state = std::make_shared<OpenGL_Internal_State>();
    const auto& opengl_state{ std::static_pointer_cast<OpenGL_Internal_State>(opengl_platform_state->internal_state) };

    const auto& platform_state{ std::static_pointer_cast<Platform_Internal_State>(app_platform_state->internal_state) };

    opengl_state->display = platform_state->display;

    int visual_id = 0;
    int num_fb_configs = 0;
    int default_screen = DefaultScreen(opengl_state->display);
    GLXFBConfig* fb_configs = 0;
    fb_configs = glXGetFBConfigs(opengl_state->display, default_screen, &num_fb_configs);
    if (!fb_configs || num_fb_configs == 0) {
        fprintf(stderr, "glXGetFBConfigs failed\n");
        return -1;
    }

    GLXFBConfig fb_config = fb_configs[0];
    glXGetFBConfigAttrib(opengl_state->display, fb_config, GLX_VISUAL_ID, &visual_id);

    opengl_state->context = glXCreateNewContext(opengl_state->display, fb_config, GLX_RGBA_TYPE, 0, true);
    opengl_state->window = glXCreateWindow(opengl_state->display, fb_config, platform_state->window, 0);
    opengl_state->drawable = opengl_state->window;

    glXMakeContextCurrent(opengl_state->display, opengl_state->drawable, opengl_state->drawable, opengl_state->context);

    return true;
}

void
OpenGL_Platform::swap_buffers(const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state)
{
    const auto& opengl_state{ std::static_pointer_cast<OpenGL_Internal_State>(opengl_platform_state->internal_state) };

    glXSwapBuffers(opengl_state->display, opengl_state->drawable);
}

}

#endif
