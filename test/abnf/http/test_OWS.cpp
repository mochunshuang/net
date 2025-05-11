#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto OWS = make_pass_test<mcs::abnf::http::OWS>();
    static_assert(OWS(""_span));
    static_assert(OWS(" "_span));
    static_assert(OWS("                 "_span));
    static_assert(OWS("                                                       "_span));
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND