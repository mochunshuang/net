#include <cassert>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <utility>

// NOLINTBEGIN

// helper class for runtime polymorphism demo below
struct B
{
    // NOTE: unique_ptr + 继承结构。必须虚析构。这和 shared_ptr 很不一样
    virtual ~B() = default;

    virtual void bar()
    {
        std::cout << "B::bar\n";
    }
};

struct D : B
{
    D()
    {
        std::cout << "D::D\n";
    }
    ~D()
    {
        std::cout << "D::~D\n";
    }

    void bar() override
    {
        std::cout << "D::bar\n";
    }
};

// a function consuming a unique_ptr can take it by value or by rvalue reference
std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
    p->bar();
    return p; // NOTE: 优化 配合 RVO
}

// helper function for the custom deleter demo below
void close_file(std::FILE *fp)
{
    std::fclose(fp);
}

// unique_ptr-based linked list demo
struct List
{
    struct Node
    {
        int data;
        std::unique_ptr<Node> next;
    };

    std::unique_ptr<Node> head;

    ~List()
    {
        // destroy list nodes sequentially in a loop, the default destructor
        // would have invoked its “next”'s destructor recursively, which would
        // cause stack overflow for sufficiently large lists.
        while (head)
        {
            auto next = std::move(head->next);
            head = std::move(next);
        } // NOTE: 移动就好，但是必须有变量接收
    }
    // NOTE: 头插法，因为总是修改头部
    void push(int data)
    {
        head = std::unique_ptr<Node>(new Node{data, std::move(head)});
    }
};

void make_unique() noexcept;
int main()
{
    std::cout << "1) Unique ownership semantics demo\n";
    {
        // Create a (uniquely owned) resource
        std::unique_ptr<D> p = std::make_unique<D>();

        // Transfer ownership to “pass_through”,
        // which in turn transfers ownership back through the return value
        std::unique_ptr<D> q = pass_through(std::move(p));

        // “p” is now in a moved-from 'empty' state, equal to nullptr
        assert(!p); // NOTE: 没有任何问题
        assert(q);
        assert(p.get() != q.get());
    }

    std::cout << "\n"
                 "2) Runtime polymorphism demo\n";
    {
        // Create a derived resource and point to it via base type
        std::unique_ptr<B> p = std::make_unique<D>();

        // Dynamic dispatch works as expected
        p->bar();
    }

    std::cout << "\n"
                 "3) Custom deleter demo\n";
    std::ofstream("demo.txt") << 'x'; // prepare the file to read
    {
        using unique_file_t = std::unique_ptr<std::FILE, decltype(&close_file)>;
        unique_file_t fp(std::fopen("demo.txt", "r"), &close_file);
        if (fp)
            std::cout << char(std::fgetc(fp.get())) << '\n';
    } // “close_file()” called here (if “fp” is not null)

    std::cout << "\n"
                 "4) Custom lambda expression deleter and exception safety demo\n";
    try
    {
        // NOTE: 要求删除器是指针类型。 还是不能通过 make_unique 自定义
        std::unique_ptr<D, void (*)(D *)> p(new D, [](D *ptr) {
            std::cout << "destroying from a custom deleter...\n";
            delete ptr;
        });

        throw std::runtime_error(""); // “p” would leak here if it were a plain pointer
    }
    catch (const std::exception &)
    {
        std::cout << "Caught exception\n";
    }

    std::cout << "\n"
                 "5) Array form of unique_ptr demo\n";
    {
        std::unique_ptr<D[]> p(new D[3]);
    } // “D::~D()” is called 3 times

    std::cout << "\n"
                 "6) Linked list demo\n";
    {
        List wall;
        const int enough{1'000};
        for (int beer = 0; beer != enough; ++beer)
            wall.push(beer);
        std::cout << enough << " bottles of beer on the wall...\n";
    } // destroys all the beers

    std::cout << "\n"
                 "7) make_unique:\n";
    make_unique();
}

struct Vec3
{
    int x, y, z;

    // Following constructor is no longer needed since C++20.
    Vec3(int x = 0, int y = 0, int z = 0) noexcept : x(x), y(y), z(z) {}

    friend std::ostream &operator<<(std::ostream &os, const Vec3 &v)
    {
        return os << "{ x=" << v.x << ", y=" << v.y << ", z=" << v.z << " }";
    }
};

// Output Fibonacci numbers to an output iterator.
template <typename OutputIt>
OutputIt fibonacci(OutputIt first, OutputIt last)
{
    for (int a = 0, b = 1; first != last; ++first)
    {
        *first = b;
        b += std::exchange(a, b);
    }
    return first;
}

void make_unique() noexcept
{
    // Use the default constructor.
    std::unique_ptr<Vec3> v1 = std::make_unique<Vec3>();
    // Use the constructor that matches these arguments.
    std::unique_ptr<Vec3> v2 = std::make_unique<Vec3>(0, 1, 2);
    // Create a unique_ptr to an array of 5 elements.
    std::unique_ptr<Vec3[]> v3 = std::make_unique<Vec3[]>(5);

    // Create a unique_ptr to an uninitialized array of 10 integers,
    // then populate it with Fibonacci numbers.
    std::unique_ptr<int[]> i1 = std::make_unique_for_overwrite<int[]>(10);
    fibonacci(i1.get(), i1.get() + 10);

    std::cout << "make_unique<Vec3>():      " << *v1 << '\n'
              << "make_unique<Vec3>(0,1,2): " << *v2 << '\n'
              << "make_unique<Vec3[]>(5):   ";
    for (std::size_t i = 0; i < 5; ++i)
        std::cout << std::setw(i ? 30 : 0) << v3[i] << '\n';
    std::cout << '\n';

    std::cout << "make_unique_for_overwrite<int[]>(10), fibonacci(...): [" << i1[0];
    for (std::size_t i = 1; i < 10; ++i)
        std::cout << ", " << i1[i];
    std::cout << "]\n";
}
// NOLINTEND