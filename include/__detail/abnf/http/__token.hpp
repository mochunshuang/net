#pragma once

#include "./__tchar.hpp"

namespace mcs::abnf::http
{
    // token = 1*tchar
    using token = one_or_more<tchar>;
}; // namespace mcs::abnf::http