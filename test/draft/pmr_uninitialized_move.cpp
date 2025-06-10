#include <iostream>
#include <memory>
#include <vector>

// NOLINTBEGIN

class Movable
{
  public:
    std::unique_ptr<int> data;
    Movable(int val) : data(new int(val)) {}
    Movable(Movable &&other) noexcept : data(std::move(other.data))
    {
        std::cout << "移动构造: " << *data << "\n";
    }
};

class Resource
{
    int *data;

  public:
    explicit Resource(int size) : data(new int[size])
    {
        std::cout << "Resource allocated\n";
    }
    Resource(Resource &&other) noexcept : data(other.data) // NOTE: 资源浅复制
    {
        other.data = nullptr;
        std::cout << "Resource moved\n";
    }
    ~Resource()
    {
        if (data)
        {
            delete[] data;
            std::cout << "Resource freed\n";
        }
    }
};

void test_resource()
{
    std::vector<Resource> source;
    source.emplace_back(10);
    source.emplace_back(20);
    source.emplace_back(30);

    std::cout << "====allocate start\n";
    // 分配未初始化的内存
    auto p = std::allocator<Resource>().allocate(source.size());
    std::cout << "====allocate end\n";

    std::cout << "====uninitialized_move start\n";
    // 移动构造到未初始化内存
    std::uninitialized_move(source.begin(), source.end(), p);
    std::cout << "====uninitialized_move end\n";

    // 源对象现在应该处于有效但未指定状态
    std::cout << "Source size: " << source.size() << ",but dont used\n";

    std::cout << "====destroy_n start\n";
    // 销毁目标对象
    std::destroy_n(p, source.size());
    std::cout << "====destroy_n end\n";
    // 释放内存
    std::allocator<Resource>().deallocate(p, source.size());
}

int main()
{
    std::vector<Movable> src;
    src.emplace_back(100);
    src.emplace_back(200); // NOTE: 直接设置值，6666

    alignas(Movable) unsigned char raw[sizeof(Movable) * 2];
    Movable *dest = reinterpret_cast<Movable *>(raw);

    std::cout << "========uninitialized_move\n";
    // 移动对象
    auto end = std::uninitialized_move(src.begin(), src.end(), dest);
    src.clear(); // 显式清空源对象

    std::cout << "目标对象值: " << *dest[0].data << ", " << *dest[1].data << "\n";
    std::destroy(dest, end);

    std::cout << "========test_resource\n";
    test_resource();
}
// NOLINTEND