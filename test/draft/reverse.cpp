#include <array>
#include <cassert>
#include <ranges>
#include <algorithm>
#include <iostream>

int main()
{
    std::array arr{'[', '1', '?', '2', '#', '3', ']'};
    std::span sp(arr);

    auto i0 = std::ranges::find(sp, '?');
    assert(i0 - sp.begin() == 2);
    if (i0 != sp.end())
    {
        auto reverse_view = std::ranges::subrange(i0, sp.end()) | std::views::reverse;
        auto i1_reverse = std::ranges::find(reverse_view, '#');
        if (i1_reverse != reverse_view.end())
        {
            auto i1 = i1_reverse.base() - 1;
            std::cout << "Found '#' at position: " << std::distance(sp.begin(), i1)
                      << '\n';

            assert(i1 - sp.begin() == 4);
        }
    }
    // NOTE: 以上内容编译期不友好:
    {
        std::array arr{'[', '1', '?', '2', '#', '3', ']'};
        const auto k_size = sp.size();
        size_t idx_0 = k_size;
        size_t idx_1 = k_size;

        // 查找第一个 '@'
        for (size_t i = 0; i < k_size; ++i)
        {
            if (sp[i] == '?')
            {
                idx_0 = i;
                break;
            }
        }
        // 在 '@' 之后的部分查找最后一个 ':'
        for (size_t i = 0; i < k_size; ++i)
        {
            if (sp[i] == '?')
            {
                idx_0 = i;
                break;
            }
        }
        // NOTE: idx_0 + 1 因为知道肯定是 不同的字符串
        for (size_t left = (idx_0 < k_size ? idx_0 + 1 : 0), right = k_size;
             right-- > left;)
        {
            if (sp[right] == '#')
            {
                idx_1 = right;
                break;
            }
        }

        assert(idx_0 == 2);
        assert(idx_1 == 4);

        auto front = sp.first(idx_0);
        assert(std::string(front.begin(), front.end()) == "[1");

        auto tail = sp.subspan(idx_1 + 1);
        assert(std::string(tail.begin(), tail.end()) == "3]");

        auto mid = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
        assert(std::string(mid.begin(), mid.end()) == "2");
    }
    {
        size_t n = 0;
        for (size_t i = n; i-- > 0;)
        {
            // 循环内使用 i
            assert(false); // 不会死循环
        }
        {
            // NOTE: 反向循环经典代码
            size_t n = 1;
            for (size_t i = n; i-- > 0;)
            {
                assert(i == 0);
            }
        }
    }
    return 0;
}