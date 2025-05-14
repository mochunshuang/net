#include <array>
#include <iostream>
#include <cstddef>
#include <algorithm>

// 定义接口概念
template <typename T>
concept Drawable = requires(const T &obj) {
    { obj.draw() } -> std::same_as<void>;
};

// 类型擦除包装器
template <std::size_t MaxSize>
class PolymorphicObject
{
    alignas(std::max_align_t) std::byte storage_[MaxSize];
    using VTable = void (*)(const std::byte *);
    VTable vtable_;

    template <typename T>
    static constexpr VTable create_vtable()
    {
        return [](const std::byte *data) constexpr {
            const T &obj = *static_cast<const T *>(static_cast<const void *>(data));
            obj.draw();
        };
    }

  public:
    template <typename T>
        requires(sizeof(T) <= MaxSize && Drawable<T>)
    constexpr PolymorphicObject(T &&obj) noexcept : vtable_(create_vtable<T>())
    {
        new (storage_) T(std::forward<T>(obj));
    }

    constexpr void draw() const
    {
        vtable_(storage_);
    }
};

// 具体类型定义
struct Circle
{
    constexpr void draw() const
    {
        std::cout << "○\n";
    }
};

struct Square
{
    constexpr void draw() const
    {
        std::cout << "□\n";
    }
};

// 辅助函数推导最大尺寸并创建数组
template <typename... Ts>
constexpr auto make_polymorphic_array(Ts &&...objs)
{
    constexpr std::size_t max_size = std::max({sizeof(Ts)...});
    return std::array<PolymorphicObject<max_size>, sizeof...(Ts)>{
        PolymorphicObject<max_size>(std::forward<Ts>(objs))...};
}

int main()
{
    
    auto shapes = make_polymorphic_array(Circle{}, Square{}, Circle{});

    for (const auto &shape : shapes)
    {
        shape.draw();
    }
}