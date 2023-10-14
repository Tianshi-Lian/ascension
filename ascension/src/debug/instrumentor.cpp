/**
 * File: instrumentor.cpp
 * Project: ascension
 * File Created: 2023-03-06 20:09:27
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-10-14 16:24:22
 * ------------------
 * Copyright 2023 Rob Graham
 * ==================
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ==================
 */

#include "debug/instrumentor.hpp"

#include <iomanip>
#include <sstream>

#include "debug/logger.hpp"

namespace ascension::debug {

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
            "ascension",
            "Instrumentor::begin_session called with session {} when session {} already open.",
            name,
            m_current_session
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
        Logger::error("ascension", "Instrumentor::begin_session failed to open result files {}", filepath);
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
    std::chrono::duration<f64, std::micro> start_time,
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

Instrumentor_Timer::Instrumentor_Timer(std::string name)
  : m_name(std::move(name))
  , m_start_time(std::chrono::steady_clock::now())
  , m_stopped(false)
{
}

Instrumentor_Timer::~Instrumentor_Timer()
{
    if (!m_stopped) {
        stop();
    }
}

void
Instrumentor_Timer::stop()
{
    const auto end_time{ std::chrono::steady_clock::now() };
    const auto elapsed_time{ std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch() -
                             std::chrono::time_point_cast<std::chrono::microseconds>(m_start_time).time_since_epoch() };
    const auto high_res_start_time{ std::chrono::duration<f64, std::micro>{ m_start_time.time_since_epoch() } };

    Instrumentor::get().output_profile_result(m_name, high_res_start_time, elapsed_time, std::this_thread::get_id());

    m_stopped = true;
}

}
