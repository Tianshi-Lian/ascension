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
    explicit Renderer()
      : Plugin(Plugin_Type::Renderer)
    {
    }

    virtual void initialize(const std::shared_ptr<yuki::platform::Platform_State>& platform_state) = 0;

    ~Renderer() override = default;

    /**
     * @brief Begin drawing a new scene.
     */
    virtual void begin_scene() = 0;
    /**
     * @brief Finish drawing the current scene.
     */
    virtual void end_scene() = 0;

    Renderer(const Renderer&) = default;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = default;
    Renderer& operator=(Renderer&&) = delete;
};

/**
 * @class Renderer_Plugin_Factory
 *
 * @brief A base class for any erika renderer plugin factories. Any plugin which provides rendering capabilities, such as
 * DirectX11, Vulkan, OpenGL etc must have a respective Renderer_Plugin_Factory for their type.
 */
using Renderer_Plugin_Factory = erika::plugins::Plugin_Factory<Renderer>;

}
