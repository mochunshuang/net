#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Content-Encoding = [ content-coding *( OWS "," OWS content-coding ) ]
    constexpr auto content_enc_pass = make_pass_test<mcs::abnf::http::content_Encoding>();
    constexpr auto content_enc_fail =
        make_unpass_test<mcs::abnf::http::content_Encoding>();
    static_assert(content_enc_pass("gzip"_span));
    static_assert(content_enc_pass("br, deflate"_span)); // 多编码
    static_assert(content_enc_fail("gzip;q=0.8"_span));  // 非法参数

    return 0;
}
// NOLINTEND