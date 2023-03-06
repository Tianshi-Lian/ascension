#include <chrono>
#include <fstream>
#include <mutex>
#include <thread>

namespace yuki::debug {

class Instrumentor {
  public:
    Instrumentor(const Instrumentor&) = delete;
    Instrumentor(Instrumentor&&) = delete;
    Instrumentor& operator=(const Instrumentor&) = delete;
    Instrumentor& operator=(Instrumentor&&) = delete;

    static Instrumentor& get();

    void begin_session(const std::string& name, const std::string& filepath = "logs/results.json");
    void end_session();

    void output_profile_result(
        const std::string& name,
        std::chrono::duration<double, std::micro> start_time,
        std::chrono::microseconds elapsed_time,
        std::thread::id thread_id
    );

  private:
    Instrumentor() = default;
    ~Instrumentor();

    void internal_end_session();

    std::mutex m_mutex;
    std::string m_current_session;
    std::ofstream m_output_stream;
};

class Instrumentor_Timer {
  public:
    explicit Instrumentor_Timer(std::string name);
    ~Instrumentor_Timer();

    Instrumentor_Timer(const Instrumentor_Timer&) = default;
    Instrumentor_Timer(Instrumentor_Timer&&) = delete;
    Instrumentor_Timer& operator=(const Instrumentor_Timer&) = default;
    Instrumentor_Timer& operator=(Instrumentor_Timer&&) = delete;

    void stop();

  private:
    std::string m_name;
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
    bool m_stopped;
};

// TODO: Change all our arrays to std::array<char, N>
template<size_t N>
struct cleanup_timer_name_result {
    char data[N];
};

template<size_t N, size_t K>
constexpr auto
cleanup_timer_name(const char (&expr)[N], const char (&remove)[K])
{
    cleanup_timer_name_result<N> result = {};

    size_t src_index = 0;
    size_t dst_index = 0;
    while (src_index < N) {
        size_t match_index = 0;
        while (match_index < K - 1 && src_index + match_index < N - 1 && expr[src_index + match_index] == remove[match_index]) {
            match_index++;
        }
        if (match_index == K - 1) {
            src_index += match_index;
        }
        result.data[dst_index++] = expr[src_index] == '"' ? '\'' : expr[src_index];
        src_index++;
    }
    return result;
}

}

#define PROFILE_BEGIN_SESSION(name, filepath) yuki::debug::Instrumentor::get().begin_session(name, filepath) // NOLINT
#define PROFILE_END_SESSION() yuki::debug::Instrumentor::get().end_session()                                 // NOLINT

// NOLINTNEXTLINE
#define PROFILE_SCOPE_LINE_INTERNAL(name, line)                                                                                \
    constexpr auto fixedName##line = yuki::debug::cleanup_timer_name(name, "__cdecl ");                                        \
    yuki::debug::Instrumentor_Timer timer##line(fixedName##line.data)
#define PROFILE_SCOPE_LINE(name, line) PROFILE_SCOPE_LINE_INTERNAL(name, line) // NOLINT
#define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)                 // NOLINT
#define PROFILE_FUNCTION() PROFILE_SCOPE(__PRETTY_FUNCTION__)                  // NOLINT

// TODO: Implement debug define
// #if YUKI_DEBUG
// #else
// #define PROFILE_BEGIN_SESSION(name, filepath)
// #define PROFILE_END_SESSION(YDI_FUNC_SIG)
// #endif
