#include <iostream>
#include <cstddef>
// NOLINTBEGIN

// 使用 alignas 指定 256 字节对齐
alignas(256) struct chunk
{
    char data[256];
};

// 验证对齐值的函数
template <typename T>
void check_alignment()
{
    T obj;
    std::cout << "Type: " << typeid(T).name() << "\n";
    std::cout << "Alignment requirement: " << alignof(T) << " bytes\n";
    std::cout << "Address of object: " << &obj << "\n";
    std::cout << "Is aligned to 256 bytes? "
              << ((reinterpret_cast<std::uintptr_t>(&obj) % 256) == 0) << "\n\n";
}

int main()
{
    // 测试自定义对齐类型
    check_alignment<chunk>();

    // 测试普通类型
    check_alignment<int>();
    check_alignment<double>();

    // 测试数组对齐（注意数组对齐通常等于元素对齐）
    alignas(256) char buffer[256];
    std::cout << "Buffer alignment: " << alignof(decltype(buffer)) << " bytes\n";
    std::cout << "Buffer address: " << static_cast<void *>(buffer) << "\n";
    std::cout << "Is buffer aligned to 256 bytes? "
              << ((reinterpret_cast<std::uintptr_t>(buffer) % 256) == 0) << "\n";

    {
        // 使用对齐 new 分配
        chunk *p = new (std::align_val_t{256}) chunk;

        // 验证对齐
        uintptr_t address = reinterpret_cast<uintptr_t>(p);
        bool is_aligned = (address % 256 == 0);

        std::cout << "Address: " << std::hex << address << std::dec << "\n"
                  << "Address % 256: " << (address % 256) << "\n"
                  << "Is 256-byte aligned: " << std::boolalpha << is_aligned << "\n";

        // 正确释放
        ::operator delete(p, std::align_val_t{256});
    }
    std::cout << " \n new chunk: \n";
    {
        // 使用对齐 new 分配
        chunk *p = new chunk;

        // 验证对齐
        uintptr_t address = reinterpret_cast<uintptr_t>(p);
        bool is_aligned = (address % 256 == 0);

        std::cout << "Address: " << std::hex << address << std::dec << "\n"
                  << "Address % 256: " << (address % 256) << "\n"
                  << "Is 256-byte aligned: " << std::boolalpha << is_aligned << "\n";

        // 正确释放
        delete p;
    }

    return 0;
}
// NOLINTEND