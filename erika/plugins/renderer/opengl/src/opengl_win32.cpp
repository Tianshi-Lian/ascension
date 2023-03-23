#include "opengl_platform.hpp"

#ifdef _WIN32

#include <glad/glad.h>

#include <windows.h>

#include "yuki/debug/instrumentor.hpp"
#include "yuki/debug/logger.hpp"

#include "yuki/platform/platform_types.hpp"

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0X2092
#define WGL_CONTEXT_FLAGS_ARB 0X2094
#define WGL_CONTEXT_COREPROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

namespace {
struct Platform_Internal_State {
    HINSTANCE h_instance;
    HWND window_handle;
};

struct OpenGL_Internal_State {
    HDC device_context;
};

}

namespace erika::plugins::renderer::opengl {

using PFNWGLCREATECONTEXTATTRIBSARBPROC = HGLRC (*)(HDC, HGLRC, const int*);

using PFNWGLGETEXTENSIONSSTRINGEXTPROC = const char* (*)();
using PFNWGLSWAPINTERVALEXTPROC = BOOL (*)(int);
using PFNWGLGETSWAPINTERVALEXTPROC = int (*)();

bool
OpenGL_Platform::create_context(
    const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state,
    const std::shared_ptr<yuki::platform::Platform_State>& app_platform_state
)
{
    PROFILE_FUNCTION();
    yuki::debug::Logger::info("erika > Creating Win32 OpenGL context...");

    opengl_platform_state->internal_state = std::make_shared<OpenGL_Internal_State>();
    const auto& opengl_state{ std::static_pointer_cast<OpenGL_Internal_State>(opengl_platform_state->internal_state) };

    const auto& platform_state{ std::static_pointer_cast<Platform_Internal_State>(app_platform_state->internal_state) };

    opengl_state->device_context = GetDC(platform_state->window_handle);

    PIXELFORMATDESCRIPTOR format_descriptor{};
    format_descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    format_descriptor.nVersion = 1;
    format_descriptor.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    format_descriptor.iPixelType = PFD_TYPE_RGBA;
    format_descriptor.cColorBits = 24;

    format_descriptor.cDepthBits = 32;
    format_descriptor.cStencilBits = 8;

    int pixelFormat = ChoosePixelFormat(opengl_state->device_context, &format_descriptor);
    SetPixelFormat(opengl_state->device_context, pixelFormat, &format_descriptor);

    HGLRC tempRC = wglCreateContext(opengl_state->device_context);
    wglMakeCurrent(opengl_state->device_context, tempRC);

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    wglCreateContextAttribsARB =
        reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

    const std::array attrib_list = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        3,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        3,
        WGL_CONTEXT_FLAGS_ARB,
        0,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_COREPROFILE_BIT_ARB,
        0,
    };

    HGLRC hglrc = wglCreateContextAttribsARB(opengl_state->device_context, nullptr, attrib_list.begin());
    wglMakeCurrent(nullptr, nullptr);

    wglDeleteContext(tempRC);
    wglMakeCurrent(opengl_state->device_context, hglrc);

    if (gladLoadGL() == 0) {
        yuki::debug::Logger::critical("Could not initialize GLAD");
        return false;
    }
    else {
        yuki::debug::Logger::debug("OpenGL Version %d.%d", GLVersion.major, GLVersion.minor);
    }

    UpdateWindow(platform_state->window_handle);

    // TODO: Support vsync?
    // PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT =
    //     reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(wglGetProcAddress("wglGetExtensionsStringEXT"));
    // bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;
    // int vsync = 0;

    // if (swapControlSupported) {
    //     PFNWGLSWAPINTERVALEXTPROC wglSwapInternalEXT =
    //         reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    //     PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT =
    //         reinterpret_cast<PFNWGLGETSWAPINTERVALEXTPROC>(wglGetProcAddress("wglGetSwapIntervalEXT"));
    //     if (wglSwapInternalEXT(1) != 0) {
    //         std::cout << "VSynch enabled \n";
    //     }
    //     else {
    //         std::cout << "Could not enable VSynch";
    //     }
    // }
    // else {
    //     std::cout << "WGL_EXT_swap_control not supported \n";
    // }

    return true;
}

void
OpenGL_Platform::swap_buffers(const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state)
{
    const auto& opengl_state{ std::static_pointer_cast<OpenGL_Internal_State>(opengl_platform_state->internal_state) };

    SwapBuffers(opengl_state->device_context);
}

}

#endif
