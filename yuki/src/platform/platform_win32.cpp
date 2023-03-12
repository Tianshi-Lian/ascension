#include "platform/platform.hpp"

#if _WIN32

#include <windows.h>
#include <windowsx.h>

namespace yuki::platform {

std::shared_ptr<void> Platform::m_internal_state;

struct Internal_State {
    HINSTANCE h_instance;
    HWND hwnd;
};

bool
Platform::initialize(
    const std::shared_ptr<void>& /*platform_state*/,
    const std::string& /*application_name*/,
    i32 /*pos_x*/,
    i32 /*pos_y*/,
    i32 /*width*/,
    i32 /*height*/
)
{
    m_internal_state = std::make_shared<Internal_State>();
    const auto internal_state = std::static_pointer_cast<Internal_State>(m_internal_state);

    return true;
}

}

#endif
