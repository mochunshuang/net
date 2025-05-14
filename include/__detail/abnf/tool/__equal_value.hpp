

#include "../__detail/__types.hpp"
#include <string_view>

namespace mcs::abnf::tool
{
    constexpr auto equal_value(__detail::span_param_in a,
                               const std::string_view &b) noexcept
    {
        const auto k_size = a.size();
        if (k_size != b.size())
            return false;
        for (std::size_t i = 0; i < k_size; ++i)
        {
            if (a[i] != static_cast<__detail::OCTET>(b[i]))
                return false;
        }
        return true;
    }

}; // namespace mcs::abnf::tool
