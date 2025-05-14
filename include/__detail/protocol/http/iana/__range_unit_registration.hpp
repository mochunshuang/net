#pragma once

#include <string_view>
#include <array>

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::iana
{
    /**
     * @see
     * https://www.rfc-editor.org/rfc/rfc9110.html#name-range-unit-registration
     */
    struct PortRegistration
    {
        struct RangeUnit
        {
            std::string_view name;
        };
        static constexpr auto registration = // NOLINT
            [] consteval {
                return std::array{RangeUnit{"bytes"}, RangeUnit{"none"}};
            }();

        constexpr static std::optional<RangeUnit> isRegistration(
            abnf::octets_view_in span) noexcept
        {
            for (const auto &item : registration)
            {
                if (abnf::tool::equal_value(span, item.name))
                    return item;
            }
            return std::nullopt;
        }
    };
}; // namespace mcs::protocol::http::iana