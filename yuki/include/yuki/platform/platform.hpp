/**
 * File: platform.hpp
 * Project: yuki
 * File Created: 2023-03-12 21:20:07
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:05:13
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

#include <functional>
#include <memory>

#include "platform_types.hpp"

namespace yuki {

/**
 * @class Platform
 *
 * @brief A class which describes a platform layer for the applications. Each supported platform will
 * need to implement the functions in this class in a corresponding .cpp file.
 */
class Platform {
public:
    /**
     * @brief Initialize the platform layer.
     *
     * @param       platform_state      a pointer to the current platform state
     * @param       application_name    name of the application being ran
     * @param       pos_x               starting x position of the application window
     * @param       pos_y               starting x position of the application window
     * @param       width               starting width of the application window
     * @param       height              starting height of the application window
     *
     * @return      true if initialization was successful, else false
     */
    static bool initialize(
        const std::shared_ptr<Platform_State>& platform_state,
        const std::string& application_name,
        u32 pos_x,
        u32 pos_y,
        u32 width,
        u32 height
    );

    /**
     * @brief Initialize a the platform state.
     *        Platform::initialize() will call this on it's own if internal_state is nullptr.
     *        This allows users to initialize state and use platform functions without a full platform layer.
     *
     * @param       platform_state      a pointer to the current platform state
     */
    static void initialize_state(const std::shared_ptr<Platform_State>& platform_state);

    /**
     * @brief Shutdown the application layer.
     *
     * @param       platform_state      a pointer to the current platform state
     */
    static void shutdown(const std::shared_ptr<Platform_State>& platform_state);
    /**
     * @brief Process any platform-specific messages.
     *
     * @param       platform_state      a pointer to the current platform state
     *
     * @return      true if the application should continue running, else false
     */
    static bool process_messages(const std::shared_ptr<Platform_State>& platform_state);

    /**
     * @brief Get the time in ms since the application started
     */
    static f64 get_platform_time(const std::shared_ptr<Platform_State>& platform_state);

    /**
     * @brief Sleep the active thread for the specific time.
     *
     * @param       milliseconds      then number of milliseconds to sleep for
     */
    static void sleep(u32 milliseconds);

    /**
     * @brief Load a shared library from disk.
     *
     * @param     filepath      filepath to the library to load
     *
     * @return    handle to the loaded library, if successful then internal_state
     * will not be a nullptr
     *
     */
    static Library_Handle load_shared_library(const std::string& filepath);
    /**
     * @brief Free a loaded shared library.
     *
     * @param     library_handle      reference to a loaded library handle to free
     */
    static void free_shared_library(const Library_Handle& library_handle);

    /**
     * @brief Load an exported function from a loaded library.
     *
     * @param     library_handle      reference to a loaded library to load from
     * @param     function_name       name of the function to attempt to load
     *
     * @return    function<T> holding the loaded function, else nullptr
     */
    template<typename T>
    static std::function<T> load_library_function(const Library_Handle& library_handle, const std::string& function_name)
    {
        const auto function = load_library_function_internal(library_handle, function_name);
        if (function == nullptr) {
            return nullptr;
        }

        std::function<T> result;
        if (function != nullptr) {
            // NOLINTNEXTLINE - we're playing with generic-ing functions to and from system types this is unfortunately needed.
            result = reinterpret_cast<T*>(*function);
        }

        return result;
    }

private:
    static std::shared_ptr<void (*)()>
    load_library_function_internal(const Library_Handle& library_handle, const std::string& function_name);
};

}
