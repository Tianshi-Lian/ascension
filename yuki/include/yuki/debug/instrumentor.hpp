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

}

#define PROFILE_BEGIN_SESSION(name, filepath) yuki::debug::Instrumentor::get().begin_session(name, filepath) // NOLINT
#define PROFILE_END_SESSION() yuki::debug::Instrumentor::get().end_session()                                 // NOLINT

// TODO: Implement debug define
// #if YUKI_DEBUG
// #else
// #define PROFILE_BEGIN_SESSION(name, filepath)
// #define PROFILE_END_SESSION()
// #endif
