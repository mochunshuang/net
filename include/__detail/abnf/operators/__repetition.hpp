#pragma once

#include "./__operate_result.hpp"

namespace mcs::abnf::operators
{

    template <typename Rule, std::size_t Min = detail::k_min_size_value,
              std::size_t Max = detail::k_max_size_value>
        requires(Min <= Max)
    inline constexpr auto repetition = [](detail::octets_view_in sp) noexcept { // NOLINT
        std::size_t count = 0;
        auto remain = sp;
        while (count <= Max)
        {
            auto res = Rule::parse(remain);
            remain = res.remain;
            if (!res)
                break;
            ++count;
        }
        return count >= Min && count <= Max
                   ? operate_result{.success = true, .remaining = remain}
                   : operate_result{.success = false, .remaining = sp};
    };

}; // namespace mcs::abnf::operators