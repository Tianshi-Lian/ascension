#include "plugins/plugin_types.hpp"

namespace erika::plugins {

Plugin::Plugin(Plugin_Type type, std::string name)
  : m_type(type)
  , m_name(std::move(name))
{
}

}
