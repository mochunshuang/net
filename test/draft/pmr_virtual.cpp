#include <cassert>
#include <memory_resource>
#include <iostream>
#include <vector>

// NOLINTBEGIN

// 有虚函数的基类
class Base
{
  public:
    Base()
    {
        std::cout << "Base 构造 @ " << this << "\n";
    }
    virtual ~Base()
    {
        std::cout << "Base 析构 @ " << this << "\n";
    }
    virtual void identify() const
    {
        std::cout << "我是 Base @ " << this << "\n";
    }
};

#define derived_has_member 0

// 派生类
class Derived : public Base
{
    // 当没有成员时。指针偏移量一样
    // index ptr of 0,1,1,3: 3112842417320,3112842417328,3112842417336,3112842417344
    // index ptr of 0,1,1,3: 2446050448552,2446050449856,2446050449872,2446050448560
#if derived_has_member
    int v;
#endif
  public:
    Derived()
    {
        std::cout << "Derived 构造 @ " << this << "\n";
    }
    ~Derived() override
    {
        std::cout << "Derived 析构 @ " << this << "\n";
    }
    void identify() const override
    {
        std::cout << "我是 Derived @ " << this << "\n";
    }
};

// 安全创建和销毁工具
template <typename T, typename... Args>
T *create_object(std::pmr::memory_resource &pool, Args &&...args)
{

    void *mem = pool.allocate(sizeof(T), alignof(T));
    return new (mem) T(std::forward<Args>(args)...);
}

template <typename T>
void destroy_object(T *obj, std::pmr::memory_resource &pool)
{
    if (obj)
    {
        obj->~T();
        pool.deallocate(obj, sizeof(T), alignof(T));
    }
}

int main()
{
    // 创建内存池资源
    std::pmr::unsynchronized_pool_resource pool;

    std::cout << "===== 测试1: 创建和销毁基本对象 =====\n";
    {
        // 创建基类对象
        Base *base = create_object<Base>(pool);
        base->identify(); // 调用虚函数

        // 销毁对象
        destroy_object(base, pool);
    }

    std::cout << "\n===== 测试2: 多态对象创建和销毁 =====\n";
    {
        // 创建派生类对象但使用基类指针
        Base *derived = create_object<Derived>(pool);
        derived->identify(); // 正确调用派生类的虚函数

        // 销毁对象 - 多态析构
        destroy_object(derived, pool);
    }

    std::cout << "\n===== 测试3: 内存重用和虚函数调用 =====\n";
    {
        // 创建并销毁对象
        Derived *obj1 = create_object<Derived>(pool);
        obj1->identify();
        destroy_object(obj1, pool);

        // NOTE: 证明了，确实是选一块内存 能放得下的 内存
        std::cout << "=====Base重用Derived内存\n";
        // 重用相同内存
        Base *obj2 = create_object<Base>(pool);
        obj2->identify(); // 安全调用虚函数
        destroy_object(obj2, pool);
    }

    std::cout << "\n===== 测试4: 容器中的对象 =====\n";
    {
        // 使用内存池创建对象向量
        std::vector<Base *> objects;

        // 创建混合类型对象
        std::cout << "=====混合创建Base,Derived\n";
        objects.push_back(create_object<Base>(pool));
        objects.push_back(create_object<Derived>(pool));
        objects.push_back(create_object<Derived>(pool));
        objects.push_back(create_object<Base>(pool));

        // NOTE:
        std::cout << "index ptr of 0,1,1,3: " << (ptrdiff_t)objects[0] << ","
                  << (ptrdiff_t)objects[1] << "," << (ptrdiff_t)objects[2] << ","
                  << (ptrdiff_t)objects[3] << "," << '\n';

#if derived_has_member
        // NOTE: 不共享 一个 chunk 内存块。
        static_assert(sizeof(Base) < sizeof(Derived));
        assert((ptrdiff_t)objects[2] - (ptrdiff_t)objects[1] == sizeof(Derived));
        assert((ptrdiff_t)objects[3] - (ptrdiff_t)objects[0] == sizeof(Base));
#else
        // NOTE: 共享 一个 chunk 内存块。 他们是连续的
        static_assert(sizeof(Base) == sizeof(Derived));
        assert((ptrdiff_t)objects[2] - (ptrdiff_t)objects[1] == sizeof(Derived));
        assert((ptrdiff_t)objects[3] - (ptrdiff_t)objects[0] == 3 * sizeof(Base));

        assert((ptrdiff_t)objects[1] - (ptrdiff_t)objects[0] == sizeof(Derived));
        assert((ptrdiff_t)objects[2] - (ptrdiff_t)objects[1] == sizeof(Derived));
        assert((ptrdiff_t)objects[3] - (ptrdiff_t)objects[2] == sizeof(Derived));
#endif

        std::cout << "=====多态调用虚函数\n";
        // 多态调用虚函数
        for (Base *obj : objects)
        {
            obj->identify();
        }

        std::cout << "=====销毁所有对象\n";
        // 正确销毁所有对象
        for (Base *obj : objects)
        {
            destroy_object(obj, pool);
        }
    }

    // NOTE: Derived + Base 的地址是一样的。运行时是这样

    return 0;
}
// NOLINTEND