#include <cassert>
#include <cstddef>
#include <memory_resource>
#include <iostream>
#include <vector>

// NOLINTBEGIN

// 类型安全的对象池
template <typename T>
class TypedObjectPool
{
  public:
    explicit TypedObjectPool(
        std::pmr::memory_resource *upstream = std::pmr::get_default_resource())
        : pool_(create_pool_options(), upstream)
    {
    }

    // 分配对象
    template <typename... Args>
    T *construct(Args &&...args)
    {
        void *mem = pool_.allocate(sizeof(T), alignof(T));
        T *obj = new (mem) T(std::forward<Args>(args)...);
        allocations_.push_back(mem);
        std::cout << "构造对象: " << obj << " (内存: " << mem << ")\n";
        return obj;
    }

    // 销毁对象
    void destroy(T *obj)
    {
        if (!obj)
            return;

        obj->~T();
        void *mem = reinterpret_cast<void *>(obj);
        pool_.deallocate(mem, sizeof(T), alignof(T));

        // 标记为已释放
        auto it = std::find(allocations_.begin(), allocations_.end(), mem);
        if (it != allocations_.end())
        {
            deallocations_.push_back(mem);
            allocations_.erase(it);
            std::cout << "销毁对象: " << obj << " (内存: " << mem << ")\n";
        }
    }

    // 打印内存状态
    void print_memory_status() const
    {
        std::cout << "\n===== 内存状态 ====="
                  << "\n活动对象: " << allocations_.size()
                  << "\n已释放对象: " << deallocations_.size() << "\n内存块地址历史:\n";

        std::cout << "活动内存块: ";
        for (void *mem : allocations_)
        {
            std::cout << mem << " ";
        }

        std::cout << "\n已释放内存块: ";
        for (void *mem : deallocations_)
        {
            std::cout << mem << " ";
        }
        std::cout << "\n\n";
    }

    // 获取底层资源
    std::pmr::memory_resource *resource()
    {
        return &pool_;
    }

  private:
    static std::pmr::pool_options create_pool_options()
    {
        std::pmr::pool_options options;
        options.max_blocks_per_chunk = 100;              // 每个块的最大分配数
        options.largest_required_pool_block = sizeof(T); // NOTE: 仅处理此类型大小
        return options;
    }

    std::pmr::unsynchronized_pool_resource pool_;
    std::vector<void *> allocations_;   // 当前分配的内存
    std::vector<void *> deallocations_; // 已释放的内存
};

// 测试对象
struct GameObject
{
    GameObject(int id, float x, float y) : id(id), position{x, y}
    {
        std::cout << "  GameObject " << id << " 创建于 (" << x << ", " << y << ")\n";
    }

    ~GameObject()
    {
        std::cout << "  GameObject " << id << " 销毁\n";
    }

    int id;
    float position[2];
};

void test_noexcept() noexcept
{
    // Size: 12 bytes, alignment 4 bytes。 //NOTE: 恰好 10
    struct Object
    {
        int id{};
        float position[2];
        Object(int id, float x, float y) noexcept : id(id), position{x, y} {}
    };
    // 创建类型安全的对象池
    TypedObjectPool<Object> gameObjectPool;

    // 测试大量分配
    std::vector<Object *> objects;
    std::cout << "===== 大量分配测试 =====\n";

    // 分配超过 max_blocks_per_chunk 的对象
    const size_t OVER_LIMIT = 150; // > max_blocks_per_chunk(100)
    for (int i = 0; i < OVER_LIMIT; i++)
    {
        objects.push_back(gameObjectPool.construct(i, i * 10.0f, i * 20.0f));
    }

    // NOTE: 对象字节填充的证据。对齐
    // NOTE: 无法避免，必须是 2 的幂 的对象大小
    assert((ptrdiff_t)objects[1] - (ptrdiff_t)objects[0] == 0x10);
    assert((ptrdiff_t)objects[1] - (ptrdiff_t)objects[0] > sizeof(Object));

    gameObjectPool.print_memory_status();
    std::cout << "分配了 " << OVER_LIMIT << " 个对象，没有异常\n";

    // 测试大对象分配
    std::cout << "\n===== 大对象分配测试 =====\n";
    try
    {
        // 尝试分配大于 largest_required_pool_block 的对象
        struct BigObject
        {
            char data[sizeof(Object) * 2]; // 2倍大小
        };

        // 使用相同的池分配大对象
        void *bigMem = gameObjectPool.resource()->allocate(sizeof(Object));
        std::cout << "大对象分配成功: " << bigMem << "\n";
        gameObjectPool.resource()->deallocate(bigMem, sizeof(Object));
    }
    catch (const std::exception &e)
    {
        std::cout << "异常: " << e.what() << "\n";
    }

    // 清理
    for (auto obj : objects)
    {
        gameObjectPool.destroy(obj);
    }

    // NOTE: std::pmr::pool_options 不会约束 内存池大小，内存大小是无限的，直到崩溃
}

int main()
{
    test_noexcept();

    // 创建类型安全的对象池
    TypedObjectPool<GameObject> gameObjectPool;

    std::cout << "===== 分配阶段 =====\n";
    // 分配一些游戏对象
    GameObject *obj1 = gameObjectPool.construct(1, 10.0f, 20.0f);
    GameObject *obj2 = gameObjectPool.construct(2, 30.0f, 40.0f);
    GameObject *obj3 = gameObjectPool.construct(3, 50.0f, 60.0f);

    gameObjectPool.print_memory_status();

    std::cout << "===== 释放中间对象 =====\n";
    // 释放中间对象
    gameObjectPool.destroy(obj2);
    gameObjectPool.print_memory_status();

    std::cout << "===== 重用内存测试 =====\n";
    // 创建新对象 - 应重用 obj2 的内存
    GameObject *obj4 = gameObjectPool.construct(4, 70.0f, 80.0f);
    gameObjectPool.print_memory_status();

    std::cout << "===== 释放所有对象 =====\n";
    gameObjectPool.destroy(obj1);
    gameObjectPool.destroy(obj3);
    gameObjectPool.destroy(obj4);
    gameObjectPool.print_memory_status();

    std::cout << "===== 最终重用测试 =====\n";
    // 创建新对象 - 应重用之前的所有内存
    GameObject *obj5 = gameObjectPool.construct(5, 90.0f, 100.0f);
    GameObject *obj6 = gameObjectPool.construct(6, 110.0f, 120.0f);
    GameObject *obj7 = gameObjectPool.construct(7, 130.0f, 140.0f);
    gameObjectPool.print_memory_status();

    // 清理
    gameObjectPool.destroy(obj5);
    gameObjectPool.destroy(obj6);
    gameObjectPool.destroy(obj7);

    return 0;
}
// NOLINTEND