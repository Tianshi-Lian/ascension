#include "debug/logger.hpp"

#include <array>
#include <iostream>
#include <libgen.h>
#include <unistd.h>

#include <magic_enum/magic_enum.hpp>

using namespace std;
using namespace yuki;
using namespace std::chrono;

namespace {
template <typename... Args>
void write_direct_log(const char* format, Args&&... args) {
    std::array<char, MAX_LEN_FMT_BUFFER> format_buffer{0};
    int ret = snprintf(format_buffer.begin(), MAX_LEN_FMT_BUFFER, format, std::forward<Args>(args)...);   // NOLINT

    if (ret != -1) {
        std::array<char, MAX_LEN_STR_BUFFER> string_buffer{0};
        ret = snprintf(string_buffer.begin(), MAX_LEN_STR_BUFFER, "%S", format_buffer);   // NOLINT
        if (ret > 0) {
            string str(string_buffer.begin());
            std::cout << str << std::endl;
        }
    }
}

}   // namespace

namespace yuki {

Logger_Worker Logger::s_worker{};   // NOLINT

int Logger_Util::file_exists(const std::string& file_path) {
    if (access(file_path.c_str(), F_OK) != 0) {
        if (errno == ENOENT) {
            write_direct_log("LoggerUtil::HasPermissions() the path(%s) does not exist", file_path.c_str());
        } else if (errno == EACCES) {
            write_direct_log("LoggerUtil::HasPermissions() the path(%s) is not accessible", file_path.c_str());
        }

        return errno;
    }

    return 0;   // success
}

void Logger_Util::get_time_string(char* buffer) {
    constexpr long long ms_in_s = 1000;
    constexpr int start_year = 1900;

    system_clock::time_point now = system_clock::now();

    long long ms_since_epoch = duration_cast<milliseconds>(now.time_since_epoch()).count();
    auto sec_since_epoch = time_t(ms_since_epoch / ms_in_s);

    struct tm time_info {};
    int ret = localtime_s(&time_info, &sec_since_epoch);

    if (ret == 0) {
        // Format time in yyyy-MM-dd HH:mm:ss.SSS format
        // NOLINTNEXTLINE
        ret = snprintf(buffer, MAX_LEN_DATE_BUFFER, "%04d-%02d-%02d %02d:%02d:%02d.%03lld", start_year + time_info.tm_year,
                       1 + time_info.tm_mon, time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec,
                       ms_since_epoch % ms_in_s);

        if (ret == 0) {
            throw Logger_Exception(Log_Exception_Type::FORMAT, "Logger_Util::get_time_string() failed to format time");
        }
    }
}

int Logger_Util::has_permissions(const char* file_path) {
    // Check read access
    if (access(file_path, R_OK) != 0) {
        write_direct_log("LoggerUtil::HasPermissions() the path(%s) is not readable (access denied)", file_path);
        return errno;
    }

    // Check write access
    if (access(file_path, W_OK) != 0) {
        if (errno == EACCES) {
            write_direct_log("LoggerUtil::HasPermissions() the path(%s) is not write-able (access denied)", file_path);
            return errno;
        } else if (errno == EROFS) {
            write_direct_log("LoggerUtil::HasPermissions() the path(%s) is not write-able (read-only file-system)", file_path);
            return errno;
        }
        return errno;
    }

    return 0;
}

void Logger_Util::sleep(unsigned int milliseconds) {
    constexpr unsigned long ms_to_ns = 1000000L;
    constexpr unsigned long s_to_ns = 1000000000L;

    struct timespec time_req = {0};
    unsigned long val = ((unsigned long)milliseconds) * ms_to_ns;
    time_req.tv_sec = val / s_to_ns;
    time_req.tv_nsec = (long)(val % s_to_ns);
    nanosleep(&time_req, (struct timespec*)nullptr);
}

Logger_Worker::Logger_Worker()
    : m_severity_level(Log_Level::ERROR), m_file_log_enabled(false), m_console_log_enabled(false), m_is_app_interrupted(false),
      m_app_log_thread(nullptr) {}

Logger_Worker::~Logger_Worker() {
    try {
        drop_all();
    } catch (Logger_Exception& le) {
        std::cout << "LoggerWorker::~LoggerWorker() error closing stream (" << le.get_message() << ")" << std::endl;
    }
}

void Logger_Worker::initialize(std::string& log_file_path) {
    try {
        this->m_log_file_path = log_file_path;

        m_app_log_thread = std::make_unique<std::thread>(&Logger_Worker::write_to_log_file, this);
        m_app_log_thread->detach();
        m_is_app_interrupted = false;
    } catch (const std::exception& e) {
        write_direct_log("Failed to create logger threads(%s)", e.what());
        throw Logger_Exception(Log_Exception_Type::INIT,
                               Logger_Util::str_format("LoggerWorker::Init(): failed to create logger threads(%s)", e.what()));
    } catch (...) {
        write_direct_log("Failed to create logger threads(unknown exception)");
        throw Logger_Exception(Log_Exception_Type::INIT,
                               "LoggerWorker::Init(): failed to create logger threads(unknown exception)");
    }
}

void Logger_Worker::output_log_line(Log_Level level, const char* log_record) {
    string string(log_record);
    std::lock_guard<std::mutex> lock(m_mutex_log_queue);

    if (m_file_log_enabled) {
        log_queue.push(string);
    }

    if (m_console_log_enabled) {
        // Printing coloured characters to terminal.
        // Not supported by all terminals; if colour sequences are not supported, garbage will show up.
        //
        // The codes for foreground colours used are:
        //          foreground background
        // red      31         41
        // yellow   33         43
        // green    32         42
        // white    37         47
        //
        // Additionally, used numbers are:
        // reset        0  (everything back to normal)
        // bright       1  (often a brighter shade of the same colour)
        // dim          2  (often a dimmer shade of the same colour)
        // inverse      7  (swap foreground and background colours)
        switch (level) {
        case Log_Level::DEBUG:
            cout << "\033[2m" << string << "\033[0m" << std::endl;
            break;
        case Log_Level::NOTICE:
            cout << "\033[1;32m" << string << "\033[0m" << std::endl;
            break;
        case Log_Level::WARNING:
            cout << "\033[1;33m" << string << "\033[0m" << std::endl;
            break;
        case Log_Level::ERROR:
            cout << "\033[1;31m" << string << "\033[0m" << std::endl;
            break;
        case Log_Level::CRITICAL:
            cout << "\033[1;7;31;47m" << string << "\033[0m" << std::endl;
            break;
        default:
            cout << string << endl;
            break;
        }
    }
}

void Logger_Worker::write_to_log_file() {
    while (!m_is_app_interrupted) {
        try {
            string tmp;
            if (!log_queue.pop(tmp)) {
                // Wait for 100 ms to get data filled in queue
                Logger_Util::sleep(SLEEP_IN_MS);
                continue;
            }

            if (!m_log_file_stream.is_open()) {
                m_log_file_stream.open(m_log_file_path, std::ofstream::out | std::ofstream::app | std::ostream::binary);
            }

            // Write errors to stdout when stream error occurred
            if (m_log_file_stream.bad() || m_log_file_stream.fail()) {
                write_direct_log(tmp.c_str());
                m_log_file_stream.close();
            } else {
                {
                    std::lock_guard<std::mutex> lock(m_mutex_log_file);
                    m_log_file_stream << std::string(tmp.begin(), tmp.end()) << endl;
                }
                m_log_file_stream.flush();
            }
        } catch (std::exception& ex) {
            write_direct_log("LoggerWorker::WriteToAplFile() failed to write to application log file(%s)", ex.what());
            throw Logger_Exception(
                Log_Exception_Type::STREAM,
                Logger_Util::str_format("LoggerWorker::WriteToAplFile() failed to write to application log file(%s)",
                                        ex.what()));
        } catch (...) {
            write_direct_log("LoggerWorker::WriteToAplFile() failed to write to application log file(unknown exception)");
            throw Logger_Exception(Log_Exception_Type::STREAM,
                                   "LoggerWorker::WriteToAplFile() failed to write to application log file(unknown exception)");
        }
    }
}

void Logger_Worker::drop_all() {
    // Sleep server for 200 ms before exit
    Logger_Util::sleep(2 * SLEEP_IN_MS);

    m_is_app_interrupted = true;

    try {
        if (m_log_file_stream.is_open()) {
            m_log_file_stream.close();
        }
    } catch (...) {
        throw Logger_Exception(Log_Exception_Type::EXIT, "LoggerWorker::DropAll() failed to close application log file stream");
    }

    // Disable all logging operations
    m_file_log_enabled = false;
    m_console_log_enabled = false;
}

Logger::Logger() = default;

Logger::~Logger() {
    try {
        drop_all();
    } catch (Logger_Exception& le) {
        std::cout << "Logger::~Logger() error closing stream (" << le.get_message() << ")" << std::endl;
    }
}

void Logger::initialize(const std::string& log_file_path) {
    std::string file_path = log_file_path;

    // Set application log file path to default, if empty.
    if (log_file_path.empty() || log_file_path[0] == '\0' || log_file_path[0] == ' ') {
        file_path = LOG_PATH_DEFAULT;
        write_direct_log("Logger::initialize() application log file path is not valid, setting to default value (%s)",
                         log_file_path.c_str());
    }

    // Extract path and check directory permissions
    const std::string& dir_path = file_path;
    size_t final_slash = dir_path.find_last_of('/');
    std::string dir_path_value = dir_path.substr(0, final_slash);
    const auto has_dir_permissions = Logger_Util::has_permissions(dir_path_value.c_str());
    if (has_dir_permissions != 0) {
        write_direct_log("Logger::initialize() failed to validate application log file directory (%s) permissions. Error (%d)",
                         dir_path_value.c_str(), has_dir_permissions);
        throw Logger_Exception(
            Log_Exception_Type::PERMISSION,
            Logger_Util::str_format(
                "initialize::Init() failed to validate application log file directory (%s) permissions. Error (%d)",
                dir_path_value.c_str(), has_dir_permissions));
    }

    // Check file exists, if so check permissions on that file
    const auto file_exists = Logger_Util::file_exists(log_file_path);
    if (file_exists == 0) {
        const auto has_file_permissions = Logger_Util::has_permissions(log_file_path.c_str());
        if (has_file_permissions != 0) {
            write_direct_log("Logger::initialize() failed to validate application log file (%s) permissions. Error (%d)",
                             log_file_path.c_str(), has_file_permissions);
            throw Logger_Exception(
                Log_Exception_Type::PERMISSION,
                Logger_Util::str_format(
                    "Logger::initialize() failed to validate application log file (%s) permissions. Error (%d)",
                    log_file_path.c_str(), has_file_permissions));
        }
    }

    s_worker.initialize(file_path);
}

void Logger::set_log_severity_level(Log_Level level) { s_worker.m_severity_level = level; }

void Logger::enable_file_logging(bool value) { s_worker.m_file_log_enabled = value; }

void Logger::enable_console_logging(bool value) { s_worker.m_console_log_enabled = value; }

void Logger::drop_all() {
    // Sleep server for 200ms to break the write threads wait
    Logger_Util::sleep(2 * SLEEP_IN_MS);

    try {
        s_worker.drop_all();
    } catch (Logger_Exception& le) {
        write_direct_log("Logger::DropAll() error closing stream (%s)", le.get_message());
        throw Logger_Exception(Log_Exception_Type::EXIT,
                               Logger_Util::str_format("Logger::DropAll() error closing stream (%s)", le.get_message()));
    }
}

}   // namespace yuki
