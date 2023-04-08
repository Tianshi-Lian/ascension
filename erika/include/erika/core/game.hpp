#pragma once

#include "erika/plugins/plugin_manager.hpp"

namespace yuki {
struct Platform_State;
}

namespace erika {

/**
 * @class Game
 *
 * @brief A base class for all games which run on top of the erika engine.
 */
class Game {
    friend class Engine;

  public:
    explicit Game(std::string window_title = "Erika Game");
    virtual ~Game() = default;

    [[nodiscard]] const std::string& get_window_title() const;

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
     * @param     interpolation    interpolation value between 0 and 1 relative to current update frames progress
     */
    virtual void on_render(f32 interpolation) = 0;

    /**
     * @brief Set the window title for the games application.
     *
     * @param     value     string holding the value to set the title to
     */
    void set_window_title(std::string value);

    /**
     * @brief The created platform state which can be used to access platform-specific methods.
     */
    std::shared_ptr<yuki::Platform_State> m_platform_state;

    erika::plugins::Plugin_Manager m_plugin_manager;

  private:
    bool initialize();
    void update(f32 delta_time);
    void render(f32 interpolation);

    void set_platform_state(const std::shared_ptr<yuki::Platform_State>& platform_state);

    std::string m_window_title;
};

namespace game {

std::shared_ptr<Game>
create();

}

}
