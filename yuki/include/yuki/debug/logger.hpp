/**
 * File: logger.hpp
 * Project: yuki
 * File Created: 2023-02-25 11:46:28
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-14 10:22:44
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

#pragma once

#include <mutex>
#include <queue>
#include <thread>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <magic_enum/magic_enum.hpp>

#define SLEEP_IN_MS 100
#define LOG_PATH_DEFAULT "logs/app.log"
#define DEFAULT_BUFFER_LENGTH 256

namespace yuki::debug {

/**
 * @enum Log_Level
 *
 * @brief Enumerator which defined the different log severity levels.
 * Available log levels are:
 *   MANUAL, DEBUG, INFO, NOTICE, WARNING, ERROR, CRITICAL
 */
enum class Severity {
    LOG_MANUAL = 99,
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_NOTICE = 2,
    LOG_WARNING = 3,
    LOG_ERROR = 4,
    LOG_CRITICAL = 5
};

/**
 * @enum Log_Exception_Type
 *
 * @brief Enumerator which defines the different logger exception types.
 * Available exception types are:
 *   INIT, STREAM, PERMISSION, EXIT
 */
enum class Log_Exception_Type {
    INIT = 1,
    STREAM = 2,
    PERMISSION = 3,
    EXIT = 4,
    INVALID_ARG = 5,
    FORMAT = 6
};

/**
 * @class Logger_Exception
 *
 * @brief Class which implements logger general exception handler.
 */
class Logger_Exception : std::exception {
public:
    /**
     * @brief Sets the logger exception type.
     *
     * @param	type The logger exception type.
     */
    explicit Logger_Exception(Log_Exception_Type type)
      : m_type(type)
    {
    }

    /**
     * @brief Sets the logger exception type & message in string format.
     *
     * @param	type	    The logger exception type.
     * @param	message		The logger exception message in string format.
     */
    Logger_Exception(Log_Exception_Type type, std::string message)
      : m_message(std::move(message))
      , m_type(type)
    {
    }

    /**
     * @brief Retrieves the logger exception type.
     *
     * @return	The logger exception type.
     */
    Log_Exception_Type get_type()
    {
        return m_type;
    }

    /**
     * @brief Retrieves the logger exception message as string.
     *
     * @return	std::string containing the logger exception message.
     */
    std::string get_message()
    {
        return m_message;
    }

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
     * @brief Pop elements from queue.
     *
     * @param	result	Pointer to a string where the resulting C-string is
     *stored.
     *
     * @return 	true is returned in the case that an item popped from the queue.
     *			Otherwise, false is returned.
     */
    bool pop(std::string& result)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        result = m_queue.front();
        m_queue.pop();
        return true;
    }

    /**
     * @brief Push an element to the queue.
     *
     * @param	value		The string element that requires insertion.
     */
    void push(const std::string& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(value);
    }

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
     * @brief Creates a timestamp of the current time in the format "yyyy-MM-dd
     * HH:mm:ss.SSS"
     *
     * @return  std::string containing the formatted timestamp.
     */
    static std::string get_time_string();

    /**
     * @brief Checks whether we have read & write access to the specified file.
     *
     * @param	filepath		The path to the file to validate.
     *
     * @return 	true is returned in the case that proper permissions are set.
     *			Otherwise, false is returned and an error logged.
     */
    static bool has_permissions_for_file(const std::string& filepath);

    /**
     * @brief Set sleep to the process for given number of milliseconds.
     *
     * @param	milliseconds	The number of milliseconds to sleep
     */
    static void sleep(u32 milliseconds);

    /**
     * @brief Composes a string with the same text that would be printed.
     * After the format parameter, the function expects at least as many
     *additional arguments as specified by format.
     *
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a
     *sequence of additional arguments, each containing a value to be used to
     *replace a format specifier in the format string.
     *
     * @return  std::string containing the formatted data.
     */
    template<typename... Args>
    static std::string str_format(const std::string& format, Args&&... args)
    {
        return fmt::format(format, std::forward<Args>(args)...);
    }

private:
    static std::mutex& get_mutex();
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

    /**
     * @brief Initialize logger threads and sets the log file names.
     *
     * @param	log_filepath	Pointer to the application log file name
     *
     * @throw	The logger exception with exception details.
     */
    void initialize(const std::string& log_filepath);

    /**
     * @brief Receives the log record to write, and pushes the record to the
     * application log queue.
     *
     * @param	level		The log severity level
     * @param 	log_record 	Pointer to the log record which is to be add to the
     * queue.
     */
    void output_log_line(Severity level, const std::string& log_record);

    /**
     * @brief Pop log record from application log queue and writes to application log
     * file.
     *
     * @throw	The logger exception with exception details.
     */
    void write_to_log_file();

    /**
     * @brief Release and close all loggers
     *
     * @throw	The logger exception with exception details.
     */
    void drop_all();

    Logger_Worker(const Logger_Worker&) = delete;
    Logger_Worker(Logger_Worker&&) = delete;
    Logger_Worker& operator=(const Logger_Worker&) = delete;
    Logger_Worker& operator=(Logger_Worker&&) = delete;

private:
    std::unique_ptr<std::thread> m_app_log_thread;
    volatile bool m_is_app_interrupted;

    Severity m_severity_level;

    Blocking_string_Queue log_queue;
    std::mutex m_mutex_log_queue;

    volatile bool m_file_log_enabled;
    volatile bool m_console_log_enabled;

    std::string m_log_filepath;
    std::ofstream m_log_file_stream;
    std::mutex m_mutex_log_file;
};

/**
 * @class Logger
 *
 * @brief Utility class which provides the interfaces to write application logs.
 * Original by: https://github.com/prajilal/cpplogger
 */
class Logger {
public:
    Logger();
    ~Logger();

    /**
     * @brief Validate the log files and initialize logger worker process.
     * Set log file path to default if no values provided from calling module.
     *
     * @param	log_filepath	Pointer to the application log file name
     *
     * @throw	The logger exception with exception details.
     */
    static void initialize(
        const std::string& log_filepath,
        Severity level = Severity::LOG_NOTICE,
        bool log_to_file = true,
        bool log_to_console = false
    );

    /**
     * @brief Set the log severity level (determines minimum log level).
     *
     * @param	level	the log severity level
     */
    static void set_log_severity_level(Severity level);

    /**
     * @brief Enable/disable application logging to file.
     *
     * @param	value	the parameter to enable or disable logging to file.
     */
    static void enable_file_logging(bool value);

    /**
     * @brief Enable/disable application logging to console.
     *
     * @param	value	the parameter to enable or disable logging to console.
     */
    static void enable_console_logging(bool value);

    /**
     * @brief Write debug level log record to the application log file.
     *
     * @param   level   The severity level to output this log record at.
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a
     *sequence of additional arguments, each containing a value to be used to
     *replace a format specifier in the format string.
     */
    template<typename... Args>
    static void log(Severity level, const std::string& source, const std::string& format, Args&&... args)
    {
        write_log(level, source, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Write debug level log records to the application log file.
     *
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a
     *sequence of additional arguments, each containing a value to be used to
     *replace a format specifier in the format string.
     */
    template<typename... Args>
    static void debug(const std::string& source, const std::string& format, Args&&... args)
    {
        write_log(Severity::LOG_DEBUG, source, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Write info level log records to the application log file.
     *
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a
     *sequence of additional arguments, each containing a value to be used to
     *replace a format specifier in the format string.
     */
    template<typename... Args>
    static void info(const std::string& source, const std::string& format, Args&&... args)
    {
        write_log(Severity::LOG_INFO, source, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Write notice level log records to the application log file.
     *
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a
     *sequence of additional arguments, each containing a value to be used to
     *replace a format specifier in the format string.
     */
    template<typename... Args>
    static void notice(const std::string& source, const std::string& format, Args&&... args)
    {
        write_log(Severity::LOG_NOTICE, source, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Write warning level log records to the application log file.
     *
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a
     *sequence of additional arguments, each containing a value to be used to
     *replace a format specifier in the format string.
     */
    template<typename... Args>
    static void warn(const std::string& source, const std::string& format, Args&&... args)
    {
        write_log(Severity::LOG_WARNING, source, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Write error level log records to the application log file.
     *
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a
     *sequence of additional arguments, each containing a value to be used to
     *replace a format specifier in the format string.
     */
    template<typename... Args>
    static void error(const std::string& source, const std::string& format, Args&&... args)
    {
        write_log(Severity::LOG_ERROR, source, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Write critical level log records to the application log file.
     *
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	Depending on the format string, the function may expect a
     *sequence of additional arguments, each containing a value to be used to
     *replace a format specifier in the format string.
     */
    template<typename... Args>
    static void critical(const std::string& source, const std::string& format, Args&&... args)
    {
        write_log(Severity::LOG_CRITICAL, source, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Release and close all loggers
     *
     * @throw	The logger exception with exception details.
     */
    static void drop_all();

    Logger(const Logger&) = default;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = default;
    Logger& operator=(Logger&&) = delete;

private:
    /**
     * @brief Get the internal worker object for this logger.
     * Initializes the worker if required.
     *
     * @return   reference to the internal worker instance
     */
    static Logger_Worker& get_worker();

    /**
     * @brief Write the formatted log record to the respective log queue.
     * Standard format: yyyy-MM-dd HH:mm:ss.SSS [LEVEL ](source): Message
     *
     * @param	level	The log severity level
     * @param	format	String that contains a format string that follows the
     *same specifications as format in printf (see printf for details)
     * @param	args	The variable argument list (va_list)
     */
    template<typename... Args>
    static void write_log(Severity level, const std::string& source, const std::string& format, Args&&... args)
    {
        if (level < get_worker().m_severity_level) {
            return;
        }

        std::string formatted_message = fmt::format(format, std::forward<Args>(args)...);

        std::string log_string;
        if (level != Severity::LOG_MANUAL) {
            const auto timestamp = Logger_Util::get_time_string();
            const auto log_level = std::string(magic_enum::enum_name(level)).substr(4);
            log_string = fmt::format("{0} [{1: <8}] ({2}) > {3}", timestamp, log_level, source, formatted_message);
        }
        else {
            log_string = formatted_message;
        }

        if (!log_string.empty()) {
            get_worker().output_log_line(level, log_string);
        }
    }
};

}
