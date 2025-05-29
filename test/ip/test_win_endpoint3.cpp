

// NOLINTBEGIN

#include <iostream>

#if defined(_MSC_VER)

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

int main()
{
    namespace net = mcs::protocol::ip; // NOLINT

    mcs::protocol::ip::tcp::endpoint ep(net::address_v4::any(), 12345);

    std::cout << "main done\n";
    return 0;
}
#else

int main()
{
    std::cout << "main done\n";
    return 0;
}
#endif

// NOLINTEND