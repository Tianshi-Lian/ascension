#include "erika/core/entry.hpp" // IWYU pragma: keep

#include "qilian.hpp"

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

    return true;
}

void
Qilian::on_update(f32 delta_time)
{
    PROFILE_FUNCTION();
    // yuki::debug::Logger::debug("qilian > Qilian::on_update(%f)", delta_time);

    (void)delta_time;
}

void
Qilian::on_render(f32 delta_time)
{
    PROFILE_FUNCTION();
    // yuki::debug::Logger::debug("qilian > Qilian::on_render(%f)", delta_time);

    (void)delta_time;
}

}

std::shared_ptr<erika::Game>
erika::game::create()
{
    return std::make_shared<qilian::Qilian>();
}
