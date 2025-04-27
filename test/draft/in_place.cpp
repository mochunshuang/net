#include <tuple>
#include <string>
#include <utility>

struct NoCopyMove
{
    NoCopyMove(int) {}
    NoCopyMove(const NoCopyMove &) = delete;
    NoCopyMove(NoCopyMove &&) = delete;
};

int main()
{
    // NOTE: 不可能。起码需要移动构造
    // 注意：需要C++17及以上版本支持分段构造
    // std::tuple<std::string, NoCopyMove> t(
    //     std::piecewise_construct,
    //     std::forward_as_tuple("hello"), // 构造std::string
    //     std::forward_as_tuple(42)       // 构造NoCopyMove
    // );
}