#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <thread>
// NOLINTBEGIN

using namespace std::chrono_literals;

struct Base
{
    Base()
    {
        std::cout << "Base::Base()\n";
    }

    // Note: non-virtual destructor is OK here
    ~Base()
    {
        std::cout << "Base::~Base()\n";
    }
};

struct Derived : public Base
{
    Derived()
    {
        std::cout << "Derived::Derived()\n";
    }

    ~Derived()
    {
        std::cout << "Derived::~Derived()\n";
    }
};

void print(auto rem, std::shared_ptr<Base> const &sp)
{
    std::cout << rem << "\n\tget() = " << sp.get() << ", use_count() = " << sp.use_count()
              << '\n';
}

void thr(std::shared_ptr<Base> p)
{
    std::this_thread::sleep_for(987ms);
    // NOTE: 打印是线程不安全的
    print("     outter pointer in a thread befor:", p);
    std::shared_ptr<Base> lp = p; // thread-safe, even though the
                                  // shared use_count is incremented
    print("     Local pointer in a thread:", lp);
    print("     outter pointer in a thread after:", p);
    {
        static std::mutex io_mutex;
        std::lock_guard<std::mutex> lk(io_mutex);
        print("Local pointer in a thread:", lp);
    }
    std::cout << "============end fun==========\n\n";
}

void single_thread() noexcept;
void make_shared() noexcept;

int main()
{
    // NOTE: Derived 就是保存指针本身。 共享指针的封装
    std::shared_ptr<Base> p = std::make_shared<Derived>();

    print("Created a shared Derived (as a pointer to Base)", p);

    std::thread t1{thr, p}, t2{thr, p}, t3{thr, p};

    // NOTE: 接下来的 分号复制操作，无论任何 use_count() == 0
    p.reset(); // release ownership from main

    print("Shared ownership between 3 threads and released ownership from main:", p);

    t1.join();
    t2.join();
    t3.join();

    // NOTE: 发现最大值是 6 .

    std::cout << "\n-----> reset after test: \n"; // NOTE: 析构 Derived 在这行之前
    // NOTE: reset 后 就不一样了
    {
        // 测试： const auto& 是否增加 use_count()
        int count = p.use_count();
        [=](const auto &p) {
            assert(count == p.use_count());
        }(p);
        [=](auto &p) {
            assert(count == p.use_count());
        }(p);
        // NOTE: ? 为什么不增加计数？ 可能是 BUG的来源
        [=](auto p) {
            assert(count == p.use_count());
        }(p);

        // NOTE: 记住构造是不会增加 use_count 的
        auto p2{p};
        assert(p2.use_count() == p.use_count());

        // NOTE: 构造shared_ptr后 默认的 use_count == 0
        assert(p.use_count() == 0);

        {
            // NOTE: 赋值copy 才会 增加
            std::shared_ptr<Base> p2 = p;
            assert(p.use_count() == 0);
            assert(p2.use_count() == 0); // 为什么会这样？
            // NOTE: reset(); 前面调用，是一切的根源
        }
    }

    std::cout << "\n-----> single_thread start\n";
    // NOTE: 单线程普通测试
    single_thread();
    std::cout << "\n-----> single_thread end\n";

    {
        struct Foo
        {
            int id{0};
            Foo(int i = 0) : id{i}
            {
                std::cout << "Foo::Foo(" << i << ")\n";
            }
            ~Foo()
            {
                std::cout << "Foo::~Foo(), id=" << id << '\n';
            }
        };

        // NOTE: 可以拦截什么时候 调用 析构函数
        std::cout << "\n-----> constructor with object and deleter\n";
        std::shared_ptr<Foo> sh5(new Foo{12}, [](auto *p) {
            static_assert(std::is_same_v<decltype(p), Foo *>);
            std::cout << "Call delete from lambda... p->id=" << p->id << '\n';
            delete p;
        });

        // NOTE: 不能通过其他方式 方便的自定义删除器了
        // NOTE: 使用std::make_shared：简单高效，但无法自定义删除器。
    }
    {
        std::cout << "\n----->  access to the stored array\n";
        const std::size_t arr_size = 10;
        std::shared_ptr<int[]> pis(new int[10]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        for (std::size_t i = 0; i < arr_size; ++i)
            std::cout << pis[i] << ' ';
        std::cout << '\n';
    }
    {
        class Value
        {
            int i;

          public:
            Value(int i) : i(i)
            {
                std::cout << "Value(), i = " << i << '\n';
            }
            ~Value()
            {
                std::cout << "~Value(), i = " << i << '\n';
            }
            void print() const
            {
                std::cout << "i = " << i << '\n';
            }
        };
        std::cout << "\n----->  shared_ptr + pmr\n";
        // Create a polymorphic allocator using the monotonic buffer resource
        std::byte buffer[sizeof(Value) * 8];
        std::pmr::monotonic_buffer_resource resource(buffer, sizeof(buffer));
        std::pmr::polymorphic_allocator<Value> allocator(&resource);

        std::vector<std::shared_ptr<Value>> v;

        for (int i{}; i != 4; ++i)
            // Use std::allocate_shared with the custom allocator
            v.emplace_back(std::allocate_shared<Value>(allocator, i));

        for (const auto &sp : v)
            sp->print();
    }

    make_shared(); // C++20 后更强大

    std::cout << "All threads completed, the last one deleted Derived.\n";
}

void single_thread() noexcept
{
    std::shared_ptr<Base> p = std::make_shared<Derived>();
    assert(p.use_count() == 1);

    int count = p.use_count();
    [=](const auto &p) {
        assert(count == p.use_count());
    }(p);
    [=](auto &p) {
        assert(count == p.use_count());
    }(p);
    // NOTE: 增加引用计数
    [=](auto p) {
        assert(count + 1 == p.use_count());
    }(p);

    // NOTE: 构造增加次数。 全部都能感知
    auto p2{p};
    assert(p2.use_count() == p.use_count());
    assert(p2.use_count() == 2);

    auto p3 = p2;
    assert(p3.use_count() == 3);
    assert(p2.use_count() == 3);
    assert(p.use_count() == 3);

    // note: move 语义不变
    auto p4 = std::move(p3);
    assert(p4.use_count() == 3);
    assert(p3.use_count() == 0);
}

void make_shared() noexcept
{
    struct C
    {
        // constructors needed (until C++20)
        C(int i) : i(i) {}
        C(int i, float f) : i(i), f(f) {}
        int i;
        float f{};
    };
    // using “auto” for the type of “sp1”
    auto sp1 = std::make_shared<C>(1); // overload (1)
    static_assert(std::is_same_v<decltype(sp1), std::shared_ptr<C>>);
    std::cout << "sp1->{ i:" << sp1->i << ", f:" << sp1->f << " }\n";

    // being explicit with the type of “sp2”
    std::shared_ptr<C> sp2 = std::make_shared<C>(2, 3.0f); // overload (1)
    static_assert(std::is_same_v<decltype(sp2), std::shared_ptr<C>>);
    static_assert(std::is_same_v<decltype(sp1), decltype(sp2)>);
    std::cout << "sp2->{ i:" << sp2->i << ", f:" << sp2->f << " }\n";

    // shared_ptr to a value-initialized float[64]; overload (2):
    std::shared_ptr<float[]> sp3 = std::make_shared<float[]>(64);

    // shared_ptr to a value-initialized long[5][3][4]; overload (2):
    std::shared_ptr<long[][3][4]> sp4 = std::make_shared<long[][3][4]>(5);

    // shared_ptr to a value-initialized short[128]; overload (3):
    std::shared_ptr<short[128]> sp5 = std::make_shared<short[128]>();

    // shared_ptr to a value-initialized int[7][6][5]; overload (3):
    std::shared_ptr<int[7][6][5]> sp6 = std::make_shared<int[7][6][5]>();

    // shared_ptr to a double[256], where each element is 2.0; overload (4):
    std::shared_ptr<double[]> sp7 = std::make_shared<double[]>(256, 2.0);

    // shared_ptr to a double[7][2], where each double[2]
    // element is {3.0, 4.0}; overload (4):
    std::shared_ptr<double[][2]> sp8 = std::make_shared<double[][2]>(7, {3.0, 4.0});

    // shared_ptr to a vector<int>[4], where each vector
    // has contents {5, 6}; overload (4):
    std::shared_ptr<std::vector<int>[]> sp9 =
        std::make_shared<std::vector<int>[]>(4, {5, 6});

    // shared_ptr to a float[512], where each element is 1.0; overload (5):
    std::shared_ptr<float[512]> spA = std::make_shared<float[512]>(1.0);

    // shared_ptr to a double[6][2], where each double[2] element
    // is {1.0, 2.0}; overload (5):
    std::shared_ptr<double[6][2]> spB = std::make_shared<double[6][2]>({1.0, 2.0});

    // shared_ptr to a vector<int>[4], where each vector
    // has contents {5, 6}; overload (5):
    std::shared_ptr<std::vector<int>[4]> spC =
        std::make_shared<std::vector<int>[4]>({5, 6});
}
// NOLINTEND