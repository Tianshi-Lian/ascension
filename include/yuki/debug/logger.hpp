// A simple thread-safe application logging class.
// Supports logging to the console and to a file.
// Original by: https://github.com/prajilal/cpplogger

#pragma once

#include <array>
#include <cstdarg>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>

#include <magic_enum/magic_enum.hpp>

#define MAX_LEN_FMT_BUFFER  2048
#define MAX_LEN_STR_BUFFER  2080
#define MAX_LEN_DATE_BUFFER 32
#define SLEEP_IN_MS         100

#define LOG_PATH_DEFAULT "logs/app.log"

// Default log/error codes
#define LOG_CODE_DEBUG_DEFAULT  0001
#define LOG_CODE_INFO_DEFAULT   0001
#define LOG_CODE_NOTICE_DEFAULT 0001
#define LOG_CODE_WARN_DEFAULT   0001
#define LOG_CODE_ERROR_DEFAULT  0001
#define LOG_CODE_CRIT_DEFAULT   0001

namespace yuki {

/**
 * @enum Log_Level
 *
 * @brief Enumerator which defined the different log severity levels.
 * Available log levels are:
 *   DEBUG, INFO, NOTICE, WARNING, ERROR, CRITICAL
 */
enum class Log_Level { DEBUG = 0, INFO = 1, NOTICE = 2, WARNING = 3, ERROR = 4, CRITICAL = 5 };

/**
 * @enum Log_Exception_Type
 *
 * @brief Enumerator which defines the different logger exception types.
 * Available exception types are:
 *   INIT, STREAM, PERMISSION, EXIT
 */
enum class Log_Exception_Type { INIT = 1, STREAM = 2, PERMISSION = 3, EXIT = 4, INVALID_ARG = 5, FORMAT = 6 };

/**
 * @class Logger_Exception
 *
 * @brief Class which implements logger general exception handler.
 */
class Logger_Exception : std::exception {
    public:
    /*! Sets the logger exception type.
     *
     * @param	type The logger exception type.
     */
    explicit Logger_Exception(Log_Exception_Type type) : m_type(type){};

    /*! Sets the logger exception type & message in C-string format.
     *
     * @param	type	    The logger exception type.
     * @param	message		The logger exception message in C-string format.
     */
    Logger_Exception(Log_Exception_Type type, std::string message) : m_message(std::move(message)), m_type(type){};

    /**
     * Retrieves the logger exception type.
     *
     * @return	The logger exception type.
     */
    Log_Exception_Type get_type() { return m_type; };

    /**
     * Retrieves the logger exception message as C-string.
     *
     * @return	The logger exception message as C-string.
     */
    const char* get_message() { return m_message.c_str(); };

    private:
    std::string m_message;
    Log_Exception_Type m_type;
};

/**
 * @class Blocking_string_Queue
 *
 * @brief Utility queue class which performs string pop/push operation.
 */
class Blocking_string_Queue {
    public:
    /**
     * Pop elements from queue.
     *
     * @param	result	Pointer to a string where the resulting C-string is stored.
     *
     * @return 	true is returned in the case that an item popped from the queue.
     *			Otherwise, false is returned.
     */
    bool pop(std::string& result) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        result = m_queue.front();
        m_queue.pop();
        return true;
    };

    /**
     * Push an element to the queue.
     *
     * @param	value		The C-string element that requires insertion.
     */
    void push(const std::string& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(value);
    };

    private:
    std::mutex m_mutex;
    std::queue<std::string> m_queue;
};

/**
 * @class Logger_Util
 *
 * @brief Utility class which provides helper functions to logging operations.
 */
class Logger_Util {

    public:
    /**
     * Checks whether the specified file exists
     *
     * @param 	file_path	Pointer to a file name that requires validation.
     *
     * @return 	0 is returned in the case that the file exist.
     *			Otherwise, an error code is returned.
     */
    static int file_exists(const std::string& file_path);

    /**
     * Write time stamp in 'yyyy-MM-dd HH:mm:ss.SSS' format to string.
     *
     * @param	buffer	Pointer to a buffer where the resulting C-char is stored.
     *					The buffer should be large enough to contain the resulting string.
     */
    static void get_time_string(char* buffer);

    /**
     * Checks whether proper permissions (exist/read/write) are set..
     *
     * @param	file_path		Pointer to the file/directory name that requires validation
     *
     * @return 	0 is returned in the case that proper permissions are set.
     *			Otherwise, error number is returned.
     */
    static int has_permissions(const char* file_path);

    /**
     * Set sleep to the process for given number of milliseconds.
     *
     * @param	milliseconds	the number of milliseconds to sleep
     */
    static void sleep(unsigned int milliseconds);

    /**
     * Composes a string with the same text that would be printed.
     * After the format parameter, the function expects at least as many additional arguments as specified by format.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     *
     * @return	formatted data in string format
     */
    template <typename... Args>
    static std::string str_format(const char* format, Args&&... args) {
        std::array<char, MAX_LEN_FMT_BUFFER> format_buffer{0};
        int ret = snprintf(format_buffer.begin(), MAX_LEN_FMT_BUFFER, format, std::forward<Args>(args)...);

        std::string result;
        if (ret != -1) {
            result = std::string(format_buffer.begin());
        }

        return result;
    }
};

class Logger;

/**
 * @class Logger_Worker
 *
 * @brief Worker class which manages logging threads and all logging operations.
 */
class Logger_Worker {
    friend Logger;

    public:
    Logger_Worker();
    ~Logger_Worker();

    Logger_Worker(const Logger_Worker&) = delete;
    Logger_Worker(Logger_Worker&&) = delete;
    Logger_Worker& operator=(const Logger_Worker&) = delete;
    Logger_Worker& operator=(Logger_Worker&&) = delete;

    /**
     * Initialize logger threads and sets the log file names.
     *
     * @param	log_file_path	Pointer to the application log file name
     *
     * @throw	The logger exception with exception details.
     */
    void initialize(std::string& log_file_path);

    /**
     * Receives the log record to write, and pushes the record to the application log queue.
     *
     * @param	level		The log severity level
     * @param 	log_record 	Pointer to the log record which is to be add to the queue.
     */
    void output_log_line(Log_Level level, const char* log_record);

    /**
     * Pop log record from application log queue and writes to application log file.
     *
     * @throw	The logger exception with exception details.
     */
    void write_to_log_file();

    /**
     * Release and close all loggers
     *
     * @throw	The logger exception with exception details.
     */
    void drop_all();

    private:
    std::unique_ptr<std::thread> m_app_log_thread;
    volatile bool m_is_app_interrupted;

    Log_Level m_severity_level;

    Blocking_string_Queue log_queue;
    std::mutex m_mutex_log_queue;

    volatile bool m_file_log_enabled;
    volatile bool m_console_log_enabled;

    std::string m_log_file_path;
    std::ofstream m_log_file_stream;
    std::mutex m_mutex_log_file;
};

/**
 * @class Logger
 *
 * @brief Utility class which provides the interfaces to write application logs.
 */
class Logger {
    public:
    Logger();
    ~Logger();

    Logger(const Logger&) = default;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = default;
    Logger& operator=(Logger&&) = delete;

    /**
     * Validate the log files and initialize logger worker process.
     * Set log file path to default if no values provided from calling module.
     *
     * @param	log_file_path	Pointer to the application log file name
     *
     * @throw	The logger exception with exception details.
     */
    static void initialize(const std::string& log_file_path);

    /**
     * Set the log severity level (determines minimum log level).
     *
     * @param	level	the log severity level
     */
    static void set_log_severity_level(Log_Level level);

    /**
     * Enable/disable application logging to file.
     *
     * @param	value	the parameter to enable or disable logging to file.
     */
    static void enable_file_logging(bool value);

    /**
     * Enable/disable application logging to console.
     *
     * @param	value	the parameter to enable or disable logging to console.
     */
    static void enable_console_logging(bool value);

    /**
     * Write debug level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void debug(const char* format, Args&&... args) {
        debug(LOG_CODE_DEBUG_DEFAULT, format, std::forward<Args>(args)...);
    }
    /**
     * Write debug level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void debug(unsigned long code, const char* format, Args&&... args) {
        write_log(Log_Level::DEBUG, code, format, std::forward<Args>(args)...);
    }

    /**
     * Write info level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void info(const char* format, Args&&... args) {
        info(LOG_CODE_INFO_DEFAULT, format, std::forward<Args>(args)...);
    }
    /**
     * Write info level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void info(unsigned long code, const char* format, Args&&... args) {
        write_log(Log_Level::INFO, code, format, std::forward<Args>(args)...);
    }

    /**
     * Write notice level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void notice(const char* format, Args&&... args) {
        notice(LOG_CODE_NOTICE_DEFAULT, format, std::forward<Args>(args)...);
    }
    /**
     * Write notice level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void notice(unsigned long code, const char* format, Args&&... args) {
        write_log(Log_Level::NOTICE, code, format, std::forward<Args>(args)...);
    }

    /**
     * Write warning level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void warn(const char* format, Args&&... args) {
        warn(LOG_CODE_WARN_DEFAULT, format, std::forward<Args>(args)...);
    }
    /**
     * Write warning level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void warn(unsigned long code, const char* format, Args&&... args) {
        write_log(Log_Level::WARNING, code, format, std::forward<Args>(args)...);
    }

    /**
     * Write error level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void error(const char* format, Args&&... args) {
        error(LOG_CODE_ERROR_DEFAULT, format, std::forward<Args>(args)...);
    }
    /**
     * Write error level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void error(unsigned long code, const char* format, Args&&... args) {
        write_log(Log_Level::ERROR, code, format, std::forward<Args>(args)...);
    }

    /**
     * Write critical level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void critical(const char* format, Args&&... args) {
        critical(LOG_CODE_CRIT_DEFAULT, format, std::forward<Args>(args)...);
    }
    /**
     * Write critical level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    template <typename... Args>
    static void critical(unsigned long code, const char* format, Args&&... args) {
        write_log(Log_Level::CRITICAL, code, format, std::forward<Args>(args)...);
    }

    /**
     * Release and close all loggers
     *
     * @throw	The logger exception with exception details.
     */
    static void drop_all();

    private:
    /**
     * Write the formatted log record to the respective log queue.
     *
     * @param	level	The log severity level
     * @param	code	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	args	The variable argument list (va_list)
     */
    template <typename... Args>
    static void write_log(Log_Level level, unsigned long code, const char* format, Args&&... args) {
        if (level < s_worker.m_severity_level) {
            return;
        }

        std::array<char, MAX_LEN_DATE_BUFFER> time{0};
        Logger_Util::get_time_string(time.begin());

        std::array<char, MAX_LEN_FMT_BUFFER> format_buffer{0};
        int ret = snprintf(format_buffer.begin(), MAX_LEN_FMT_BUFFER, format, std::forward<Args>(args)...);

        auto log_level = std::string(magic_enum::enum_name(level));

        // yyyy-MM-dd HH:mm:ss.SSS [LEVEL ](code): Message
        std::array<char, MAX_LEN_STR_BUFFER> string_buffer{0};
        // NOLINTNEXTLINE
        ret = snprintf(string_buffer.begin(), MAX_LEN_STR_BUFFER, "%s [%-6s](%04lu): %s", time, log_level.c_str(), code,
                       format_buffer);

        if (ret > 0) {
            s_worker.output_log_line(level, string_buffer.begin());
        }
    }

    static Logger_Worker s_worker;   // NOLINT
};

}   // namespace yuki
