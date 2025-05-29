#include <iostream>

// NOLINTBEGIN

template <typename T>
class Processor
{
  public:
    static void process(T *obj)
    {
        // ❌ 错误：无法访问 T 的私有成员（除非 T 将 Processor<T> 声明为友元）
        std::cout << obj->value << std::endl;
    }
};

class Data
{
  private:
    int value = 100;
    friend class Processor<Data>; // ✅ 显式友元声明后合法 [必须，否则非法]
};

int main()
{
    // 使用
    Data data;
    Processor<Data>::process(&data); // 合法（依赖友元声明）

    {
        class Base
        {
          private:
            static void hidden() {}
        };

        class Derived : public Base
        {
          public:
            static void test()
            {
                // ❌ 错误：无法访问基类的私有静态函数
                // Base::hidden();
            }
        };
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND