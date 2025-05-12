#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Accept-Ranges = acceptable-ranges （复用已有测试）
    // acceptable-ranges = range-unit *( OWS "," OWS range-unit )
    constexpr auto accept_ranges_pass = make_pass_test<mcs::abnf::http::Accept_Ranges>();
    static_assert(accept_ranges_pass("bytes"_span)); // 复用前例

    return 0;
}
// NOLINTEND