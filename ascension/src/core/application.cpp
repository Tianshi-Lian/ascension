#include "core/application.hpp"

#include <GL/glew.h>
#include <SDL.h>

#include <yuki/debug/logger.hpp>

namespace ascension::core {

constexpr static int OpenGL_MAJOR = 4;
constexpr static int OpenGL_MINOR = 3;

using namespace yuki::debug;

Application::Application()
  : m_should_quit(false)
  , m_window_title("Application")
  , m_window_position(0, 0)
  , m_window_size(0, 0)
{
}

Application::~Application()
{
    SDL_GL_DeleteContext(m_gl_context.get());
    m_gl_context.reset();
}

void
Application::setup(const std::string& app_name, glm::ivec2 window_position, glm::ivec2 window_size)
{
    m_window_title = app_name;
    m_window_position = window_position;
    m_window_size = window_size;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0) {
        Logger::critical("Ascension", "Failed to initialise SDL! Error {}", SDL_GetError());
        return;
    }

    const auto window = std::shared_ptr<SDL_Window>(
        SDL_CreateWindow(
            m_window_title.c_str(),
            m_window_position.x,
            m_window_position.y,
            m_window_size.x,
            m_window_size.y,
            SDL_WINDOW_OPENGL
        ),
        SDL_DestroyWindow
    );

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OpenGL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OpenGL_MINOR);

    const auto gl_context = std::make_shared<SDL_GLContext>(SDL_GL_CreateContext(window.get()));

    if (glewInit() != GLEW_OK) {
        Logger::critical("Ascension", "Failed to initialise glew! Error {}", SDL_GetError());
        return;
    }

    m_window = std::static_pointer_cast<void>(window);
    m_gl_context = std::static_pointer_cast<void>(gl_context);

    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
}

i32
Application::run()
{
    auto window = std::static_pointer_cast<SDL_Window>(m_window);

    while (!m_should_quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_WINDOWEVENT: {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        quit();
                    }
                } break;
                case SDL_QUIT: {
                    quit();
                } break;
            }
        }

        update(1.0);

        glClear(GL_COLOR_BUFFER_BIT);

        render(1.0f);

        SDL_GL_SwapWindow(window.get());
    }

    return 0;
}

void
Application::quit()
{
    m_should_quit = true;
}

void
Application::initialize()
{
}

void
Application::update(f64 delta_time)
{
    (void)delta_time;
}

void
Application::render(f32 interpolation)
{
    (void)interpolation;
}

}
