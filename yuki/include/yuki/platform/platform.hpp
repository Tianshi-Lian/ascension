
#include <memory>

namespace yuki::platform {

class Platform {
  public:
    static bool initialize(
        const std::shared_ptr<void>& platform_state,
        const std::string& application_name,
        i32 pos_x,
        i32 pos_y,
        i32 width,
        i32 height
    );

    static bool shutdown(std::shared_ptr<void> platform_state);
    static bool process_messages(std::shared_ptr<void> platform_state);
    static bool sleep(u64 millisecond);

  private:
    static std::shared_ptr<void> m_internal_state;
};

}
