#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    static_assert(not RWS(""_span));
    static_assert(RWS(" "_span));
    static_assert(RWS("                 "_span));
    static_assert(RWS("                                                       "_span));
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND