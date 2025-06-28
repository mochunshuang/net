#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <algorithm>
#include <span>

// NOLINTBEGIN

class S
{
    int x_;
    float y_;
    double z_;

  public:
    constexpr S(int x, float y, double z) : x_{x}, y_{y}, z_{z} {}
    [[nodiscard("no side-effects!")]] constexpr bool operator==(
        const S &) const noexcept = default;
};

consteval bool test()
{
    alignas(S) unsigned char storage[sizeof(S)]{};
    // S *ptr = std::construct_at((S *)(storage), 42, 2.71f, 3.14);
    // error: '__builtin_bit_cast' is not a constant expression because 'S*' is a pointer
    // type
    // S *ptr = std::bit_cast<S *>(&storage);//NOTE: 不允许
    S uninitialized = std::bit_cast<S>(storage);
    S *ptr = &uninitialized;
    // 在编译期构造对象
    std::construct_at(ptr, 42, 2.71f, 3.14);
    const bool res{*ptr == S{42, 2.71f, 3.14}};
    // std::destroy_at(&uninitialized); //NOTE: 不要释放 栈内存
    // std::destroy_at(ptr); // NOTE: 不要释放 栈内存
    // NOTE:  error: destroying 'uninitialized' outside its lifetime。
    // NOTE: consteval 来检查释放内存泄漏真的很好

    return res;
}

constexpr bool test1()
{

    alignas(S) unsigned char storage[2 * sizeof(S)]{};
    void *v = storage;
    //  error: call to non-'constexpr' function 'void* operator new(std::size_t, void*)'
    S *ptr = new (v) S(42, 2.71f, 3.14);
    const bool res{*ptr == S{42, 2.71f, 3.14}};
    return res;
}

constexpr bool test2()
{
    alignas(S) unsigned char storage[2 * sizeof(S)]{};
    //  error: call to non-'constexpr' function 'void* operator new(std::size_t, void*)'
    S *ptr = new (static_cast<void *>(&storage[1])) S(42, 2.71f, 3.14);
    const bool res{*ptr == S{42, 2.71f, 3.14}};

    // NOTE: 返回首地址
    assert(ptr == (void *)&storage[1]);
    return res;
}

constexpr bool test3()
{
    alignas(S) unsigned char storage[sizeof(S)]{};
    //  error: call to non-'constexpr' function 'void* operator new(std::size_t, void*)'
    auto *ptr = std::construct_at(static_cast<S *>(static_cast<void *>(&storage)), 42,
                                  2.71f, 3.14);
    const bool res{*ptr == S{42, 2.71f, 3.14}};

    // NOTE: 返回首地址
    assert(ptr == (void *)&storage[0]);
    return res;
}

constexpr bool test4()
{
    std::array<S, 1> storage = {S{0, 0, 0}};
    //  error: call to non-'constexpr' function 'void* operator new(std::size_t,
    //  void*)'
    auto *ptr = std::construct_at(&storage[0], 42, 2.71f, 3.14);
    const bool res{*ptr == S{42, 2.71f, 3.14}};

    // NOTE: 返回首地址
    assert(ptr == (void *)&storage[0]);
    return res;
}

constexpr bool test5()
{
    // std::array<S, 1> storage = {S{0, 0, 0}};
    auto *storage = new S{0, 0, 0};

    auto *ptr = std::construct_at(&storage[0], 42, 2.71f, 3.14);
    const bool res{*ptr == S{42, 2.71f, 3.14}};

    // NOTE: 返回首地址
    assert(ptr == (void *)&storage[0]);

    delete storage;
    return res;
}

consteval bool test6()
{
    // std::array<S, 1> storage = {S{0, 0, 0}};
    auto *storage = new S{0, 0, 0};

    S *ptr = std::construct_at(&storage[0], 42, 2.71f, 3.14);
    const bool res{*ptr == S{42, 2.71f, 3.14}};

    // NOTE: 返回首地址
    assert(ptr == (void *)&storage[0]);

    // NOTE: 允许间接删除吗？不允许
    // NOTE: because allocated storage has not been deallocated
    // std::destroy_at(ptr);
    // std::destroy_at(std::addressof(ptr));
    // std::destroy_at(std::to_address(ptr));

    // NOTE: 必须这样
    delete storage;
    return res;
}

consteval bool test7()
{
    struct chunk
    {
        // NOTE: enum class byte : unsigned char. 都需要强转，有代价
        std::array<unsigned char, 1024> buffer;
        bool use{false};
    };
    static_assert(sizeof(chunk) == 1025);

    chunk buf;
    std::fill(buf.buffer.begin(), buf.buffer.end(), 1);
    assert(buf.buffer[1] == 1);

    auto *ptr = buf.buffer.data();
    // static_cast<chunk *>(buf.buffer); // NOTE: 不允许这样
    // auto *usd = static_cast<bool *>(ptr + 1);// NOTE: 不允许
    // 使用span访问整个对象的内存
    std::span<unsigned char> span(static_cast<unsigned char *>(buf.buffer.data()),
                                  sizeof(chunk));
    // span[1024] = true; //越界

    {
        struct chunk
        {
            // NOTE: 底层数据结构不会检查
            unsigned char buffer[1024];
            bool use{false};
        };
        chunk buf;
        std::span<unsigned char> span(buf.buffer, sizeof(chunk));
        // span[1024] = true; // NOTE: 还是被检查出来了
        // span.data() + 1024 = true;
        // *static_cast<unsigned char *>(span.data() + 1024) = 1;

        unsigned char *ptr = buf.buffer;
        *static_cast<unsigned char *>(ptr + 1023) = 1; // NOTE: 1024 总是不行
                                                       // assert(buf.use);
    }

    // NOTE: 肯定也不允许，因为 size 不一样
    //  chunk uninitialized = std::bit_cast<chunk>(buf.buffer);

    return true;
}

int main()
{
    static_assert(test());
    // static_assert(test3()); // NOTE: 不可以 'unsigned char [16]' is not similar to 'S'
    // static_assert(test1()); // NOTE: 不可以
    assert(test1());
    assert(test2());
    assert(test3());

    // NOTE: 内存块，仅仅负责构造就好了。释放按整块处理
    static_assert(test4());
    static_assert(test5());
    static_assert(test6());

    static_assert(test7());

    // NOTE: 毫无疑问 std::construct_at 优先于 new 表达式
    // NOTE: new 表达式 在 c++26 编译期可以计算

    { // NOTE: reinterpret_cast 解决一切，但是只能运行时
        struct chunk
        {
            // NOTE: 底层数据结构不会检查
            unsigned char buffer[1024];
            bool use{false};
        };
        chunk buf;
        auto *ptr = reinterpret_cast<chunk *>(&buf.buffer);
        assert(not buf.use);
        ptr->use = true;
        assert(buf.use);

        // NOTE: 这样设计是失败的
    }
    std::cout << " main done\n";
}

// NOLINTEND