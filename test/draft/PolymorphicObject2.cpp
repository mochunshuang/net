#include <array>
#include <algorithm>
#include <bit>
#include <type_traits>

// 编译期类型列表
template <typename... Ts>
struct TypeList
{
};

// 编译期计算最大尺寸
template <typename List>
struct MaxSize;

template <typename... Ts>
struct MaxSize<TypeList<Ts...>>
{
    static constexpr std::size_t value = std::max({sizeof(Ts)...});
};

// 类型擦除容器（自动尺寸推导）
template <typename TypeList>
class PolymorphicStorage
{
};

// 使用示例
struct Circle
{
    constexpr void draw() const
    { /*...*/
    }
};
struct Square
{
    constexpr void draw() const
    { /*...*/
    }
};

// 定义允许的类型列表
using ShapeTypes = TypeList<Circle, Square>;

int main()
{

    return 0;
}