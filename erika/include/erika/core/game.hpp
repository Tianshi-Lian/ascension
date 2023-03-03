#pragma once

namespace erika {

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
    virtual bool on_initialize() = 0;
    virtual void on_update(f32 delta_time) = 0;
    virtual void on_render(f32 delta_time) = 0;

  private:
    bool initialize();
    void update(f32 delta_time);
    void render(f32 delta_time);
};

}
