#include <any>
#include <cassert>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>

using octet = std::uint8_t;
using data_view = std::span<const octet>;
using octets_view_in = const std::span<const octet> &;
struct parser_ctx
{
    using parser_objet = std::any *(octets_view_in) noexcept;
    data_view data;
    parser_objet *impl{};
};
static_assert(sizeof(std::span<octet>) == 16); // NOLINT
static_assert(sizeof(parser_ctx) == 24);       // NOLINT

static_assert(sizeof(std::vector<octet>) == 24); // NOLINT

template <class _Ty>
constexpr bool _Any_is_small = // NOLINT
    alignof(_Ty) <= alignof(max_align_t) && std::is_nothrow_move_constructible_v<_Ty> &&
    sizeof(_Ty) <= 48;

// NOTE: 如果保存指针的值： 永远是  sizeof(void *) 大小

int main()
{
    /**
     * @brief 小对象满足：
template <class _Ty>
constexpr bool _Any_is_small = alignof(_Ty) <= alignof(max_align_t)
                            && is_nothrow_move_constructible_v<_Ty> && sizeof(_Ty) <=
_Any_small_space_size;

max_align_t:
_EXPORT_STD using max_align_t = double; // most aligned type

_Any_small_space_size:
inline constexpr size_t _Any_small_space_size = (_Small_object_num_ptrs - 2) *
sizeof(void*);
     *
     */
    std::cout << std::boolalpha;

    // any 类型
    std::any a = 1;
    std::cout << a.type().name() << ": " << std::any_cast<int>(a) << '\n';
    a = 3.14; // NOLINT
    std::cout << a.type().name() << ": " << std::any_cast<double>(a) << '\n';
    a = true;
    std::cout << a.type().name() << ": " << std::any_cast<bool>(a) << '\n';

    // 有误的转型
    try
    {
        a = 1;
        std::cout << std::any_cast<float>(a) << '\n';
    }
    catch (const std::bad_any_cast &e)
    {
        std::cout << e.what() << '\n';
    }

    // 拥有值
    a = 2;
    if (a.has_value())
        std::cout << a.type().name() << ": " << std::any_cast<int>(a) << '\n';

    // 重置
    a.reset();
    if (!a.has_value())
        std::cout << "没有值\n";

    // 指向所含数据的指针
    a = 3;
    int *i = std::any_cast<int>(&a);

    {
        std::vector<std::any> v;
        v.emplace_back(1);
        v.emplace_back(std::string{"string"});

        auto v0 = std::any_cast<int>(v[0]);
        auto v1 = std::any_cast<std::string>(v[1]);
        assert(v0 == 1);
        assert(v1 == std::string_view{"string"});
    }
    std::cout << *i << '\n';
}