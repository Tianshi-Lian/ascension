/**
 * File: ascension.hpp
 * Project: ascension
 * File Created: 2023-02-24 19:27:40
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-08-07 13:37:42
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

#include "graphics/sprite_batch.hpp"

namespace ascension {

class Ascension : public core::Application {
public:
    Ascension() = default;
    ~Ascension() override = default;

    void on_initialize() override;
    void on_update(f64 delta_time) override;
    void on_render(f32 interpolation) override;

    Ascension(const Ascension&) = delete;
    Ascension(Ascension&&) = delete;
    Ascension& operator=(const Ascension&) = delete;
    Ascension& operator=(Ascension&&) = delete;

private:
    graphics::Sprite_Batch m_sprite_batch;
    graphics::Sprite_Batch m_font_batch;
};

} // namespace ascension
