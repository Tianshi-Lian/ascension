#include "plugins/renderer.hpp"

namespace erika::plugins {

Renderer::Renderer()
  : Plugin(Plugin_Type::Renderer)
  , m_screen_width(0)
  , m_screen_height(0)
{
}

void
Renderer::set_screen_size(const u32 width, const u32 height)
{
    on_resize(m_screen_width, m_screen_height);

    m_screen_width = width;
    m_screen_height = height;
}

u32
Renderer::get_screen_width() const
{
    return m_screen_width;
}

u32
Renderer::get_screen_height() const
{
    return m_screen_height;
}

}
