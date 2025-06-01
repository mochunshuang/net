#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // chunk-ext-name = token
    constexpr auto chunk_ext_name_pass = make_pass_test<chunk_ext_name>();
    constexpr auto chunk_ext_name_fail = make_unpass_test<chunk_ext_name>();
    static_assert(chunk_ext_name_pass("abc"_span));
    static_assert(!chunk_ext_name_fail("\r"_span));
    static_assert(!chunk_ext_name_fail("\n"_span));
    static_assert(!chunk_ext_name_fail("\r\n"_span));

    static_assert(!chunk_ext_name_fail("\"\""_span));

    return 0;
}
// NOLINTEND