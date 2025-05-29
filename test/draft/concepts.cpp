
#include <cstdint>
#include <iostream>

template <typename T>
concept result_check = requires(T t) {
    { t.send() } noexcept;
};

template <typename T>
concept context = requires(T t) {
    { t.lisen() } noexcept;
    requires result_check<decltype(t.lisen())>;
};

// 简化
template <typename T>
concept context2 = requires(T t) {
    { t.lisen() } noexcept -> result_check;
};

struct io_context
{
    struct Listener
    {
        void send() noexcept {}
    };
    auto lisen() noexcept // NOLINT
    {
        return Listener{};
    }
};

static_assert(context<io_context>);

static_assert(context2<io_context>);

struct error_context
{

    void lisen() noexcept // NOLINT
    {
    }
};

static_assert(not context<error_context>);

// 默认放在第一个还是第二个
template <typename A, typename B>
concept min = [] consteval {
    return sizeof(A) < sizeof(B);
}();

static_assert(min<int8_t, int16_t>);

struct A
{
    int method() noexcept // NOLINT
    {
        return 0;
    }
};

template <typename T>
concept min_uint64 = requires(T &t) {
    { t.method() } -> min<uint64_t>;
};

template <typename T>
concept min_uint8 = requires(T &t) {
    { t.method() } -> min<uint8_t>;
};

static_assert(not min_uint8<A>);

// NOTE: 返回值，作为第一个参数
static_assert(min_uint64<A>);

template <typename A, typename... left>
concept equal_size = [] consteval {
    return sizeof(A) == (sizeof(left) + ... + 0);
}();

template <typename T>
concept equal_2uint16 = requires(T &t) {
    { t.method() } -> equal_size<uint16_t, uint16_t>;
};

template <typename T>
concept equal_uint16 = requires(T &t) {
    { t.method() } -> equal_size<uint16_t>;
};

template <typename T>
concept equal_4uint8 = requires(T &t) {
    { t.method() } -> equal_size<uint8_t, uint8_t, uint8_t, uint8_t>;
};

static_assert(equal_2uint16<A>);
static_assert(equal_4uint8<A>);
static_assert(not equal_uint16<A>);

int main()
{
    std::cout << "main done\n";
    return 0;
}