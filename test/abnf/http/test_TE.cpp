#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // TE = [ t-codings *( OWS "," OWS t-codings ) ]
    constexpr auto te_pass = make_pass_test<mcs::abnf::http::TE>();
    constexpr auto te_fail = make_unpass_test<mcs::abnf::http::TE>();

    static_assert(te_pass("trailers"_span));            // 标准值
    static_assert(te_pass("compress, chunked"_span));   // 多编码
    static_assert(te_pass("gzip, deflate;q=0.5"_span)); // 非法参数
    {
        static_assert(make_pass_test<mcs::abnf::http::t_codings>()("deflate;q=0.5"_span));
    }

    static_assert(te_fail("trailers, "_span)); // 空尾项

    return 0;
}
// NOLINTEND