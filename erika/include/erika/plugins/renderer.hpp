#pragma once

#include "erika/plugins/plugin_types.hpp"

namespace erika::plugins {

class Renderer : public Plugin {

  public:
    explicit Renderer(std::string name)
      : Plugin(Plugin_Type::Renderer, std::move(name))
    {
    }

    ~Renderer() override = default;

    virtual void begin_scene() = 0;
    virtual void end_scene() = 0;

    Renderer(const Renderer&) = default;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = default;
    Renderer& operator=(Renderer&&) = delete;
};

using Renderer_Plugin_Factory = erika::plugins::Plugin_Factory<Renderer>;

}
