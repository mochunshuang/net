

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <array>
#include <cassert>
#include <span>
#include <string_view>
#include <utility>

// NOLINTBEGIN
using OCTET = mcs::ABNF::OCTET;

#include <array>
#include <cstdint>
#include <span>

using OCTET = std::uint8_t;

#include <array>
#include <cstdint>
#include <span>

using OCTET = std::uint8_t;

constexpr auto test_span(mcs::ABNF::default_span_t s)
{
    return s.size() > 0;
}

constexpr auto test_span_size(mcs::ABNF::default_span_t s)
{
    return s.size();
}

int main()
{
    using namespace mcs::ABNF::URI; // NOLINT
    constexpr auto v = make_array("123");
    // std::span<const OCTET> sp(v);
    constexpr auto v2 = test_span(make_array("123"));
    static_assert(test_span(make_array("123")));

    static_assert(test_span_size(make_array("123")) == 3);

    return 0;
}

// NOLINTEND