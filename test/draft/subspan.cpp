#include <array>
#include <cassert>
#include <iostream>
#include <span>
#include <vector>
// NOLINTBEGIN
struct NonCopyableType
{
    int value;

    NonCopyableType(int v) : value(v) {}
    NonCopyableType(const NonCopyableType &) = delete; // 禁用拷贝构造
    NonCopyableType(NonCopyableType &&) = default;     // 显式允许移动构造
    NonCopyableType &operator=(const NonCopyableType &) = delete;
};

void processSubspan(const std::span<NonCopyableType> data)
{
    for (const auto &item : data)
    {
        std::cout << item.value << " ";
    }
    std::cout << "\n";
}

int main()
{
    std::vector<NonCopyableType> vec;
    vec.emplace_back(10);
    vec.emplace_back(20);
    vec.emplace_back(30);
    vec.emplace_back(40);

    std::span<NonCopyableType> original(vec);
    auto sub = original.subspan(1, 2); // 取 [20, 30]

    processSubspan(sub); // 输出: 20 30

    vec[1].value = 99;
    vec[2].value = 100;

    // NOTE: span 只是视图。源修改，也会看到变化
    processSubspan(sub); // 输出: 99 100

    {
        // NOTE: 无论怎么样 subspan 是可以调用的，因为是 const接口
        struct Data
        {
            int value;
            Data(int v) : value(v) {}
        };
        {
            auto testSubspanReadOnly = [](const std::span<const Data> &data) {
                auto sub = data.subspan(1, 2); // 子范围
                for (const auto &item : sub)
                {
                    std::cout << item.value << " ";
                }
                std::cout << "\n";

                // sub[0].value = 99; // ❌ 不能修改内容
                sub = data.subspan(0, 1); // 可以 subspan
                std::vector<Data> vec = {10000, 20000, 30000, 40000, 50000};
                // data = vec; // ❌ 错误
            };
            std::vector<Data> vec = {10000, 20000, 30000, 40000, 50000};
            testSubspanReadOnly(vec); // 输出: 20000 30000
        }
        {
            auto func = [](const std::span<Data> &data) {
                auto sub = data.subspan(1, 2); // 子范围
                sub[0].value = 99;             // 可以修改指向的值
                std::vector<Data> vec = {10000, 20000, 30000, 40000, 50000};
                // data = vec; // ❌错误
            };
            std::vector<Data> vec = {10000, 20000, 30000, 40000, 50000};
            func(vec);
        }
        {
            // NOTE: std::span<Data> &data 是错误的
            auto func = [](std::span<Data> &data) {
                auto sub = data.subspan(1, 2); // 子范围
                sub[0].value = 99;             // 可以修改指向的值
                std::vector<Data> vec = {10000, 20000, 30000, 40000, 50000};
                data = vec; // 可以修改data 指向
            };
            std::vector<Data> vec = {10000, 20000, 30000, 40000, 50000};
            // func(vec);// ❌错误
        }
    }

    // NOTE: subspan可能为空。 有未定义行为
    // 如果 offset > Extent || (Count != dynamic_extent && Count > Extent -
    // offset)，行为未定义。
    {
        int arr[] = {1, 2, 3, 4, 5};
        std::span<int> s(arr);

        // 情况 1: offset 超出范围 → 空
        auto sub1 = s.subspan(6);
        std::cout << "sub1.empty(): " << sub1.empty() << "\n"; // 1 (true)

        // 情况 2: count=0 → 空
        auto sub2 = s.subspan(2, 0);
        std::cout << "sub2.empty(): " << sub2.empty() << "\n"; // 1 (true)

        // 情况 3: offset + count 超出范围 → 空
        auto sub3 = s.subspan(3, 3);
        std::cout << "sub3.empty(): " << sub3.empty() << "\n"; // 1 (true)

        // 情况 4: 合法参数 → 非空
        auto sub4 = s.subspan(1, 2);
        std::cout << "sub4.empty(): " << sub4.empty() << "\n"; // 0 (false)
        for (int x : sub4)
            std::cout << x << " "; // 2 3
    }

    // 安全用法：显式检查参数合法性
    auto safe_subspan = [](std::span<int> sp, std::size_t offset,
                           std::size_t count = std::dynamic_extent) noexcept {
        // 优先检查offset合法性
        if (offset > sp.size())
            return std::span<int>{};

        // 修正条件：允许count == 0,k_remaining的特殊情况
        const auto k_available = sp.size() - offset;
        const auto k_actual_count = (count == std::dynamic_extent) ? k_available
                                    : (count == 0)
                                        ? 0
                                        : std::min(count, k_available); // 关键修正点

        return sp.subspan(offset, k_actual_count);
    };
    {
        int arr[] = {1, 2, 3, 4, 5};
        std::span<int> s(arr);

        auto sub1 = safe_subspan(s, 6, 3);
        std::cout << "sub1.empty(): " << sub1.empty() << "\n"; // 1 (true)

        auto sub3 = safe_subspan(s, 3, 3);
        std::cout << "sub3.empty(): " << sub3.empty() << "\n"; // 0 (false)

        // 如果count==0
        auto sub4 = safe_subspan(s, 3, 0);
        std::cout << "sub4.empty(): " << sub4.empty() << "\n"; // 1 (true)

        assert(sub1.empty());
        assert(not sub3.empty());
        assert(sub4.empty());

        assert(not safe_subspan(s, 3).empty());

        auto sub6 = safe_subspan(s, 5, 0);
        assert(sub6.empty() && sub6.size() == 0); // 通过
    }

    // NOTE: 提供的变化
    {
        {
            int arr[] = {1, 2, 3, 4, 5};
            std::span<int> s(arr);
            auto sub01 = safe_subspan(s, 0, 1);
            assert(sub01[0] == s[0]);
            assert(sub01.size() == 1);
            // NOTE: 空： 递归终止
            auto sub00 = safe_subspan(s, 0, 0);
            assert(sub00.size() == 0);

            // size
            auto sub03 = safe_subspan(s, 0, 3);
            assert(sub03.size() == 3);
            // NOTE: 映射对应
            auto sub13 = safe_subspan(s, 1, 3);
            assert(sub13.size() == 3);

            // NOTE: count 是包括起始位置的
            assert(sub13[0] == s[0 + 1]);
            assert(sub13[1] == s[1 + 1]);
            assert(sub13[2] == s[2 + 1]);
            {
                // NOTE: offset + count <=5 是必须的，同时 count是多少就是多少
                assert(s.size() == 5);
                assert(safe_subspan(s, 0, 5).size() == 5);
                assert(safe_subspan(s, 1, 4).size() == 4);
                assert(safe_subspan(s, 2, 3).size() == 3);
                assert(safe_subspan(s, 3, 2).size() == 2);
                assert(safe_subspan(s, 4, 1).size() == 1);
                assert(safe_subspan(s, 5, 0).size() == 0);
                // NOTE: index 必须小于 size
                // assert(s[s.size()]); // 崩溃
                assert(s[s.size() - 1]);
            }

            // NOTE: 视图: 指针数组，可能失效
            s[0 + 1] = 9;
            assert(sub13[0] == s[0 + 1]);
            assert(sub13[0] == 9);

            sub13[0] = 6;
            assert(sub13[0] == s[0 + 1]);
            assert(sub13[0] == 6);
        }
    }
    {
        static constexpr std::array<int, 10> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        constexpr std::span<const int> s(arr);
        constexpr auto sub0 = s.subspan(5, 3);
        static_assert(sub0[0] == 6);
        static_assert(sub0[2] == 8);

        constexpr auto sub00 = sub0.subspan(1, 2);
        static_assert(sub00[1] == 8);

        constexpr const auto &a = sub00[1];
        static_assert(&a == &sub0[2]);
    }

    return 0;
}
// NOLINTEND