
#include <memory>

namespace yuki::platform {

class Platform_State {
  public:
    std::shared_ptr<void> internal_state;
};

class Platform {
  public:
    static bool initialize(
        const std::shared_ptr<Platform_State>& platform_state,
        const std::string& application_name,
        u32 pos_x,
        u32 pos_y,
        u32 width,
        u32 height
    );

    static void shutdown(const std::shared_ptr<Platform_State>& platform_state);
    static bool process_messages(const std::shared_ptr<Platform_State>& platform_state);
    static void sleep(u32 milliseconds);
};

}
