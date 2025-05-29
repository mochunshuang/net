#include <cassert>
#include <iostream>

#if defined(_MSC_VER)

// NOLINTBEGIN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iomanip>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class Endpoint
{
  public:
    // 地址解析结果结构体
    struct ResolvedAddress
    {
        sockaddr_storage storage; // 二进制地址存储
        socklen_t addr_len;       // 地址结构实际长度
        int family;               // 地址族
        std::string ip_str;       // 可读IP字符串
        uint16_t port_host;       // 主机字节序端口
        uint16_t port_net;        // 网络字节序端口
    };

    Endpoint &WithHost(const std::string &host)
    {
        host_ = host;
        return *this;
    }

    Endpoint &WithPort(uint16_t port)
    {
        port_ = port;
        return *this;
    }

    // 完整地址解析（支持IPv4/IPv6）
    ResolvedAddress Resolve(int socktype = SOCK_STREAM) const
    {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = socktype;
        hints.ai_flags = AI_ADDRCONFIG;

        addrinfo *result;
        int status =
            getaddrinfo(host_.empty() ? nullptr : host_.c_str(),
                        port_ ? std::to_string(port_).c_str() : nullptr, &hints, &result);

        if (status != 0)
        {
            throw std::runtime_error(gai_strerrorA(status));
        }

        // 遍历所有结果
        std::vector<ResolvedAddress> addresses;
        for (auto *rp = result; rp != nullptr; rp = rp->ai_next)
        {
            ResolvedAddress addr;
            addr.addr_len = static_cast<socklen_t>(rp->ai_addrlen);
            addr.family = rp->ai_family;
            memcpy(&addr.storage, rp->ai_addr, rp->ai_addrlen);

            // 提取IP字符串
            char ipstr[INET6_ADDRSTRLEN];
            if (addr.family == AF_INET)
            {
                auto *sin = reinterpret_cast<sockaddr_in *>(&addr.storage);
                inet_ntop(AF_INET, &sin->sin_addr, ipstr, sizeof(ipstr));
                addr.port_host = ntohs(sin->sin_port);
                addr.port_net = sin->sin_port;
            }
            else
            {
                auto *sin6 = reinterpret_cast<sockaddr_in6 *>(&addr.storage);
                inet_ntop(AF_INET6, &sin6->sin6_addr, ipstr, sizeof(ipstr));
                addr.port_host = ntohs(sin6->sin6_port);
                addr.port_net = sin6->sin6_port;
            }
            addr.ip_str = ipstr;
            addresses.push_back(addr);
        }

        freeaddrinfo(result);

        if (addresses.empty())
        {
            throw std::runtime_error("No addresses resolved");
        }
        assert(addresses.size() == 1);

        // 选择第一个结果（示例用，实际应提供选择策略）
        return addresses.front();
    }

    // 打印转换细节
    static void PrintResolution(const ResolvedAddress &addr)
    {
        std::cout << "\n=== 地址转换细节 ===\n";
        std::cout << "协议族: " << (addr.family == AF_INET ? "IPv4" : "IPv6") << "\n";
        std::cout << "可读IP: " << addr.ip_str << "\n";
        std::cout << "端口（主机序）: " << addr.port_host << "\n";
        std::cout << "端口（网络序）: 0x" << std::hex << addr.port_net << "\n";

        // 打印二进制地址
        std::cout << "二进制地址: ";
        if (addr.family == AF_INET)
        {
            auto *sin = reinterpret_cast<const sockaddr_in *>(&addr.storage);
            PrintBinary(&sin->sin_addr, sizeof(in_addr));
        }
        else
        {
            auto *sin6 = reinterpret_cast<const sockaddr_in6 *>(&addr.storage);
            PrintBinary(&sin6->sin6_addr, sizeof(in6_addr));
        }
        std::cout << "\n\n";
    }

  private:
    static void PrintBinary(const void *data, size_t length)
    {
        const auto *bytes = static_cast<const unsigned char *>(data);
        for (size_t i = 0; i < length; ++i)
        {
            std::cout << std::setw(2) << std::setfill('0') << std::hex
                      << static_cast<int>(bytes[i]) << " ";
        }
    }

    std::string host_;
    uint16_t port_ = 0;
};

// 增强版测试函数
void TestConversion(const std::string &host, uint16_t port)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    std::cout << "\n---------------------------start------------------------------"
              << '\n';
    try
    {
        Endpoint ep;
        auto addr = ep.WithHost(host).WithPort(port).Resolve();

        std::cout << "原始输入: " << host << ":" << port << "\n";
        Endpoint::PrintResolution(addr);

        // 反向验证
        Endpoint reverse_ep;
        auto reverse_addr =
            reverse_ep.WithHost(addr.ip_str).WithPort(addr.port_host).Resolve();

        std::cout << "反向验证: " << addr.ip_str << ":" << addr.port_host << "\n";
        Endpoint::PrintResolution(reverse_addr);
    }
    catch (const std::exception &e)
    {
        std::cerr << "错误: " << e.what() << "\n";
    }
    std::cout << "---------------------------end------------------------------" << '\n';
    WSACleanup();
}

void RunDualStackServer()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET socks[2] = {socket(AF_INET, SOCK_STREAM, IPPROTO_TCP),
                       socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)};

    // IPv4端点
    Endpoint ep4;
    auto addr4 = ep4.WithHost("0.0.0.0").WithPort(12345).Resolve();
    if (bind(socks[0], (sockaddr *)&addr4.storage, addr4.addr_len) == 0)
    {
        std::cout << "bind socks[0] ok\n";
    }

    // IPv6端点
    Endpoint ep6;
    auto addr6 = ep6.WithHost("::").WithPort(12345).Resolve();
    if (0 == bind(socks[1], (sockaddr *)&addr6.storage, addr6.addr_len))
    {
        std::cout << "bind socks[1] ok\n";
    }

    // 同时监听（实际需用select处理）
    if (listen(socks[0], 5) != SOCKET_ERROR)
    {
        std::cout << "listen socks[0] ok\n";
    }
    if (listen(socks[1], 5) != SOCKET_ERROR)
    {
        std::cout << "listen socks[1] ok\n";
    }

    closesocket(socks[0]);
    closesocket(socks[1]);
    WSACleanup();

    std::cout << "RunDualStackServer done\n";
}

int main()
{
    // IPv4测试
    TestConversion("127.0.0.1", 8080);
    TestConversion("google.com", 80);

    // IPv6测试
    TestConversion("::1", 12345);
    TestConversion("ipv6.google.com", 80);

    RunDualStackServer();

    return 0;
}
// NOLINTEND

#else
int main()
{
    std::cout << "main done\n";
    return 0;
}
#endif