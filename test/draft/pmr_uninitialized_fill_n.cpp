#include <iostream>
#include <memory>
#include <string>

// NOLINTBEGIN

class Logger
{
    std::string id;

  public:
    Logger(std::string_view s) : id(s)
    {
        std::cout << "Logger " << id << " constructed\n";
    }
    ~Logger()
    {
        std::cout << "Logger " << id << " destroyed\n";
    }
    void log(std::string_view msg) const
    {
        std::cout << "[" << id << "] " << msg << "\n";
    }
};

int main()
{
    // 分配未初始化的内存
    auto p = std::allocator<Logger>().allocate(5);

    // NOTE: 优化，，仅仅调用一次 默认构造和析构。按 bit 复制应该是这样
    //  在未初始化内存中构造多个相同对象
    std::uninitialized_fill_n(p, 5, Logger{"default"});

    // 使用这些对象
    for (int i = 0; i < 5; ++i)
    {
        p[i].log("Hello from default logger");
    }

    std::cout << " destroy_n start\n";
    // 手动销毁
    std::destroy_n(p, 5);
    std::cout << " destroy_n end\n";

    // 释放内存
    std::allocator<Logger>().deallocate(p, 5);
}
// NOLINTEND