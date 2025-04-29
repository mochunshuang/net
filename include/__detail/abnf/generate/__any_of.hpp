#pragma once

#include "./__char.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::generate
{
    template <detail::octet... C>
    using any_of = operators::alternative<CharSensitive<C>...>;

}; // namespace mcs::abnf::generate