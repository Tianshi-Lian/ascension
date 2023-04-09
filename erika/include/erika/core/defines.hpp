/**
 * File: defines.hpp
 * Project: erika
 * File Created: 2023-03-10 21:24:41
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:03:41
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

// Exports
#ifdef ERIKA_EXPORT
#ifdef _MSC_VER
#define ERIKA_API __declspec(dllexport)
#else
#define ERIKA_API __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define ERIKA_API __declspec(dllimport)
#else
#define ERIKA_API
#endif
#endif
