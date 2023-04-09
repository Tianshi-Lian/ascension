/**
 * File: qilian.hpp
 * Project: qilian
 * File Created: 2023-04-04 18:39:29
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:06:46
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

#include "erika/core/game.hpp"

namespace qilian {

class Qilian : public erika::Game {
  public:
    Qilian();

    bool on_initialize() override;
    void on_update(f32 delta_time) override;
    void on_render(f32 interpolation) override;

  private:
};

}
