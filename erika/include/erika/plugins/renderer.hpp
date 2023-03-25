#pragma once

#include "erika/plugins/plugin_types.hpp"

namespace yuki::platform {
struct Platform_State;
}

namespace erika::plugins {

/**
 * @class Renderer
 *
 * @brief A base class for all renderer plugins. Any plugin which provides rendering capabilities, such as DirectX11, Vulkan,
 * OpenGL etc must inherit from this base class.
 */
class Renderer : public Plugin {
  public:
    struct Clear_Colour {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    static constexpr Clear_Colour Default_Clear_Colour = { 0.79f, 0.94f, 0.7f, 1.0f };

    explicit Renderer();
    ~Renderer() override = default;

    void set_screen_size(const u32 width, const u32 height);

    virtual void initialize(const std::shared_ptr<yuki::platform::Platform_State>& platform_state) = 0;

    /**
     * @brief Begin drawing a new scene.
     */
    virtual void begin_scene(const Clear_Colour& clear_color = Default_Clear_Colour) = 0;
    /**
     * @brief Finish drawing the current scene.
     */
    virtual void end_scene() = 0;

    Renderer(const Renderer&) = default;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = default;
    Renderer& operator=(Renderer&&) = delete;

  protected:
    [[nodiscard]] u32 get_screen_width() const;
    [[nodiscard]] u32 get_screen_height() const;

    virtual void on_resize(const u32 width, const u32 height) = 0;

  private:
    u32 m_screen_width;
    u32 m_screen_height;
};

/**
 * @class Renderer_Plugin_Factory
 *
 * @brief A base class for any erika renderer plugin factories. Any plugin which provides rendering capabilities, such as
 * DirectX11, Vulkan, OpenGL etc must have a respective Renderer_Plugin_Factory for their type.
 */
using Renderer_Plugin_Factory = erika::plugins::Plugin_Factory<Renderer>;

}
