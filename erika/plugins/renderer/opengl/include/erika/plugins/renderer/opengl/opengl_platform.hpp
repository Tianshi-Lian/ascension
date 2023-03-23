#pragma once

#include "opengl_platform_types.hpp"

namespace yuki::platform {
struct Platform_State;
}

namespace erika::plugins::renderer::opengl {
class OpenGL_Platform {
  public:
    static bool create_context(
        const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state,
        const std::shared_ptr<yuki::platform::Platform_State>& platform_state
    );

    static void swap_buffers(const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state);
};

}
