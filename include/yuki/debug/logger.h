// A simple thread-safe application logging class.
// Supports logging to the console and to a file.
// Original by: https://github.com/prajilal/cpplogger

#pragma once

#include <cstdarg>
#include <fstream>
#include <io.h>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#define MAX_LEN_FMT_BUFFER  2048
#define MAX_LEN_STR_BUFFER  2080
#define MAX_LEN_DATE_BUFFER 32
#define SLEEP_IN_MS         100

#define LOG_PATH_DEFAULT "logs/app.log"

// Default log/error codes
#define LOG_CODE_INFO_DEFAULT  0001
#define LOG_CODE_EVENT_DEFAULT 0001
#define LOG_CODE_DEBUG_DEFAULT 0001
#define LOG_CODE_WARN_DEFAULT  0001
#define LOG_CODE_ERR_DEFAULT   0001
#define LOG_CODE_CRIT_DEFAULT  0001

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

    /*! Sets the logger exception type & message in C-wstring format.
     *
     * @param	type	    The logger exception type.
     * @param	message		The logger exception message in C-wstring format.
     */
    Logger_Exception(Log_Exception_Type type, const std::wstring& message) : m_type(type) {
        std::string str_message(message.begin(), message.end());
        this->m_message = str_message;
    };

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
 * @class BlockingWStringQueue
 *
 * @brief Utility queue class which performs wstring pop/push operation.
 */
class Blocking_WString_Queue {
    public:
    /**
     * Pop elements from queue.
     *
     * @param	result	Pointer to a wstring where the resulting C-wstring is stored.
     *
     * @return 	true is returned in the case that an item popped from the queue.
     *			Otherwise, false is returned.
     */
    bool pop(std::wstring& result) {
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
     * @param	value		The C-wstring element that requires insertion.
     */
    void push(const std::wstring& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(value);
    };

    private:
    std::mutex m_mutex;
    std::queue<std::wstring> m_queue;
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
     * @return 	true is returned in the case that the file exist.
     *			Otherwise, false is returned.
     */
    static bool file_exists(const std::string& file_path);

    /**
     * Write time stamp in 'yyyy-MM-dd HH:mm:ss.SSS' format to string.
     *
     * @param	buffer	Pointer to a buffer where the resulting C-wchar_t is stored.
     *					The buffer should be large enough to contain the resulting string.
     */
    static void get_time_string(wchar_t* buffer);

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
     * Composes a wstring with the same text that would be printed.
     * After the format parameter, the function expects at least as many additional arguments as specified by format.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     *
     * @return	formatted data in wstring format
     */
    static std::wstring str_format(const wchar_t* format, ...);
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
    void output_log_line(Log_Level level, const wchar_t* log_record);

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

    Blocking_WString_Queue log_queue;
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
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void debug(const wchar_t* format, ...);
    /**
     * Write debug level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void debug(unsigned long code, const wchar_t* format, ...);

    /**
     * Write information level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void info(const wchar_t* format, ...);
    /**
     * Write info level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void info(unsigned long code, const wchar_t* format, ...);

    /**
     * Write notice level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void notice(const wchar_t* format, ...);
    /**
     * Write notice level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void notice(unsigned long code, const wchar_t* format, ...);

    /**
     * Write warning level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void warn(const wchar_t* format, ...);
    /**
     * Write warning level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void warn(unsigned long code, const wchar_t* format, ...);

    /**
     * Write error level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void error(const wchar_t* format, ...);
    /**
     * Write error level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void error(unsigned long code, const wchar_t* format, ...);

    /**
     * Write critical level log records to the application log file.
     *
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void critical(const wchar_t* format, ...);
    /**
     * Write critical level log record to the application log file.
     *
     * @param 	code 	The 5 digit custom defined code to each record.
     * @param	format	C string that contains a format string that follows the same specifications as format in
     *					printf (see printf for details)
     * @param	...		Depending on the format string, the function may expect a sequence of additional arguments,
     *                  each containing a value to be used to replace a format specifier in the format string.
     */
    static void critical(unsigned long code, const wchar_t* format, ...);

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
    static void write_log(Log_Level level, unsigned long code, const wchar_t* format, va_list args);

    static Logger_Worker s_worker; // NOLINT
};

}   // namespace yuki
