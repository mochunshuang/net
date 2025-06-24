#include <functional>
#include <iostream>
#include <vector>

// NOLINTBEGIN

class http_router
{
};

int main()
{
    using T = decltype(+[](int a) noexcept {});

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND