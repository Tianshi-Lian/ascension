/**
 * File: input_listener.cpp
 * Project: ascension
 * File Created: 2023-04-30 16:19:36
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-30 16:23:05
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

#include "input/input_listener.hpp"

#include <atomic>

namespace ascension::input {

Input_Listener::Input_Listener()
{
    static std::atomic_uint32_t current_id;
    m_id = current_id++;
}

u32
Input_Listener::get_id() const
{
    return m_id;
}

bool
operator==(const Input_Listener& listener_1, const Input_Listener& listener_2)
{
    return listener_1.m_id == listener_2.m_id;
}

bool
operator!=(const Input_Listener& listener_1, const Input_Listener& listener_2)
{
    return listener_1.m_id != listener_2.m_id;
}

}
