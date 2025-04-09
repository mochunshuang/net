#include <iostream>

#include "./common/iocp.hpp"

int main()
{
    (void)std::signal(SIGINT, WAIT::signal_handler);
    std::cout << "main done\n";
    return 0;
}