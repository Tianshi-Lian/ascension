#pragma once

#include <memory>

namespace erika {

class Game;

class Engine final {
  public:
    static void initialize();
    static void run(const std::shared_ptr<Game>& game);
};

}
