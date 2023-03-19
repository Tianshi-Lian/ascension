
#include <functional>
#include <memory>

namespace yuki::platform {

class Platform_State {
  public:
    std::shared_ptr<void> internal_state;
};

class Library_Handle {
  public:
    std::string filepath;
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

    static Library_Handle load_shared_library(const std::string& filepath);
    static void free_shared_library(const Library_Handle& library_handle);

    template<typename T>
    static std::function<T> load_library_function(const Library_Handle& library_handle, const std::string& function_name)
    {
        const auto function = load_library_function_internal(library_handle, function_name);
        if (function == nullptr) {
            return nullptr;
        }

        std::function<T> result;
        if (function != nullptr) {
            // NOLINTNEXTLINE - we're playing with generic-ing functions to and from system types this is unfortunately needed.
            result = reinterpret_cast<T*>(*function);
        }

        return result;
    }

  private:
    static std::shared_ptr<void (*)()>
    load_library_function_internal(const Library_Handle& library_handle, const std::string& function_name);
};

}
