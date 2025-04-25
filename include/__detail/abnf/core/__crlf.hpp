#pragma once

#include "./__cr.hpp"
#include "./__lf.hpp"
#include "../operators/__sequence.hpp"

namespace mcs::abnf::core
{
    // CRLF           =  CR LF
    using CRLF = operators::sequence<CR, LF>;
    inline constexpr CRLF crlf{}; // NOLINT
}; // namespace mcs::abnf::core