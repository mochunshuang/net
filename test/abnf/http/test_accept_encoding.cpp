#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Accept-Encoding = [ ( codings [ weight ] ) *( OWS "," OWS ( codings [ weight ] ) )
    // ]
    constexpr auto accept_encoding_pass =
        make_pass_test<mcs::abnf::http::Accept_Encoding>();
    constexpr auto accept_encoding_fail =
        make_unpass_test<mcs::abnf::http::Accept_Encoding>();
    static_assert(accept_encoding_pass("gzip, deflate;q=0.5"_span)); // 标准编码
    static_assert(accept_encoding_pass("identity;q=0"_span));        // 禁用编码

    static_assert(accept_encoding_fail("compress;level=5"_span)); // 非法参数

    return 0;
}
// NOLINTEND