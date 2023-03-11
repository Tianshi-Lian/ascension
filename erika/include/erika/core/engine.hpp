#pragma once

namespace erika {

class Game;

/**
 * @class Engine
 *
 * @brief Core engine class for the erika game engine.
 */
class Engine final {
  public:
    /**
     * @brief Initialize the game engine.
     */
    static void initialize();
    /**
     * @brief Start running a game on top of the game engine.
     *
     * @param     game      a shared pointer to the game to execute
     */
    static void run(const std::shared_ptr<Game>& game);
};

}
