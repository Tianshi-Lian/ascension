/**
 * File: ascension.hpp
 * Project: ascension
 * File Created: 2023-02-24 19:27:40
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-13 20:18:15
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

#include "core/application.hpp"

namespace ascension {

class Ascension : public core::Application {
  public:
    Ascension() = default;
    ~Ascension() override = default;

    void on_initialize() override;
    void on_update(f64 delta_time) override;
    void on_render(f32 interpolation) override;

    Ascension(const Ascension&) = default;
    Ascension(Ascension&&) = delete;
    Ascension& operator=(const Ascension&) = default;
    Ascension& operator=(Ascension&&) = delete;
};

} // namespace ascension
