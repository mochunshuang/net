#include <cassert>
#include <iostream>
#include <array>

// NOLINTBEGIN

int main()
{
    // 创建源array并初始化
    std::array<int, 5> src = {1, 2, 3, 4, 5};

    // 输出源array的地址信息
    std::cout << "Before move:" << std::endl;
    std::cout << "src address: " << &src << std::endl;
    std::cout << "src[0] address: " << &src[0] << std::endl;

    // 移动构造目标array
    std::array<int, 5> dst = std::move(src);

    // 输出移动后源和目标array的地址信息
    std::cout << "\nAfter move:" << std::endl;
    std::cout << "src address: " << &src << std::endl;
    std::cout << "src[0] address: " << &src[0] << std::endl;
    std::cout << "\ndst address: " << &dst << std::endl;
    std::cout << "dst[0] address: " << &dst[0] << std::endl;

    // NOTE: 对象池的内存块，不允许移动。
    assert(&src[0] != &dst[0]);

    // 验证元素值是否正确移动
    std::cout << "\nElements in dst:" << std::endl;
    for (int i = 0; i < dst.size(); ++i)
    {
        std::cout << "dst[" << i << "]: " << dst[i] << std::endl;
    }

    return 0;
}
// NOLINTEND