#include <iostream>

// 测试场景1：无虚析构函数 ============================
struct BaseNoVirtual
{
    char type = 'B';
    ~BaseNoVirtual()
    {
        std::cout << "~BaseNoVirtual\n";
    }
};

struct DerivedNoVirtual : BaseNoVirtual
{
    int *resource = new int(42); // 需要释放的资源
    ~DerivedNoVirtual()
    {
        delete resource; // 这个永远不会被执行！
        std::cout << "~DerivedNoVirtual\n";
    }
};

// 测试场景2：有虚析构函数 =============================
struct BaseWithVirtual
{
    char type = 'B';
    virtual ~BaseWithVirtual()
    {
        std::cout << "~BaseWithVirtual\n";
    }
};

struct DerivedWithVirtual : BaseWithVirtual
{
    int *resource = new int(42);
    ~DerivedWithVirtual() override
    {
        delete resource; // 正确释放
        std::cout << "~DerivedWithVirtual\n";
    }
};

// 测试函数 ===========================================
void test_destruction(BaseNoVirtual *b1, BaseWithVirtual *b2)
{
    // 正确的手动类型转换（假设我们知道具体类型）
    auto d1 = static_cast<DerivedNoVirtual *>(b1);
    auto d2 = static_cast<DerivedWithVirtual *>(b2);

    delete d1; // 正确执行派生类析构函数
    delete d2; // 正确执行派生类析构函数
}

void test_real_world_scenario()
{
    // 真实场景：通过基类指针删除
    BaseNoVirtual *obj1 = new DerivedNoVirtual();
    BaseWithVirtual *obj2 = new DerivedWithVirtual();

    std::cout << "\n真实场景测试：\n";
    delete obj1; // 危险操作！
    delete obj2; // 安全操作
}

int main()
{
    // 构造测试对象
    auto *obj1 = new DerivedNoVirtual();
    auto *obj2 = new DerivedWithVirtual();
    std::cout << "开始测试析构行为...\n";
    test_destruction(obj1, obj2);
    std::cout << "测试结束\n";

    // 新增真实场景测试
    test_real_world_scenario();
}