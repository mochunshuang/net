#include <iostream>

int main()
{
    // 定义lambda（显式参数在前，可推导参数在后）
    auto print_types = []<typename T, typename U>(U value) {
        std::cout << "Explicit type: " << typeid(T).name()
                  << "\nDeduced type: " << typeid(U).name() << "\nValue: " << value
                  << std::endl;
    };

    // 使用时显式指定第一个参数，第二个参数通过42推导
    print_types.template operator()<int>(42);         // T=int, U=int（42推导）
    print_types.template operator()<double>("hello"); // T=double, U=const char*
    std::cout << "main done\n";
    return 0;
}