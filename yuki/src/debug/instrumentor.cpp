#include "debug/instrumentor.hpp"

#include <iomanip>
#include <sstream>

#include "debug/logger.hpp"

namespace yuki::debug {

Instrumentor&
Instrumentor::get()
{
    static Instrumentor instance;
    return instance;
}

Instrumentor::~Instrumentor()
{
    end_session();
}

void
Instrumentor::begin_session(const std::string& name, const std::string& filepath)
{
    std::lock_guard lock(m_mutex);
    if (!m_current_session.empty()) {
        Logger::error(
            "yuki > Instrumentor::begin_session called with session %s when session %s already open.",
            name.c_str(),
            m_current_session.c_str()
        );
        internal_end_session();
    }

    m_output_stream.open(filepath);

    if (m_output_stream.is_open()) {
        m_current_session = name;
        // Write our file header
        m_output_stream << R"({"otherData": {},"traceEvents":[{})";
        m_output_stream.flush();
    }
    else {
        Logger::error("yuki > Instrumentor::begin_session failed to open result files %s", filepath.c_str());
    }
}

void
Instrumentor::end_session()
{
    std::lock_guard lock(m_mutex);
    internal_end_session();
}

void
Instrumentor::output_profile_result(
    const std::string& name,
    std::chrono::duration<double, std::micro> start_time,
    std::chrono::microseconds elapsed_time,
    std::thread::id thread_id
)
{
    std::stringstream json;
    json << std::setprecision(3) << std::fixed;
    json << ",{";
    json << R"("cat":"function",)";
    json << R"("dur":)" << elapsed_time.count() << ',';
    json << R"("name":")" << name << "\",";
    json << R"("ph":"X",)";
    json << R"("pid":0,)";
    json << R"("tid":)" << thread_id << ",";
    json << R"("ts":)" << start_time.count();
    json << "}";

    std::lock_guard lock(m_mutex);
    if (!m_current_session.empty()) {
        m_output_stream << json.str();
        m_output_stream.flush();
    }
}

void
Instrumentor::internal_end_session()
{
    if (!m_current_session.empty()) {
        // Write our file footer
        m_output_stream << "]}";
        m_output_stream.flush();

        m_output_stream.close();
        m_current_session.clear();
    }
}

}
