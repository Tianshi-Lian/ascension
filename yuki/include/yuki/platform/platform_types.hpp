#pragma once

#include <memory>

namespace yuki::platform {

/**
 * @struct Platform_State
 *
 * @brief A struct holding platform-independent state.
 */
struct Platform_State {
    std::shared_ptr<void> internal_state;
};

/**
 * @struct Library_Handle
 *
 * @brief A struct holding a platform-independent handle to a shared library.
 */
struct Library_Handle {
    std::string filepath;
    std::shared_ptr<void> internal_state;
};
}
