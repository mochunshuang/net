#include <cassert>
#include <memory_resource>
#include <iostream>
#include <string>
#include <vector>
#include <new>

// NOLINTBEGIN

// 监控内存资源：记录底层 new/delete 调用
class MonitoredResource : public std::pmr::memory_resource
{
    size_t alloc_count = 0;
    size_t dealloc_count = 0;
    size_t total_allocated = 0;

    void *do_allocate(size_t bytes, size_t alignment) override
    {
        void *ptr = ::operator new(bytes, std::align_val_t(alignment));
        alloc_count++;
        total_allocated += bytes;

        std::cout << ">>> SYSTEM ALLOC: " << bytes << " bytes at " << ptr
                  << " (align: " << alignment << ")\n";
        return ptr;
    }

    void do_deallocate(void *p, size_t bytes, size_t alignment) override
    {
        std::cout << "<<< SYSTEM DEALLOC: " << bytes << " bytes at " << p
                  << " (align: " << alignment << ")\n";

        dealloc_count++;
        total_allocated -= bytes;
        ::operator delete(p, bytes, std::align_val_t(alignment));
    }

    bool do_is_equal(const memory_resource &other) const noexcept override
    {
        return this == &other;
    }

  public:
    ~MonitoredResource() override
    {
        std::cout << "\n=== Memory Resource Summary ==="
                  << "\nTotal allocations: " << alloc_count
                  << "\nTotal deallocations: " << dealloc_count
                  << "\nNet bytes: " << total_allocated << "\n";
    }
};

// 增强型分配器
template <typename T>
struct allocator_t
{
    std::pmr::synchronized_pool_resource *pool;

    explicit allocator_t(std::pmr::synchronized_pool_resource *p) : pool(p) {}

    // 分配内存
    [[nodiscard]] T *allocate(std::size_t n)
    {
        T *ptr = static_cast<T *>(pool->allocate(n * sizeof(T), alignof(T)));
        std::cout << "[Alloc] " << n << " object(s) at " << ptr << "\n";
        return ptr;
    }

    // 释放内存
    void deallocate(T *p, std::size_t n) noexcept
    {
        std::cout << "[Dealloc] " << n << " object(s) at " << p << "\n";
        pool->deallocate(p, n * sizeof(T), alignof(T));
    }

    // 构造对象
    template <typename... Args>
    void construct(T *p, Args &&...args)
    {
        new (p) T(std::forward<Args>(args)...);
        std::cout << "[Construct] at " << p << "\n";
    }

    // 销毁对象
    void destroy(T *p) noexcept
    {
        p->~T();
        std::cout << "[Destroy] at " << p << "\n";
    }
};

// 复杂对象
struct DatabaseConnection
{
    int connection_id;
    std::string endpoint;
    std::vector<int> session_data;

    static int next_id;

    DatabaseConnection(std::string ep)
        : connection_id(++next_id), endpoint(std::move(ep)),
          session_data(1024) // 分配较大内存
    {
        std::cout << "+++ DB Connection " << connection_id << " created at " << this
                  << " (" << endpoint << ")\n";
    }

    ~DatabaseConnection()
    {
        std::cout << "--- DB Connection " << connection_id << " destroyed at " << this
                  << "\n";
    }

    void query(const std::string &sql)
    {
        std::cout << "[" << connection_id << "] Executing: " << sql << "\n";
    }
};

int DatabaseConnection::next_id = 0;

int main()
{
    MonitoredResource monitored_res;
    std::pmr::synchronized_pool_resource pool(&monitored_res);
    allocator_t<DatabaseConnection> alloc(&pool);

    static_assert(sizeof(DatabaseConnection) == 64);

    std::cout << "\n===== PHASE 1: 初始分配 =====\n";
    DatabaseConnection *conn1 = alloc.allocate(1);
    alloc.construct(conn1, "db-server-1:3306");
    conn1->query("SELECT * FROM users");

    DatabaseConnection *conn2 = alloc.allocate(1);
    alloc.construct(conn2, "db-server-2:5432");
    conn2->query("SHOW TABLES");

    assert((conn2 - conn1) == 1);
    assert((conn2 - conn1) != sizeof(DatabaseConnection));
    std::cout << "conn2 - conn1: " << conn2 - conn1 << '\n';
    std::cout << "reinterpret_cast: conn2 - conn1: "
              << (reinterpret_cast<ptrdiff_t>(conn2) - reinterpret_cast<ptrdiff_t>(conn1))
              << '\n';
    assert((reinterpret_cast<ptrdiff_t>(conn2) - reinterpret_cast<ptrdiff_t>(conn1)) ==
           sizeof(DatabaseConnection));

    std::cout << "\n===== PHASE 2: 释放并复用内存 =====\n";
    alloc.destroy(conn1);
    alloc.deallocate(conn1, 1);

    // 新连接应复用 conn1 的内存
    DatabaseConnection *conn3 = alloc.allocate(1);
    alloc.construct(conn3, "db-server-3:6379");
    conn3->query("KEYS *");

    // NOTE: 0k。能复用 conn1 的地址。理想满足
    assert(reinterpret_cast<ptrdiff_t>(conn1) == reinterpret_cast<ptrdiff_t>(conn3));

    std::cout << "\n===== PHASE 3: 批量操作 =====\n";
    std::vector<DatabaseConnection *> connections;
    void *pre_ptr = nullptr;
    for (int i = 0; i < 3; i++)
    {
        auto conn = alloc.allocate(1);
        if (pre_ptr != nullptr)
        {
            // NOTE: 总是高效的利用，即使底层系统分很多快，块大小不一致
            // NOTE: 内部 vector 堆分配也不影响
            assert((reinterpret_cast<ptrdiff_t>(conn) -
                    reinterpret_cast<ptrdiff_t>(pre_ptr)) == sizeof(DatabaseConnection));
        }
        alloc.construct(conn, "temp-server-" + std::to_string(i));
        connections.push_back(conn);

        pre_ptr = conn;
    }

    for (auto conn : connections)
    {
        conn->query("PING");
    }

    std::cout << "\n===== PHASE 4: 清理所有资源 =====\n";
    alloc.destroy(conn2);
    alloc.deallocate(conn2, 1);

    alloc.destroy(conn3);
    alloc.deallocate(conn3, 1);

    for (auto conn : connections)
    {
        alloc.destroy(conn);
        alloc.deallocate(conn, 1);
    }

    // NOTE: 存在复用。 对 do_allocate 的调用被分派到服务于可容纳请求大小的最小块的池。
    std::cout << "\n===== 内存池即将销毁 =====";
} // 内存池在此析构，释放所有底层内存

// NOLINTEND