#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
     * grandfathered = irregular           ; non-redundant tags registered
                     / regular             ; during the RFC 3066 era
     *
     */
    constexpr auto grandfathered_pass = make_pass_test<grandfathered>();
    constexpr auto grandfathered_fail = make_unpass_test<grandfathered>();

    // irregular
    static_assert(grandfathered_pass("i-ami"_span));
    static_assert(grandfathered_pass("i-bnn"_span));
    static_assert(grandfathered_pass("i-default"_span));
    static_assert(grandfathered_pass("i-enochian"_span));
    static_assert(grandfathered_pass("i-hak"_span));
    static_assert(grandfathered_pass("i-klingon"_span));

    // regular
    static_assert(grandfathered_pass("art-lojban"_span));
    static_assert(grandfathered_pass("aRt-lojban"_span));
    static_assert(grandfathered_pass("aRt-LOJBAN"_span));
    static_assert(grandfathered_pass("cel-gaulish"_span));
    static_assert(grandfathered_pass("no-bok"_span));
    static_assert(grandfathered_pass("no-nyn"_span));

    return 0;
}
// NOLINTEND