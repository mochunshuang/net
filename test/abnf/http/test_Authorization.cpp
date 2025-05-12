#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Authorization = credentials （复用credentials测试）
    constexpr auto Authorization = make_pass_test<mcs::abnf::http::Authorization>();

    static_assert(Authorization("Bearer token123"_span)); // 复用前例

    return 0;
}
// NOLINTEND