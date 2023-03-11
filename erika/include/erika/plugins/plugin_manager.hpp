#pragma once

#include <vector>

namespace erika::plugins {

class Renderer;
template<typename T>
class Plugin_Factory;

class Plugin_Manager {
  public:
    Plugin_Manager();

    // TODO: initialize() method for loading plugins. See plugins/plugin_types.hpp for more infomation.

    void register_renderer(std::string name, const std::shared_ptr<Plugin_Factory<Renderer>>& factory);
    void change_active_renderer(const std::string& name);

    [[nodiscard]] std::vector<std::string> get_registered_renderers() const;
    [[nodiscard]] std::shared_ptr<Renderer> get_active_renderer() const;

  private:
    // NOTE: We use vector<pairs> here because the vectors should be so small that it will be more efficient
    // than paying the overhead cost of a map. These vectors are likely to also just be all loaded into cache
    // when we do iterate them anyway.
    std::vector<std::pair<std::string, std::shared_ptr<Plugin_Factory<Renderer>>>> m_registered_renderers;
    std::shared_ptr<Renderer> m_active_renderer;
};

}
