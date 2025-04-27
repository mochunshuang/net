#pragma once

#include "./__char.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::generate
{
    template <detail::octet... C>
    using any_of = operators::alternative<SensitiveChar<C>...>;

}; // namespace mcs::abnf::generate