#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto RWS_rule = make_pass_test<RWS>();
    static_assert(not RWS_rule(""_span));
    static_assert(RWS_rule(" "_span));
    static_assert(RWS_rule("                 "_span));
    static_assert(
        RWS_rule("                                                       "_span));
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND