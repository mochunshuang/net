#include <iostream>

// NOLINTBEGIN

int test(int);

int test1(int a)
{
    if (a == 0)
        return 0;
    test(--a);

    return 0;
}
int test(int a)
{
    if (a == 0)
        return 0;
    return test1(--a);
    return 0;
}

int main()
{
    test(100);
    std::cout << "done\n";
    return 0;
}
// NOLINTEND