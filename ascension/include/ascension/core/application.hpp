#pragma once

#include <glm/glm.hpp>

namespace ascension::core {

class Application {
  public:
    Application();
    virtual ~Application();

    void setup(const std::string& app_name, glm::ivec2 window_position, glm::ivec2 window_size);
    i32 run();
    void quit();

    Application(const Application&) = default;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = default;
    Application& operator=(Application&&) = delete;

  protected:
    virtual void initialize();
    virtual void update(f64 delta_time);
    virtual void render(f32 interpolation);

  private:
    bool m_should_quit;

    std::string m_window_title;
    glm::ivec2 m_window_position;
    glm::ivec2 m_window_size;

    std::shared_ptr<void> m_window;
    std::shared_ptr<void> m_gl_context;
};

}
