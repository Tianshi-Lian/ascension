#include "qilian.hpp"

#include "erika/core/entry.hpp" // IWYU pragma: keep

#include "erika/plugins/renderer.hpp"

namespace qilian {

Qilian::Qilian()
  : erika::Game("Qilian")
{
}

bool
Qilian::on_initialize()
{
    PROFILE_FUNCTION();
    yuki::debug::Logger::debug("qilian", "Qilian::on_initialize()");

    m_plugin_manager.change_active_renderer("OpenGL_Renderer");

    return true;
}

void
Qilian::on_update(f32 delta_time)
{
    (void)delta_time;
}

void
Qilian::on_render(f32 interpolation)
{
    (void)interpolation;

    auto renderer = m_plugin_manager.get_active_renderer();
    renderer->begin_scene();

    renderer->end_scene();
}

}

std::shared_ptr<erika::Game>
erika::game::create()
{
    return std::make_shared<qilian::Qilian>();
}
