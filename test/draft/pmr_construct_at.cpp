#include <iostream>
#include <memory>
#include <string>

// NOLINTBEGIN

class Book
{
    std::string title;
    int pages;

  public:
    Book(std::string t, int p) : title(std::move(t)), pages(p)
    {
        std::cout << "Book '" << title << "' created\n";
    }
    ~Book()
    {
        std::cout << "Book '" << title << "' destroyed\n";
    }
    void describe() const
    {
        std::cout << "'" << title << "' has " << pages << " pages\n";
    }
};

int main()
{
    // NOTE: 手动管理对象的生命周期，construct_at和destroy_n提供了细粒度的控制
    //  分配未初始化的内存
    std::cout << "========allocate start" << "\n";
    auto p = std::allocator<Book>().allocate(2);
    std::cout << "========allocate end" << "\n";

    // 在指定位置构造对象
    std::cout << "========construct_at start" << "\n";
    std::construct_at(p, "The C++ Programming Language", 1368);
    std::construct_at(p + 1, "Effective Modern C++", 336);
    std::cout << "========construct_at end" << "\n";

    std::cout << "========describe start" << "\n";
    // 使用对象
    p[0].describe();
    p[1].describe();
    std::cout << "========describe end" << "\n";

    // 销毁对象
    std::cout << "========destroy_n start" << "\n";
    std::destroy_n(p, 2);
    std::cout << "========destroy_n end" << "\n";

    // 重新构造不同对象
    std::cout << "\n========reused============" << "\n";
    std::construct_at(p, "Clean Code", 464);
    std::construct_at(p + 1, "Design Patterns", 395);

    // 再次使用
    p[0].describe();
    p[1].describe();

    // 最终销毁和释放
    std::destroy_n(p, 2);
    std::allocator<Book>().deallocate(p, 2);
}
// NOLINTEND