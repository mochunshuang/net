#include <iostream>
#include <cstdint>
#include <vector>
#include <stack>
#include <new>
#include <memory>

// NOLINTBEGIN

// 对象定义（1024 字节大小）
struct alignas(1024) BigObject
{
    char data[1024]; // 1KB 对象
};

// 内存块（包含 64 个对象）
struct Chunk
{
    static constexpr size_t OBJECT_COUNT = 64;
    static constexpr size_t ALIGNMENT = alignof(BigObject); // 1024

    BigObject *objects;

    Chunk()
    {
        // 分配对齐内存（整个 chunk 按 1024 字节对齐）
        objects = static_cast<BigObject *>(::operator new(
            OBJECT_COUNT * sizeof(BigObject), static_cast<std::align_val_t>(ALIGNMENT)));

        // 在内存中构造对象
        for (size_t i = 0; i < OBJECT_COUNT; ++i)
        {
            new (&objects[i]) BigObject();
        }
    }

    ~Chunk()
    {
        // 析构对象
        for (size_t i = 0; i < OBJECT_COUNT; ++i)
        {
            objects[i].~BigObject();
        }

        // 释放对齐内存
        ::operator delete(objects, static_cast<std::align_val_t>(ALIGNMENT));
    }

    // 禁止复制
    Chunk(const Chunk &) = delete;
    Chunk &operator=(const Chunk &) = delete;
};

// 对象池管理器
class ObjectPool
{
  public:
    BigObject *allocate()
    {
        if (free_list.empty())
        {
            // 分配新 chunk（按 1024 对齐）
            chunks.push_back(std::make_unique<Chunk>());
            auto &chunk = chunks.back();

            // 将新 chunk 的所有对象加入空闲列表
            for (size_t i = 0; i < Chunk::OBJECT_COUNT; ++i)
            {
                free_list.push(&chunk->objects[i]);
            }
        }

        auto obj = free_list.top();
        free_list.pop();
        return obj;
    }

    void deallocate(BigObject *obj)
    {
        free_list.push(obj);
    }

  private:
    std::vector<std::unique_ptr<Chunk>> chunks;
    std::stack<BigObject *> free_list;
};

// 验证对齐的实用函数
bool is_aligned(const void *ptr, size_t alignment)
{
    return (reinterpret_cast<uintptr_t>(ptr) % alignment == 0);
}

int main()
{
    ObjectPool pool;

    // 分配并验证对齐
    BigObject *obj1 = pool.allocate();
    std::cout << "Object 1 address: " << obj1 << "\n"
              << "Is 1024-byte aligned: " << is_aligned(obj1, alignof(BigObject))
              << "\n\n";

    BigObject *obj2 = pool.allocate();
    std::cout << "Object 2 address: " << obj2 << "\n"
              << "Is 1024-byte aligned: " << is_aligned(obj2, alignof(BigObject))
              << "\n\n";

    // 验证两个对象是否在同一个 chunk
    size_t distance = reinterpret_cast<char *>(obj2) - reinterpret_cast<char *>(obj1);
    std::cout << "Distance between objects: " << distance << " bytes\n";
    std::cout << "Expected object size: " << sizeof(BigObject) << " bytes\n";
    std::cout << "Same chunk: " << (distance < Chunk::OBJECT_COUNT * sizeof(BigObject))
              << "\n";

    // 释放对象
    pool.deallocate(obj1);
    pool.deallocate(obj2);

    return 0;
}
// NOLINTEND