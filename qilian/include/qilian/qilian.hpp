#pragma once

#include "erika/core/game.hpp"

namespace qilian {

class Qilian : public erika::Game {
  public:
    Qilian();

    bool on_initialize() override;
    void on_update(f32 delta_time) override;
    void on_render(f32 delta_time) override;
};

}
