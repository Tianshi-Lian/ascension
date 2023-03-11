#pragma once

namespace erika {

/**
 * @class Game
 *
 * @brief A base class for all games which run on top of the erika engine.
 */
class Game {
    friend class Engine;

  public:
    Game() = default;
    virtual ~Game() = default;

    Game(const Game&) = default;
    Game(Game&&) = delete;
    Game& operator=(const Game&) = default;
    Game& operator=(Game&&) = delete;

  protected:
    /**
     * @brief Callback for when the game should run initialization code.
     */
    virtual bool on_initialize() = 0;
    /**
     * @brief Callback for when the game should run its update logic.
     *
     * @param     delta_time    the time elapsed in seconds since the last update
     */
    virtual void on_update(f32 delta_time) = 0;
    /**
     * @brief Callback from when the game should run its render logic.
     *
     * @param     delta_time    the time elapsed in seconds since the last render
     */
    virtual void on_render(f32 delta_time) = 0;

  private:
    bool initialize();
    void update(f32 delta_time);
    void render(f32 delta_time);
};

namespace game {

std::shared_ptr<Game>
create();

}

}
