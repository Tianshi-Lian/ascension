#include "debug/logger.h"

#include <array>
#include <iostream>
#include <libgen.h>

#include <magic_enum/magic_enum.hpp>

using namespace std;
using namespace yuki;
using namespace std::chrono;

namespace {
void write_direct_log(const wchar_t* format, va_list args) {
    std::array<wchar_t, MAX_LEN_FMT_BUFFER> format_buffer{0};
    std::array<wchar_t, MAX_LEN_STR_BUFFER> string_buffer{0};
    int ret = vswprintf(format_buffer.begin(), MAX_LEN_FMT_BUFFER, format, args);

    if (ret != -1) {
        ret = swprintf(string_buffer.begin(), MAX_LEN_STR_BUFFER, L"%S", format_buffer);   // NOLINT
        if (ret > 0) {
            wstring wstr(string_buffer.begin());
            string str(wstr.begin(), wstr.end());
            std::wcout << wstr << std::endl;
        }
    }
}

void write_sys_log(const wchar_t* format, ...) {
    va_list va_list = nullptr;
    va_start(va_list, format);
    write_direct_log(format, va_list);
    va_end(va_list);
}

}   // namespace

namespace yuki {

Logger_Worker Logger::s_worker{};   // NOLINT

bool Logger_Util::file_exists(const std::string& file_path) { return (access(file_path.c_str(), F_OK) != -1); }

void Logger_Util::get_time_string(wchar_t* buff) {
    constexpr long long ms_in_s = 1000;
    constexpr int start_year = 1900;

    system_clock::time_point now = system_clock::now();

    long long ms_since_epoch = duration_cast<milliseconds>(now.time_since_epoch()).count();
    auto sec_since_epoch = time_t(ms_since_epoch / ms_in_s);

    struct tm time_info {};
    int ret = localtime_s(&time_info, &sec_since_epoch);

    if (ret == 0) {
        // Format time in yyyy-MM-dd HH:mm:ss.SSS format
        ret = swprintf(buff, MAX_LEN_DATE_BUFFER, L"%04d-%02d-%02d %02d:%02d:%02d.%03lld", start_year + time_info.tm_year,
                       1 + time_info.tm_mon, time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec,
                       ms_since_epoch % ms_in_s);

        if (ret == 0) {
            throw Logger_Exception(Log_Exception_Type::FORMAT, L"Logger_Util::get_time_string() failed to format time");
        }
    }
}

int Logger_Util::has_permissions(const char* file_path) {
    // Check file existence
    if (access(file_path, F_OK) != 0) {
        if (errno == ENOENT) {
            write_sys_log(L"LoggerUtil::HasPermissions() the path(%s) does not exist", file_path);
        } else if (errno == EACCES) {
            write_sys_log(L"LoggerUtil::HasPermissions() the path(%s) is not accessible", file_path);
        }

        return errno;
    }

    // Check read access
    if (access(file_path, R_OK) != 0) {
        write_sys_log(L"LoggerUtil::HasPermissions() the path(%s) is not readable (access denied)", file_path);
        return errno;
    }

    // Check write access
    if (access(file_path, W_OK) != 0) {
        if (errno == EACCES) {
            write_sys_log(L"LoggerUtil::HasPermissions() the path(%s) is not write-able (access denied)", file_path);
            return errno;
        } else if (errno == EROFS) {
            write_sys_log(L"LoggerUtil::HasPermissions() the path(%s) is not write-able (read-only file-system)", file_path);
            return errno;
        }
        return errno;
    }

    // Success, return zero(0)
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

std::wstring Logger_Util::str_format(const wchar_t* format, ...) {
    va_list va_list = nullptr;
    va_start(va_list, format);
    std::array<wchar_t, MAX_LEN_FMT_BUFFER> format_buffer{0};
    int ret = vswprintf(format_buffer.begin(), MAX_LEN_FMT_BUFFER, format, va_list);
    va_end(va_list);

    wstring retString;
    if (ret != -1) {
        retString = wstring(format_buffer.begin());
    }

    return retString;
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

        // Application log write thread creation
        m_app_log_thread = std::make_unique<std::thread>(&Logger_Worker::write_to_log_file, this);
        m_app_log_thread->detach();
        m_is_app_interrupted = false;
    } catch (const std::exception& e) {
        // Write to syslog and throw
        write_sys_log(L"Failed to create logger threads(%s)", e.what());
        throw Logger_Exception(Log_Exception_Type::INIT,
                               Logger_Util::str_format(L"LoggerWorker::Init(): failed to create logger threads(%s)", e.what()));
    } catch (...) {
        // Write to syslog and throw
        write_sys_log(L"Failed to create logger threads(unknown exception)");
        throw Logger_Exception(Log_Exception_Type::INIT,
                               L"LoggerWorker::Init(): failed to create logger threads(unknown exception)");
    }
}

void Logger_Worker::output_log_line(Log_Level level, const wchar_t* log_record) {
    wstring wstring(log_record);
    std::lock_guard<std::mutex> lock(m_mutex_log_queue);

    if (m_file_log_enabled) {
        log_queue.push(wstring);
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
        // reset             0  (everything back to normal) - at the end
        // bold/bright       1  (often a brighter shade of the same colour) - to make the text bold
        // inverse           7  (swap foreground and background colours)
        // TODO(rgg): Dim debug lines a little bit?
        switch (level) {
        case Log_Level::NOTICE:
            wcout << L"\033[1;32m" << wstring << "\033[0m" << std::endl;
            break;
        case Log_Level::WARNING:
            wcout << L"\033[1;33m" << wstring << "\033[0m" << std::endl;
            break;
        case Log_Level::ERROR:
            wcout << L"\033[1;31m" << wstring << "\033[0m" << std::endl;
            break;
        case Log_Level::CRITICAL:
            wcout << L"\033[1;7;31;47m" << wstring << "\033[0m" << std::endl;
            break;
        default:
            wcout << wstring << endl;
            break;
        }
    }
}

void Logger_Worker::write_to_log_file() {
    while (!m_is_app_interrupted) {
        try {
            wstring tmp;
            if (!log_queue.pop(tmp)) {
                // Wait for 100 ms to get data filled in queue
                Logger_Util::sleep(SLEEP_IN_MS);
                continue;
            }

            if (!m_log_file_stream.is_open()) {
                m_log_file_stream.open(m_log_file_path, std::ofstream::out | std::ofstream::app | std::ostream::binary);
            }

            // Write errors to syslog when stream error occurred
            if (m_log_file_stream.bad() || m_log_file_stream.fail()) {
                write_sys_log(tmp.c_str());
                m_log_file_stream.close();
            } else {
                {
                    std::lock_guard<std::mutex> lock(m_mutex_log_file);
                    m_log_file_stream << std::string(tmp.begin(), tmp.end()) << endl;
                }
                m_log_file_stream.flush();
            }
        } catch (std::exception& ex) {
            write_sys_log(L"LoggerWorker::WriteToAplFile() failed to write to application log file(%s)", ex.what());
            throw Logger_Exception(
                Log_Exception_Type::STREAM,
                Logger_Util::str_format(L"LoggerWorker::WriteToAplFile() failed to write to application log file(%s)",
                                        ex.what()));
        } catch (...) {
            write_sys_log(L"LoggerWorker::WriteToAplFile() failed to write to application log file(unknown exception)");
            throw Logger_Exception(
                Log_Exception_Type::STREAM,
                L"LoggerWorker::WriteToAplFile() failed to write to application log file(unknown exception)");
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
        throw Logger_Exception(Log_Exception_Type::EXIT,
                               L"LoggerWorker::DropAll() failed to close application log file stream");
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
        write_sys_log(L"Logger::initialize() application log file path is not valid, setting to default value (%s)",
                      log_file_path.c_str());
    }

    // Extract path and check directory permissions
    const std::string& dir_path = file_path;
    size_t final_slash = dir_path.find_last_of('/');
    std::string dir_path_value = dir_path.substr(0, final_slash);
    if (Logger_Util::has_permissions(dir_path_value.c_str()) != 0) {
        write_sys_log(L"Logger::Init() failed to validate application log file directory (%s) permissions",
                      dir_path_value.c_str());
        throw Logger_Exception(
            Log_Exception_Type::PERMISSION,
            Logger_Util::str_format(L"Logger::Init() failed to validate application log file directory (%s) permissions",
                                    dir_path_value.c_str()));
    }

    // Validate application log file permission if already exists
    if (Logger_Util::file_exists(log_file_path) && Logger_Util::has_permissions(log_file_path.c_str()) != 0) {
        write_sys_log(L"Logger::Init() failed to validate application log file (%s) permissions", log_file_path.c_str());
        throw Logger_Exception(
            Log_Exception_Type::PERMISSION,
            Logger_Util::str_format(L"Logger::Init() failed to validate application log file (%s) permissions",
                                    log_file_path.c_str()));
    }

    s_worker.initialize(file_path);
}

void Logger::set_log_severity_level(Log_Level level) { s_worker.m_severity_level = level; }

void Logger::enable_file_logging(bool value) { s_worker.m_file_log_enabled = value; }

void Logger::enable_console_logging(bool value) { s_worker.m_console_log_enabled = value; }

void Logger::critical(const wchar_t* format, ...) {
    if (Log_Level::CRITICAL < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::CRITICAL, LOG_CODE_CRIT_DEFAULT, format, va_list);
    va_end(va_list);
}

void Logger::critical(unsigned long code, const wchar_t* format, ...) {
    if (Log_Level::CRITICAL < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::CRITICAL, code, format, va_list);
    va_end(va_list);
}

void Logger::error(const wchar_t* format, ...) {
    if (Log_Level::ERROR < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::ERROR, LOG_CODE_ERR_DEFAULT, format, va_list);
    va_end(va_list);
}

void Logger::error(unsigned long code, const wchar_t* format, ...) {
    if (Log_Level::ERROR < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::ERROR, code, format, va_list);
    va_end(va_list);
}

void Logger::info(const wchar_t* format, ...) {
    if (Log_Level::INFO < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::INFO, LOG_CODE_INFO_DEFAULT, format, va_list);
    va_end(va_list);
}

void Logger::info(unsigned long code, const wchar_t* format, ...) {
    if (Log_Level::INFO < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::INFO, code, format, va_list);
    va_end(va_list);
}

void Logger::warn(const wchar_t* format, ...) {
    if (Log_Level::WARNING < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::WARNING, LOG_CODE_WARN_DEFAULT, format, va_list);
    va_end(va_list);
}

void Logger::warn(unsigned long code, const wchar_t* format, ...) {
    if (Log_Level::WARNING < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::WARNING, code, format, va_list);
    va_end(va_list);
}

void Logger::debug(const wchar_t* format, ...) {
    if (Log_Level::DEBUG < s_worker.m_severity_level) {
        return;
    }

    va_list va_list = nullptr;
    va_start(va_list, format);
    write_log(Log_Level::DEBUG, LOG_CODE_DEBUG_DEFAULT, format, va_list);
    va_end(va_list);
}

void Logger::drop_all() {
    // Sleep server for 200ms to break the write threads wait
    Logger_Util::sleep(2 * SLEEP_IN_MS);

    try {
        s_worker.drop_all();
    } catch (Logger_Exception& le) {
        write_sys_log(L"Logger::DropAll() error closing stream (%s)", le.get_message());
        throw Logger_Exception(Log_Exception_Type::EXIT,
                               Logger_Util::str_format(L"Logger::DropAll() error closing stream (%s)", le.get_message()));
    }
}

void Logger::write_log(Log_Level level, unsigned long code, const wchar_t* format, va_list args) {
    std::array<wchar_t, MAX_LEN_DATE_BUFFER> time{0};
    std::array<wchar_t, MAX_LEN_FMT_BUFFER> format_buffer{0};
    std::array<wchar_t, MAX_LEN_STR_BUFFER> string_buffer{0};
    Logger_Util::get_time_string(time.begin());

    int ret = vswprintf(format_buffer.begin(), MAX_LEN_FMT_BUFFER, format, args);

    auto log_level = std::string(magic_enum::enum_name(level));

    // yyyy-MM-dd HH:mm:ss.SSS [LEVEL ](code): Message
    ret = swprintf(string_buffer.begin(), MAX_LEN_STR_BUFFER, L"%S [%-6s](%04lu): %S", time, log_level.c_str(), code,
                   format_buffer);

    if (ret > 0) {
        s_worker.output_log_line(level, string_buffer.begin());
    }
}

}   // namespace yuki
