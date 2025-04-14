#pragma once

#include <concepts>

#include "./__is_expected_specialization.hpp"

namespace mcs::abnf::__detail
{

    template <typename R>
    concept abnf_result = is_expected_specialization<R> || std::same_as<R, bool>;

}; // namespace mcs::abnf::__detail