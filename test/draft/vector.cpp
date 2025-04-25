#include <cassert>
#include <iostream>
#include <vector>
#include <cstdint>   // for std::uint8_t
#include <algorithm> // for std::generate, std::sort

// NOLINTBEGIN

void test_vector_uint8()
{
    // 1. 基础构造和初始化
    std::vector<std::uint8_t> vec1;                         // 空vector
    std::vector<std::uint8_t> vec2(10, 0xAA);               // 10个元素，每个都是0xAA
    std::vector<std::uint8_t> vec3{0x01, 0x02, 0x03, 0xFF}; // 初始化列表
    assert(vec2.size() == 10);
    assert(vec3.size() == 4);

    // 2. 添加元素
    vec1.push_back(0x10);
    vec1.emplace_back(0x20);
    vec1.insert(vec1.begin() + 1, 0x15);

    // 3. 访问元素
    std::cout << "vec3[2] = " << static_cast<int>(vec3[2])
              << std::endl; // 转为int方便打印
    std::cout << "vec3.at(3) = " << static_cast<int>(vec3.at(3)) << std::endl;

    // 4. 迭代器遍历
    std::cout << "vec3: ";
    for (auto it = vec3.begin(); it != vec3.end(); ++it)
    {
        std::cout << static_cast<int>(*it) << " ";
    }
    std::cout << std::endl;

    // 5. 范围for循环
    std::cout << "vec2: ";
    for (const auto &val : vec2)
    {
        std::cout << static_cast<int>(val) << " ";
    }
    std::cout << std::endl;

    // 6. 容量操作
    std::cout << "vec1 size: " << vec1.size() << ", capacity: " << vec1.capacity()
              << std::endl;
    vec1.shrink_to_fit();
    std::cout << "After shrink_to_fit, capacity: " << vec1.capacity() << std::endl;

    // 7. 算法操作
    std::generate(vec2.begin(), vec2.end(),
                  []() { return static_cast<std::uint8_t>(rand() % 256); });

    std::sort(vec2.begin(), vec2.end());

    // 8. 数据指针访问（常用于C接口）
    std::uint8_t *data = vec2.data();
    size_t size = vec2.size();
    std::cout << "Raw data: ";
    for (size_t i = 0; i < size; ++i)
    {
        std::cout << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::endl;

    // 9. 边界测试
    try
    {
        std::cout << "Attempting to access out of bounds..." << std::endl;
        [[maybe_unused]] auto val = vec3.at(10); // 故意越界
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    // 10. 移动语义测试
    std::vector<std::uint8_t> vec4 = std::move(vec3);
    std::cout << "After move, vec3 size: " << vec3.size() << std::endl;
    std::cout << "vec4 size: " << vec4.size() << std::endl;
}

int main()
{
    test_vector_uint8();
    return 0;
}
// NOLINTEND