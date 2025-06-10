#include <cassert>
#include <iostream>
#include <vector>
// NOLINTBEGIN

void print(auto rem, const std::vector<int> &c)
{
    for (std::cout << rem; const int el : c)
        std::cout << el << ' ';
    std::cout << '\n';
}

// minimal C++11 allocator with debug output
template <class Tp>
struct NAlloc
{
    typedef Tp value_type;

    NAlloc() = default;
    template <class T>
    NAlloc(const NAlloc<T> &)
    {
    }

    Tp *allocate(std::size_t n)
    {
        n *= sizeof(Tp);
        Tp *p = static_cast<Tp *>(::operator new(n));
        std::cout << "allocating " << n << " bytes @ " << p << '\n';
        return p;
    }

    void deallocate(Tp *p, std::size_t n)
    {
        std::cout << "deallocating " << n * sizeof *p << " bytes @ " << p << "\n\n";
        ::operator delete(p);
    }
};

template <class T, class U>
bool operator==(const NAlloc<T> &, const NAlloc<U> &)
{
    return true;
}

template <class T, class U>
bool operator!=(const NAlloc<T> &, const NAlloc<U> &)
{
    return false;
}

int main()
{
    std::cout << "==================== resize  ====================\n";
    // resize //NOTE: 默认修改 右边的
    {
        std::vector<int> c = {1, 2, 3};
        print("The vector holds: ", c);

        c.resize(5);
        print("After resize up to 5: ", c);

        c.resize(2);
        print("After resize down to 2: ", c);

        c.resize(6, 4);
        print("After resize up to 6 (initializer = 4): ", c);
    }
    // erase
    std::cout << "\n==================== erase  ====================\n";
    {
        std::vector<int> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        print("not erase: ", c);

        c.erase(c.begin());
        print("erase begin: ", c);

        c.erase(c.begin() + 2, c.begin() + 5);
        print("erase range: ", c);

        // Erase all even numbers
        for (std::vector<int>::iterator it = c.begin(); it != c.end();)
        {
            if (*it % 2 == 0)
                it = c.erase(it);
            else
                ++it;
        }
        print("erase all even numbers: ", c);
    }
    std::cout << "\n==================== reserve  ====================\n";
    // reserve //NOTE: 能减少 扩容或缩容的 调用次数。如果知道要存多少内存
    {
        constexpr int max_elements = 32;

        static_assert(max_elements * sizeof(int) == 128); // NOTE: 这么多的字节

        std::cout << "==================== using reserve: start\n";
        {
            std::vector<int, NAlloc<int>> v1;
            v1.reserve(
                max_elements); // reserves at least max_elements * sizeof(int) bytes
            assert(v1.size() == 0);
            assert(v1.capacity() != 0);

            for (int n = 0; n < max_elements; ++n)
            {
                if (v1.size() == v1.capacity())
                    std::cout << "size() == capacity() == " << v1.size() << '\n';
                v1.push_back(n);
            }
        }
        std::cout << "==================== using reserve: end\n";

        std::cout << "==================== not using reserve: start\n";
        {
            std::vector<int, NAlloc<int>> v1;

            for (int n = 0; n < max_elements; ++n)
            {
                if (v1.size() == v1.capacity())
                    std::cout << "size() == capacity() == " << v1.size() << '\n';
                v1.push_back(n);
            }
        }
        std::cout << "==================== not using reserve: end\n";
    }
    {
        struct A
        {
            explicit A(int v) noexcept : v{v}
            {
                std::cout << "A()\n";
            }
            A(A &&a) noexcept : v{a.v}
            {
                std::cout << "A(A &&a)\n";
            }
            A(const A &) = default;
            A &operator=(A &&a) noexcept
            {
                this->v = a.v;
                std::cout << "operator=(A &&a)\n";
                return *this;
            }
            A &operator=(const A &) = default;
            int v;
        };
        std::vector<A> v1; // NOTE: 不会调用 A构造函数
        v1.reserve(10);    // NOTE: 不会调用 A构造函数. 未初始化代码
        {
            std::cout << "===emplace start\n";
            v1.emplace(v1.cbegin(), 1); // 调用一次
            std::cout << "===emplace end\n";

            std::cout << "===push_back start\n";
            v1.push_back(A{2}); // NOTE: 不够高效
            std::cout << "===push_back end\n";

            std::cout << "===emplace_back start\n";
            assert(v1.size() == 2);
            v1.emplace_back(3); // NOTE: emplace_back 就是在 size() 索引处 填值
            std::cout << "===emplace_back end\n";

            assert(v1[2].v == 3);
        }
        {
            std::vector<int> v = {1, 2, 3};
            auto *addr0 = &v[0];
            std::cout << "初始地址: " << addr0 << std::endl;

            v.push_back(4); // 触发扩容
            auto *addr1 = &v[0];
            std::cout << "扩容后地址: " << addr1 << std::endl; // 地址改变

            assert(addr0 != addr1); // NOTE: 证明了，数据搬迁。内存地址不一样了
        }
        {
            // 互相转换
            std::uint8_t u8 = 0xAB;
            std::byte b = static_cast<std::byte>(u8);
            std::uint8_t u8_back = static_cast<std::uint8_t>(b);

            // 位操作（无需转换）
            std::byte mask = std::byte{0x0F};
            std::byte masked = b & mask; // 结果为0x0B

            // NOTE: 无法比较。 因此不好
            assert(static_cast<std::uint8_t>(masked) == 0x0B);
        }
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND