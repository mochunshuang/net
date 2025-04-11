

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <array>

#include <cassert>
#include <iostream>

// NOLINTBEGIN

int main()
{
    using namespace mcs::ABNF::URI; // NOLINT
    {
        // NOTE: 不支持
        //  constexpr bool k_table[256] = {
        //      ['!'] = true, ['$'] = true, ['&'] = true, ['\''] = true, // NOLINT
        //      ['('] = true, [')'] = true, ['*'] = true, ['+'] = true,
        //      [','] = true, [';'] = true, ['='] = true};
        {
            constexpr std::array<bool, 256> sub_delims = []() consteval {
                std::array<bool, 256> arr = {};
                arr['!'] = true;
                arr['$'] = true;
                arr['&'] = true;
                arr['\''] = true;
                arr['('] = true;
                arr[')'] = true;
                arr['*'] = true;
                arr['+'] = true;
                arr[','] = true;
                arr[';'] = true;
                arr['='] = true;
                return arr;
            }();
            static_assert(sub_delims[')']);
        }
        {
            // Array designators are a C99 extensionclang(-Wc99-designator)
            //  constexpr bool sub_delims[256] = {[0x41] = true};
            //  static_assert(sub_delims[')']);
        }
        {

            // 不允许
            //  constexpr bool sub_delims[256] = []() consteval {
            //      bool arr[256]{};
            //      for (const auto &i :
            //           {'!', '$', '&', '\'', '(', ')', '*', '*', '+', ',', ';', '='})
            //      {
            //          arr[i] = true;
            //      }
            //      return arr;
            //  }();
            //  static_assert(sub_delims[')']);
        }
        {

            constexpr std::array<bool, 256> sub_delims = []() consteval {
                std::array<bool, 256> arr{};
                for (const auto &i :
                     {'!', '$', '&', '\'', '(', ')', '*', '*', '+', ',', ';', '='})
                {
                    arr[i] = true;
                }
                return arr;
            }();
            static_assert(sub_delims[')']);
        }
    }
    TEST("test_path_rootless") = [] {
        // 有效用例
        {
            std::array<uint8_t, 1> input{'a'};
            auto result = path_rootless(input);
            EXPECT(result.has_value() && result->count == 1);
        }

        {
            std::array<uint8_t, 3> input{'a', '/', 'b'};
            auto result = path_rootless(input);
            EXPECT(result.has_value() && result->count == 3);
        }
        {
            std::array<uint8_t, 4> input{'a', '/', '/', 'b'};
            auto result = path_rootless(input);
            assert(result.has_value() && result->count == 4);
        }
        {
            std::array<uint8_t, 2> input{'a', '/'};
            auto result = path_rootless(input);
            EXPECT(result.has_value() && result->count == 2);
        }

        // 无效用例
        {
            std::array<uint8_t, 0> input{};
            auto result = path_rootless(input);
            assert(!result.has_value() && result.error().index() == 0);
        }
        {
            std::array<uint8_t, 2> input{'/', 'a'};
            auto result = path_rootless(input);
            assert(!result.has_value() && result.error().index() == 0);
        }
        {
            // 假设空格是非法的pchar
            std::array<uint8_t, 5> input{'a', '/', 'b', ' ', 'c'};
            auto result = path_rootless(input);
            assert(!result.has_value() && result.error().index() == 3);
            // 错误在空格位置
            assert(not result);
            std::cout << "err_idnex: " << result.error().index() << '\n';
        }
        {
            // : 是可以的
            std::array<uint8_t, 3> input{':', 'a', 'b'};
            auto result = path_rootless(input);
            assert(result);
        }
    };
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND