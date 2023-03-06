#include <cstddef>
#include <filesystem>
#include <iostream>

#include "debug/logger.hpp"

using namespace yuki;
using namespace std::chrono;

namespace {

template<typename... Args>
void
write_direct_log(const std::string& format, Args&&... args)
{
    std::vector<char> format_buffer(DEFAULT_BUFFER_LENGTH);
    int ret = snprintf(format_buffer.data(), format_buffer.size(), format.c_str(), std::forward<Args>(args)...);

    if (ret != -1) {
        std::vector<char> string_buffer(DEFAULT_BUFFER_LENGTH);
        ret = snprintf(string_buffer.data(), string_buffer.size(), "%s", format_buffer.data());
        if (ret > 0) {
            std::string str(string_buffer.data());
            std::cout << str << std::endl;
        }
    }
}

} // namespace

namespace yuki {
using std::localtime;

std::string
Logger_Util::get_time_string()
{
    constexpr i64 ms_in_s = 1000;
    constexpr i64 start_year = 1900;

    system_clock::time_point now = system_clock::now();

    const i64 ms_since_epoch = duration_cast<milliseconds>(now.time_since_epoch()).count();
    const i64 sec_since_epoch = ms_since_epoch / ms_in_s;

    std::string result;
    {
        std::lock_guard<std::mutex> lock(get_mutex());
        auto* time_info = std::localtime(&sec_since_epoch); // NOLINT

        std::vector<char> time_buffer(DEFAULT_BUFFER_LENGTH);

        if (time_info != nullptr) {
#ifdef _WIN32
            // NOLINTNEXTLINE
            const auto ret = snprintf(
                time_buffer.data(),
                time_buffer.size(),
                "%04lld-%02d-%02d %02d:%02d:%02d.%03lld",
                start_year + time_info->tm_year,
                1 + time_info->tm_mon,
                time_info->tm_mday,
                time_info->tm_hour,
                time_info->tm_min,
                time_info->tm_sec,
                ms_since_epoch % ms_in_s
            );
#elif __linux__
            // NOLINTNEXTLINE
            const auto ret = snprintf(
                time_buffer.data(),
                time_buffer.size(),
                "%04ld-%02d-%02d %02d:%02d:%02d.%03ld",
                start_year + time_info->tm_year,
                1 + time_info->tm_mon,
                time_info->tm_mday,
                time_info->tm_hour,
                time_info->tm_min,
                time_info->tm_sec,
                ms_since_epoch % ms_in_s
            );
#endif

            if (ret == 0) {
                throw Logger_Exception(Log_Exception_Type::FORMAT, "Logger_Util::get_time_string() failed to format time");
            }
        }
        result = std::string(time_buffer.data());
    }

    return result;
}

bool
Logger_Util::has_permissions_for_file(const std::string& filepath)
{
    // Check read access
    {
        std::ifstream file(filepath);
        if (!file.good()) {
            write_direct_log(
                "LoggerUtil::HasPermissions() the path(%s) is not "
                "readable (access denied)",
                filepath.c_str()
            );
            return false;
        }
    }

    // Check write access
    {
        std::ofstream file(filepath);
        if (!file.good()) {
            write_direct_log(
                "LoggerUtil::HasPermissions() the path(%s) is not "
                "write-able (access denied)",
                filepath.c_str()
            );
            return false;
        }
    }

    return true;
}

void
Logger_Util::sleep(unsigned int milliseconds)
{
    constexpr i32 ms_to_ns = 1000000L;
    constexpr i64 s_to_ns = 1000000000L;

    struct timespec time_req = { 0, 0 };
    i64 val = static_cast<i64>(milliseconds) * ms_to_ns;
    time_req.tv_sec = val / s_to_ns;
    time_req.tv_nsec = static_cast<i32>(val % s_to_ns);
    nanosleep(&time_req, static_cast<struct timespec*>(nullptr));
}

std::mutex&
Logger_Util::get_mutex()
{
    static std::mutex s_mutex;
    return s_mutex;
}

Logger_Worker::Logger_Worker()
  : m_app_log_thread(nullptr)
  , m_is_app_interrupted(false)
  , m_severity_level(Log_Level::ERROR)
  , m_file_log_enabled(false)
  , m_console_log_enabled(false)
{
}

Logger_Worker::~Logger_Worker()
{
    try {
        drop_all();
    }
    catch (Logger_Exception& le) {
        std::cout << "LoggerWorker::~LoggerWorker() error closing stream (" << le.get_message() << ")" << std::endl;
    }
}

void
Logger_Worker::initialize(const std::string& log_filepath)
{
    try {
        this->m_log_filepath = log_filepath;

        m_app_log_thread = std::make_unique<std::thread>(&Logger_Worker::write_to_log_file, this);
        m_app_log_thread->detach();
        m_is_app_interrupted = false;
    }
    catch (const std::exception& e) {
        write_direct_log("Failed to create logger threads(%s)", e.what());
        throw Logger_Exception(
            Log_Exception_Type::INIT,
            Logger_Util::str_format("LoggerWorker::Init(): failed to create logger threads(%s)", e.what())
        );
    }
    catch (...) {
        write_direct_log("Failed to create logger threads(unknown exception)");
        throw Logger_Exception(
            Log_Exception_Type::INIT,
            "LoggerWorker::Init(): failed to create logger "
            "threads(unknown exception)"
        );
    }
}

void
Logger_Worker::output_log_line(Log_Level level, const std::string& log_record)
{
    std::lock_guard<std::mutex> lock(m_mutex_log_queue);

    if (m_file_log_enabled) {
        log_queue.push(log_record);
    }

    if (m_console_log_enabled) {
        // Printing coloured characters to terminal.
        // Not supported by all terminals; if colour sequences are not
        // supported, garbage will show up.
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
                std::cout << "\033[2m" << log_record << "\033[0m" << std::endl;
                break;
            case Log_Level::NOTICE:
                std::cout << "\033[1;32m" << log_record << "\033[0m" << std::endl;
                break;
            case Log_Level::WARNING:
                std::cout << "\033[1;33m" << log_record << "\033[0m" << std::endl;
                break;
            case Log_Level::ERROR:
                std::cout << "\033[1;31m" << log_record << "\033[0m" << std::endl;
                break;
            case Log_Level::CRITICAL:
                std::cout << "\033[1;7;31;47m" << log_record << "\033[0m" << std::endl;
                break;
            default:
                std::cout << log_record << std::endl;
                break;
        }
    }
}

void
Logger_Worker::write_to_log_file()
{
    while (!m_is_app_interrupted) {
        try {
            std::string tmp;
            if (!log_queue.pop(tmp)) {
                // Wait for 100 ms to get data filled in queue
                Logger_Util::sleep(SLEEP_IN_MS);
                continue;
            }

            if (!m_log_file_stream.is_open()) {
                m_log_file_stream.open(m_log_filepath, std::ofstream::out | std::ofstream::app | std::ostream::binary);
            }

            // Write errors to stdout when stream error occurred
            if (m_log_file_stream.bad() || m_log_file_stream.fail()) {
                write_direct_log(tmp);
                m_log_file_stream.close();
            }
            else {
                {
                    std::lock_guard<std::mutex> lock(m_mutex_log_file);
                    m_log_file_stream << std::string(tmp.begin(), tmp.end()) << std::endl;
                }
                m_log_file_stream.flush();
            }
        }
        catch (std::exception& ex) {
            write_direct_log(
                "LoggerWorker::WriteToAplFile() failed to write to "
                "application log file(%s)",
                ex.what()
            );
            throw Logger_Exception(
                Log_Exception_Type::STREAM,
                Logger_Util::str_format(
                    "LoggerWorker::WriteToAplFile() failed to "
                    "write to application log file(%s)",
                    ex.what()
                )
            );
        }
        catch (...) {
            write_direct_log("LoggerWorker::WriteToAplFile() failed to write to "
                             "application log file(unknown exception)");
            throw Logger_Exception(
                Log_Exception_Type::STREAM,
                "LoggerWorker::WriteToAplFile() failed to write "
                "to application log file(unknown exception)"
            );
        }
    }
}

void
Logger_Worker::drop_all()
{
    // Sleep server for 200 ms before exit
    Logger_Util::sleep(2 * SLEEP_IN_MS);

    m_is_app_interrupted = true;

    try {
        if (m_log_file_stream.is_open()) {
            m_log_file_stream.close();
        }
    }
    catch (...) {
        throw Logger_Exception(
            Log_Exception_Type::EXIT,
            "LoggerWorker::DropAll() failed to close application log file "
            "stream"
        );
    }

    // Disable all logging operations
    m_file_log_enabled = false;
    m_console_log_enabled = false;
}

Logger::Logger() = default;

Logger::~Logger()
{
    try {
        drop_all();
    }
    catch (Logger_Exception& le) {
        std::cout << "Logger::~Logger() error closing stream (" << le.get_message() << ")" << std::endl;
    }
}

void
Logger::initialize(const std::string& log_filepath, Log_Level level, bool log_to_file, bool log_to_console)
{
    std::string filepath = log_filepath;

    size_t final_slash = log_filepath.find_last_of('/');
    const std::string dir_path = filepath.substr(0, final_slash);

    // Set application log file path to default, if empty.
    if (log_filepath.empty() || log_filepath[0] == '\0' || log_filepath[0] == ' ') {
        filepath = LOG_PATH_DEFAULT;
        write_direct_log(
            "Logger::initialize() application log file path is not "
            "valid, setting to default value (%s)",
            log_filepath.c_str()
        );
    }

    // Check that directory exists, otherwise attempt to make it
    if (!std::filesystem::exists(dir_path)) {
        std::error_code error;
        const auto success = std::filesystem::create_directory(dir_path, error);
        if (!success) {
            write_direct_log(
                "Logger::initialize() failed to create log file "
                "directory (%s). Error (%d)",
                dir_path.c_str(),
                error
            );
            throw Logger_Exception(
                Log_Exception_Type::INIT,
                Logger_Util::str_format(
                    "initialize::Init() failed to create log "
                    "file directory (%s). Error (%d)",
                    dir_path.c_str(),
                    error
                )
            );
        }
    }

    // Check file exists, otherwise attempt to make it
    auto log_file_just_created = false;
    if (!std::filesystem::exists(filepath)) {
        std::ofstream out_file(filepath);
        out_file << " == Log Start == ";
        out_file.close();
        log_file_just_created = true;
    }

    // Check file permissions
    const auto has_file_permissions = Logger_Util::has_permissions_for_file(log_filepath);
    if (!has_file_permissions) {
        write_direct_log(
            "Logger::initialize() failed to validate application log "
            "file (%s) permissions. Error (%d)",
            log_filepath.c_str(),
            has_file_permissions
        );
        throw Logger_Exception(
            Log_Exception_Type::PERMISSION,
            Logger_Util::str_format(
                "Logger::initialize() failed to validate application log file "
                "(%s) "
                "permissions. Error (%d)",
                log_filepath.c_str(),
                has_file_permissions
            )
        );
    }

    get_worker().initialize(filepath);

    set_log_severity_level(level);

    enable_file_logging(log_to_file);
    if (!log_file_just_created) {
        Logger::log(Log_Level::MANUAL, " ");
    }
    enable_console_logging(log_to_console);

    Logger::notice("yuki > Logger Initialized.");
}

Logger_Worker&
Logger::get_worker()
{
    static Logger_Worker s_worker;
    return s_worker;
}

void
Logger::set_log_severity_level(Log_Level level)
{
    get_worker().m_severity_level = level;
}

void
Logger::enable_file_logging(bool value)
{
    get_worker().m_file_log_enabled = value;
}

void
Logger::enable_console_logging(bool value)
{
    get_worker().m_console_log_enabled = value;
}

void
Logger::drop_all()
{
    // Sleep server for 200ms to break the write threads wait
    Logger_Util::sleep(2 * SLEEP_IN_MS);

    try {
        get_worker().drop_all();
    }
    catch (Logger_Exception& le) {
        write_direct_log("Logger::DropAll() error closing stream (%s)", le.get_message().c_str());
        throw Logger_Exception(
            Log_Exception_Type::EXIT,
            Logger_Util::str_format("Logger::DropAll() error closing stream (%s)", le.get_message().c_str())
        );
    }
}

} // namespace yuki
