#include <cassert>
#include <iostream>
#include <memory_resource>
#include <new>
#include <numeric>
#include <string>
#include <string_view>
#include <unordered_map>
#include <array>

// NOLINTBEGIN

class tracking_memory_resource : public std::pmr::memory_resource
{
  public:
    std::vector<std::size_t> allocations{};
    std::vector<std::size_t> deallocations{};

    tracking_memory_resource(std::pmr::memory_resource *delegate) : m_delegate{delegate}
    {
    }

    void *do_allocate(std::size_t bytes, std::size_t align) override
    {
        allocations.push_back(bytes);
        return m_delegate->allocate(bytes, align);
    }

    void do_deallocate(void *ptr, std::size_t bytes, std::size_t align) override
    {
        deallocations.push_back(bytes);
        m_delegate->deallocate(ptr, bytes, align);
    }

    bool do_is_equal(const memory_resource &other) const noexcept override
    {
        return m_delegate->is_equal(other);
    }

    bool all_memory_deallocated()
    {

        return std::accumulate(begin(allocations), end(allocations), 0) ==
               std::accumulate(begin(deallocations), end(deallocations), 0);
    }

  private:
    std::pmr::memory_resource *m_delegate;
};

void test_null_memory_resource() // NOLINT
{
    const char *undeletable = "xyz";

    /**
 * @brief
返回一个指针 p，指向一个从 std::pmr::memory_resource
派生的静态存储期对象，具有以下属性
它的 allocate() 函数总是抛出 std::bad_alloc；
它的 deallocate() 函数不起作用；
对于任何 memory_resource r，p->is_equal(r) 返回 &r == p。
每次调用此函数时都返回相同的值。
 *
 */
    std::pmr::memory_resource *mr = std::pmr::null_memory_resource();

    bool throw_bad_alloc = false;
    try
    {
        [[maybe_unused]] auto *p = mr->allocate(1);
    }
    catch (const std::bad_alloc &c)
    {
        throw_bad_alloc = true;
    }
    assert(throw_bad_alloc == true);

    {

        try
        {
            mr->deallocate(const_cast<char *>(undeletable), 4);
        }
        catch (const std::bad_alloc &c)
        {
            assert(false);
        }
    }

    // NOTE: == 测试
    assert(*mr == *std::pmr::null_memory_resource());

    {
        // allocate memory on the stack
        constexpr auto k_size = 20000;
        std::array<std::byte, k_size> buf;

        // without fallback memory allocation on heap
        std::pmr::monotonic_buffer_resource pool{buf.data(), buf.size(),
                                                 std::pmr::null_memory_resource()};

        // allocate too much memory
        std::pmr::unordered_map<long, std::pmr::string> coll{&pool};

        try
        {
            for (std::size_t i = 0; i < buf.size(); ++i)
            {
                coll.emplace(i, "just a string with number " + std::to_string(i));

                if (i && i % 50 == 0)
                    std::clog << "size: " << i << "...\n";
            }
        }
        catch (const std::bad_alloc &e)
        {
            std::cerr << e.what() << '\n';

            assert(e.what() == std::string_view{"std::bad_alloc"});
        }

        std::cout << "size: " << coll.size() << '\n';
    }
}

void test_new_delete_resource()
{
    std::pmr::memory_resource *mr = std::pmr::get_default_resource();

    // 测试 0 字节分配
    // NOTE: memory_resource::allocate(0) 不保证返回 nullptr
    void *p = mr->allocate(0);
    mr->deallocate(p, 0); // 确保能正确配对
    assert(nullptr != p);

    // NOTE: 允许 0 字节分配
    {
        int *p = new int[0]; // 返回非 nullptr，但不能访问 p[0] // NOLINT
        assert(nullptr != p);
        delete p; // NOLINT
    }

    char *bytes = (char *)mr->allocate(3);
    bytes[0] = 'a';
    bytes[1] = 'b';
    bytes[2] = 'c';
    try
    {
        mr->deallocate(bytes, 3);
    }
    catch (...)
    {
        assert(false);
    }
    assert(*mr == *std::pmr::get_default_resource());
    assert(mr == std::pmr::get_default_resource());

    // 尝试判断是否为某种已知的具体类型
    if (auto *sync_mr = dynamic_cast<std::pmr::synchronized_pool_resource *>(mr); sync_mr)
    {
        // 是 synchronized_pool_resource
        std::cout << "synchronized_pool_resource" << '\n';
    }
    else if (auto *unsync_mr = dynamic_cast<std::pmr::unsynchronized_pool_resource *>(mr);
             unsync_mr)
    {
        // 是 unsynchronized_pool_resource
        std::cout << "unsynchronized_pool_resource" << '\n';
    }
    else if (auto *monotonic_mr = dynamic_cast<std::pmr::monotonic_buffer_resource *>(mr);
             monotonic_mr)
    {
        // 是 monotonic_buffer_resource
        std::cout << "monotonic_buffer_resource" << '\n';
    }
    else
    {
        // 可能是其他实现定义的类型（如默认的 new/delete 封装）
        std::cout << "new/delete" << '\n';
        // NOTE: 确实如此
        assert(std::pmr::get_default_resource() == std::pmr::new_delete_resource());
    }
    /*
返回一个指针 p，指向一个从 std::pmr::memory_resource
派生的类型的静态存储期对象，具有以下属性
    它的 allocate() 函数使用 ::operator new 来分配内存；
    它的 deallocate() 函数使用 ::operator delete 来释放内存；
    对于任何 memory_resource r，p->is_equal(r) 返回 &r == p。
每次调用此函数都返回相同的值。
*/
}

void test_default_resource()
{

    tracking_memory_resource test_mr{std::pmr::get_default_resource()};

    std::pmr::memory_resource *before = std::pmr::set_default_resource(&test_mr);
    std::pmr::memory_resource *after = std::pmr::get_default_resource();

    assert(before != after);
    assert(after == &test_mr);

    void *ptr = std::pmr::get_default_resource()->allocate(10);
    std::pmr::get_default_resource()->deallocate(ptr, 10);

    assert(1 == test_mr.allocations.size());
    assert(10 == test_mr.allocations[0]);

    assert(1 == test_mr.deallocations.size());
    assert(10 == test_mr.deallocations[0]);
    {
        void *ptr = test_mr.allocate(5);
        test_mr.deallocate(ptr, 5);

        assert(2 == test_mr.allocations.size());
        assert(5 == test_mr.allocations[1]);

        assert(2 == test_mr.deallocations.size());
        assert(5 == test_mr.deallocations[1]);
    }

    // NOTE: 默认是 new_delete_resource
    std::pmr::set_default_resource(nullptr);
    assert(before == std::pmr::get_default_resource());
}

void test_monotonic_buffer_resource()
{
    std::pmr::monotonic_buffer_resource mbr;
    mbr.upstream_resource();

    void *ptr = mbr.allocate(1024);

    try
    {
        mbr.deallocate(ptr, 1024);
    }
    catch (...)
    {
        assert(false);
    }
    try
    {
        mbr.deallocate(ptr, 1024);
    }
    catch (...)
    {
        assert(false); // NOTE: 离谱
        // std::cout << "deallocate error\n";
    }

    mbr.release(); // 才真正的释放
}

int main()
{
    {
        std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        int sum = std::accumulate(v.begin(), v.end(), 0);
        int product = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>());
        assert(sum == 55);
        assert(product == 3628800);
    }
    std::cout << "===========test_null_memory_resource\n";
    test_null_memory_resource();
    std::cout << "\n===========test_null_memory_resource\n";
    test_new_delete_resource();
    std::cout << "\n===========test_default_resource\n";
    test_default_resource();
    std::cout << "\n===========test_monotonic_buffer_resource\n";
    test_monotonic_buffer_resource();
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND