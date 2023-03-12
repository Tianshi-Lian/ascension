#include "erika/core/entry.hpp" // IWYU pragma: keep

#include "ascension.hpp"

std::shared_ptr<erika::Game>
erika::game::create()
{
    return std::make_shared<ascension::Ascension>();
}
