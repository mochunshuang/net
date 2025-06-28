#include <iostream>
#include <vector>
#include <chrono>
#include <new>
// NOLINTBEGIN

constexpr size_t OBJECT_SIZE = 1024;
constexpr size_t OBJECT_COUNT = 1000;
constexpr size_t ITERATIONS = 100;

struct Object
{
    char data[OBJECT_SIZE];
};

void test_aligned()
{
    std::vector<Object *> objects;
    objects.reserve(OBJECT_COUNT);

    // 分配对齐对象
    for (size_t i = 0; i < OBJECT_COUNT; ++i)
    {
        objects.push_back(static_cast<Object *>(
            ::operator new(sizeof(Object), static_cast<std::align_val_t>(OBJECT_SIZE))));
    }

    auto start = std::chrono::high_resolution_clock::now();

    // 模拟访问
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        for (Object *obj : objects)
        {
            for (size_t j = 0; j < OBJECT_SIZE; ++j)
            {
                obj->data[j] = static_cast<char>(j + iter);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Aligned test: " << duration.count() << " μs\n";

    // 释放
    for (Object *obj : objects)
    {
        ::operator delete(obj, static_cast<std::align_val_t>(OBJECT_SIZE));
    }
}

void test_unaligned()
{
    std::vector<Object *> objects;
    objects.reserve(OBJECT_COUNT);

    // 分配非对齐对象
    for (size_t i = 0; i < OBJECT_COUNT; ++i)
    {
        objects.push_back(new Object);
    }

    auto start = std::chrono::high_resolution_clock::now();

    // 模拟访问（相同操作）
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        for (Object *obj : objects)
        {
            for (size_t j = 0; j < OBJECT_SIZE; ++j)
            {
                obj->data[j] = static_cast<char>(j + iter);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Unaligned test: " << duration.count() << " μs\n";

    // 释放
    for (Object *obj : objects)
    {
        delete obj;
    }
}

int main()
{
    test_aligned();
    test_unaligned();
    // NOTE: 5% 提升

    static_assert(std::hardware_destructive_interference_size == 64);

    return 0;
}
/*
Aligned test: 153642 μs
Unaligned test: 161933 μs
*/
// NOLINTEND