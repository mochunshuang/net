#include <any>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <span>

using octet = std::uint8_t;
using octets_view = std::span<const octet>;
using octets_view_in = const std::span<const octet> &;

struct parser_ctx
{
    octets_view root_span;
    std::size_t cur_index{};
    std::size_t end_index{};
};

constexpr static parser_ctx make_parser_ctx(octets_view_in root_span) noexcept // NOLINT
{
    return {.root_span = root_span, .cur_index = 0, .end_index = root_span.size()};
}

struct const_parser_ctx
{
    const octets_view &root_span; // NOLINT
    const std::size_t &cur_index; // NOLINT
    const std::size_t &end_index; // NOLINT
    explicit const_parser_ctx(const parser_ctx &ctx) noexcept
        : root_span(ctx.root_span), cur_index(ctx.cur_index), end_index(ctx.end_index)
    {
    }
    const_parser_ctx() = delete;
    [[nodiscard]] auto size() const noexcept
    {
        return end_index - cur_index;
    }
    [[nodiscard]] bool empty() const noexcept
    {
        return end_index <= cur_index;
    }
};

struct Ruler
{
    constexpr static std::any build(octets_view_in /*sp*/) noexcept
    {
        struct A
        {
        };
        return A{};
    }
};

std::optional<int> test(const_parser_ctx ctx) noexcept // NOLINT
{
    if (ctx.root_span[ctx.cur_index] == 'a')
        return 1;
    return std::nullopt;
}

int main()
{
    {
        std::array<octet, 1> array{'a'};
        // std::span<octet> span = array;
        auto ctx = make_parser_ctx(array);
        assert(ctx.root_span[0] == 'a');
        assert(ctx.cur_index == 0);
        assert(ctx.end_index == 1);

        auto ret = test(const_parser_ctx{ctx});
        assert(ret.has_value() && ret.value() == 1);

        std::size_t count{0};
        if (ret)
        {
            count += ret.value();
        }
        assert(count == 1);

        assert(ctx.root_span[0] == 'a');
        assert(ctx.cur_index == 0);
        assert(ctx.end_index == 1);

        {
            const_parser_ctx c_ctx0(); // NOLINT
            static_assert(std::is_function_v<decltype(c_ctx0)>);

            const_parser_ctx c_ctx{ctx};
            static_assert(not std::is_function_v<decltype(c_ctx)>);
            static_assert(std::is_object_v<decltype(c_ctx)>);
            {
                // const_parser_ctx c_ctx0; //才不行
            }
            // 引用是占用内存大小的
            static_assert(sizeof(const_parser_ctx) == 24); // NOLINT

            static_assert(sizeof(int &) == 4);
        }
    }
    std::cout << "main done\n";
    return 0;
}