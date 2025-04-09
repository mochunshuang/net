#include <iostream>
// NOLINTBEGIN

namespace ip
{
    struct address_v6
    {
    };
    struct address_v4
    {
    };

    struct address
    {
        address() = default;

        template <typename Version>
        constexpr address(Version) noexcept
        {
        }
    };
} // namespace ip

struct protocol_type
{
    enum protocol : uint8_t
    {
        IPV4,
        IPV6
    };

    // 静态编译时方法
    static consteval protocol v6() noexcept
    {
        return IPV6;
    }
    static consteval protocol v4() noexcept
    {
        return IPV4;
    }

    // 允许直接比较枚举值
    constexpr bool operator==(protocol other) const noexcept
    {
        return value_ == other;
    }

    // 构造函数（要可以可隐式转换）
    constexpr protocol_type(protocol p = IPV4) noexcept : value_(p) {}

  private:
    protocol value_{};
};

// 端点类示例
class basic_endpoint
{
  public:
    using port_type = uint16_t;

    // 满足接口要求：proto == protocol_type::v4() || proto == protocol_type::v6()
    constexpr basic_endpoint(const protocol_type &proto, port_type port_num) noexcept
    {
        if (proto == protocol_type::v6())
        {
            address_ = ip::address_v6(); // 假设存在这些类型
        }
        else
        {
            address_ = ip::address_v4();
        }
        port_ = port_num;
    }

  private:
    ip::address address_; // 假设的地址类型
    port_type port_;
};

int main()
{
    {
        [[maybe_unused]] constexpr auto ep1 =
            basic_endpoint(protocol_type::v4(), 80); // 正确
        [[maybe_unused]] constexpr auto ep2 =
            basic_endpoint(protocol_type::IPV6, 443); // 也正确（通过隐式转换）
        [[maybe_unused]] constexpr auto ep3 = basic_endpoint(protocol_type(), 80);
        [[maybe_unused]] constexpr auto ep4 =
            basic_endpoint(protocol_type(protocol_type::protocol::IPV6), 80);

        constexpr auto v = protocol_type::v4();
        static_assert(v == protocol_type::IPV4);
        static_assert(v == protocol_type::v4());

        constexpr protocol_type t{};
        static_assert(v == t);
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND