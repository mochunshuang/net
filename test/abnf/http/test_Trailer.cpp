#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Trailer = [ field-name *( OWS "," OWS field-name ) ]
    constexpr auto trailer_pass = make_pass_test<mcs::abnf::http::Trailer>();
    constexpr auto trailer_fail = make_unpass_test<mcs::abnf::http::Trailer>();

    static_assert(trailer_pass("Content-MD5"_span));          // 单个字段
    static_assert(trailer_pass("X-Trace-ID, X-Secret"_span)); // 多字段
    static_assert(trailer_fail("Invalid Field"_span));        // 含空格
    static_assert(trailer_fail("X-Data, "_span));             // 空字段项

    return 0;
}
// NOLINTEND