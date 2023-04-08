#pragma once

namespace yuki {
struct Platform_State;
}

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
     * @param     platform_state      a shared pointer to the current platform state
     * @param     game      a shared pointer to the game to execute
     */
    static void run(const std::shared_ptr<yuki::Platform_State>& platform_state, const std::shared_ptr<Game>& game);
};

}
