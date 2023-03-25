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
    /**
     * @struct Clear_Color
     *
     * @brief A struct for storing the RGBA colour used for clearing the back buffer.
     */
    struct Clear_Color {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    static constexpr Clear_Color Default_Clear_Color = { 0.79f, 0.94f, 0.7f, 1.0f };

    explicit Renderer();
    ~Renderer() override = default;

    /**
     * @brief Set the screen size the renderer should draw to.
     *
     * @param   width     the width of the screen
     * @param   height    the height of the screen
     */
    void set_screen_size(const u32 width, const u32 height);

    /**
     * @brief Initialize the renderer plugin.
     *
     * @param     platform_state      a shared pointer to the OS platform_state
     */
    virtual void initialize(const std::shared_ptr<yuki::platform::Platform_State>& platform_state) = 0;

    /**
     * @brief Begin drawing a new scene.
     *
     * @param     clear_color     an RGBA color to clear the back buffer with before rendering starts
     */
    virtual void begin_scene(const Clear_Color& clear_color = Default_Clear_Color) = 0;
    /**
     * @brief Finish drawing the current scene.
     */
    virtual void end_scene() = 0;

    Renderer(const Renderer&) = default;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = default;
    Renderer& operator=(Renderer&&) = delete;

  protected:
    /**
     * @brief Get the width of the screen.
     *
     * @return    u32     the current screen width
     */
    [[nodiscard]] u32 get_screen_width() const;
    /**
     * @brief Get the height of the screen.
     *
     * @return    u32     the current screen height
     */
    [[nodiscard]] u32 get_screen_height() const;

    /**
     * @brief Callback for when the renderer is due to be resized.
     * m_screen_width and m_screen_height will be updated after this call and therefore can be used to check the previous size.
     *
     * @param     width     the new width of the screen
     * @param     height    the new height of the screen
     */
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
