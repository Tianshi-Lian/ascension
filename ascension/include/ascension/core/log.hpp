/**
 * File: log.hpp
 * Project: ascension
 * File Created: 2023-04-18 19:02:06
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-10-14 16:09:57
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

#include "debug/logger.hpp"

namespace ascension::core::log {

static constexpr const char* SOURCE_NAME = { "ascension" };

template<typename... Args>
static void
debug(const std::string& format, Args&&... args)
{
    ascension::debug::Logger::debug(SOURCE_NAME, format, std::forward<Args>(args)...);
}

template<typename... Args>
static void
info(const std::string& format, Args&&... args)
{
    ascension::debug::Logger::info(SOURCE_NAME, format, std::forward<Args>(args)...);
}

template<typename... Args>
static void
notice(const std::string& format, Args&&... args)
{
    ascension::debug::Logger::notice(SOURCE_NAME, format, std::forward<Args>(args)...);
}

template<typename... Args>
static void
warn(const std::string& format, Args&&... args)
{
    ascension::debug::Logger::warn(SOURCE_NAME, format, std::forward<Args>(args)...);
}

template<typename... Args>
static void
error(const std::string& format, Args&&... args)
{
    ascension::debug::Logger::error(SOURCE_NAME, format, std::forward<Args>(args)...);
}

template<typename... Args>
static void
critical(const std::string& format, Args&&... args)
{
    ascension::debug::Logger::critical(SOURCE_NAME, format, std::forward<Args>(args)...);
}

}
