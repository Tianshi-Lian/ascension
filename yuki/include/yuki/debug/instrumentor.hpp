#include <chrono>
#include <fstream>
#include <mutex>
#include <thread>

namespace yuki::debug {

/**
 * @class Instrumentor
 *
 * @brief A debug utility class for profiling/instrumenting code.
 */
class Instrumentor {
  public:
    Instrumentor(const Instrumentor&) = delete;
    Instrumentor(Instrumentor&&) = delete;
    Instrumentor& operator=(const Instrumentor&) = delete;
    Instrumentor& operator=(Instrumentor&&) = delete;

    /**
     * @brief Get the global Instrumentor object.
     *
     * @return  reference to the global Instrumentor.
     */
    static Instrumentor& get();

    /**
     * @brief Begin a new instrumentation session.
     * This will collect and output profiling calls until end_session is called.
     *
     * @param   name      The name of the profiling session.
     * @param   filepath  The filepath to output the results to. Results are in json format.
     */
    void begin_session(const std::string& name, const std::string& filepath = "logs/results.json");

    /**
     * @brief End the current profiling session.
     */
    void end_session();

    /**
     * @brief Output a profiling result into the active session.
     *
     * @param   name          The result name, this is usually the fuction/line which is profiled.
     * @param   start_time    The starting time in microseconds of the profiled section.
     * @param   elapsed_time  The time elapsed during the profiled section.
     * @param   thread_id     The ID of the thread which had this profiled section.
     */
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

    /**
     * @brief Stop the current timer.
     * This will cause the result to be written to the current session.
     */
    void stop();

  private:
    std::string m_name;
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
    bool m_stopped;
};

template<size_t N>
struct _cleanup_timer_name_result {
    std::array<char, N> data;
};

template<size_t N, size_t K>
constexpr auto
// NOLINTNEXTLINE = This all has to be compile time literals and it's silly to copy a string for the sake of std::array
_cleanup_timer_name(const char (&expr)[N], const char (&remove)[K])
{
    _cleanup_timer_name_result<N> result = {};

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

#ifdef YUKI_DEBUG
#define PROFILE_BEGIN_SESSION(name, filepath) yuki::debug::Instrumentor::get().begin_session(name, filepath) // NOLINT
#define PROFILE_END_SESSION() yuki::debug::Instrumentor::get().end_session()                                 // NOLINT

// NOLINTNEXTLINE
#define PROFILE_SCOPE_LINE_INTERNAL(name, line)                                                                                \
    constexpr auto fixedName##line = yuki::debug::_cleanup_timer_name(name, "__cdecl ");                                       \
    yuki::debug::Instrumentor_Timer timer##line(std::string(fixedName##line.data.data()))
#define PROFILE_SCOPE_LINE(name, line) PROFILE_SCOPE_LINE_INTERNAL(name, line) // NOLINT
#define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)                 // NOLINT
#define PROFILE_FUNCTION() PROFILE_SCOPE(__PRETTY_FUNCTION__)                  // NOLINT

#else
#define PROFILE_BEGIN_SESSION(name, filepath)
#define PROFILE_END_SESSION(YDI_FUNC_SIG)

#define PROFILE_SCOPE_LINE(name, line)
#define PROFILE_SCOPE_LINE_INTERNAL(name, line)
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif
