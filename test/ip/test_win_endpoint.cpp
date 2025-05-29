#include <iostream>

// NOLINTBEGIN

#if defined(_MSC_VER)

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

class Endpoint
{
  public:
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

    // 核心验证方法：返回可验证的IP字符串和端口
    std::string ResolveIP() const
    {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        addrinfo *result;
        int status =
            getaddrinfo(host_.c_str(), std::to_string(port_).c_str(), &hints, &result);

        if (status != 0)
        {
            return "Resolution failed";
        }

        // 获取第一个IPv4或IPv6地址
        char ipstr[INET6_ADDRSTRLEN];
        const void *addr;
        if (result->ai_family == AF_INET)
        { // IPv4
            auto *ipv4 = reinterpret_cast<sockaddr_in *>(result->ai_addr);
            addr = &(ipv4->sin_addr);

            std::cout << "[ ipv4 ]\t";
        }
        else
        { // IPv6
            auto *ipv6 = reinterpret_cast<sockaddr_in6 *>(result->ai_addr);
            addr = &(ipv6->sin6_addr);

            std::cout << "[ ipv6 ]\t";
        }

        inet_ntop(result->ai_family, addr, ipstr, sizeof(ipstr));
        freeaddrinfo(result);
        return ipstr;
    }

    uint16_t GetPortNetworkOrder() const
    {
        return htons(port_);
    }

    sockaddr_in ResolveTCP() const
    {
        // NOTE: 地址 配置
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        // NOTE: 系统API
        addrinfo *result;
        int status =
            getaddrinfo(host_.c_str(), std::to_string(port_).c_str(), &hints, &result);

        if (status != 0 || !result)
        {
            throw std::runtime_error("Address resolution failed");
        }

        sockaddr_in resolved_addr = *reinterpret_cast<sockaddr_in *>(result->ai_addr);
        freeaddrinfo(result);

        std::cout << "Binary IP: ";
        for (int i = 0; i < 4; ++i)
        {
            std::cout << std::hex << (int)((unsigned char *)&resolved_addr.sin_addr)[i]
                      << " ";
        }
        std::cout << "\n";
        return resolved_addr;
    }

  private:
    std::string host_;
    uint16_t port_ = 0;
};

// 验证测试函数
void TestEndpoint(const std::string &host, uint16_t port)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    Endpoint ep;
    ep.WithHost(host).WithPort(port);

    std::cout << "Testing: " << host << ":" << port << "\n";
    std::cout << "Resolved IP: " << ep.ResolveIP() << "\n";
    std::cout << "Port in network byte order: 0x" << std::hex << ep.GetPortNetworkOrder()
              << "\n\n";

    WSACleanup();
}

// 在TestEndpoint中添加接口绑定验证
void TestInterfaceBinding(const std::string &interfaceIp)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0); // 随机端口
    inet_pton(AF_INET, interfaceIp.c_str(), &addr.sin_addr);

    if (bind(sock, (sockaddr *)&addr, sizeof(addr)) == 0)
    {
        std::cout << "Successfully bound to: " << interfaceIp << "\n";
    }
    else
    {
        std::cout << "Bind failed on: " << interfaceIp << " Error: " << WSAGetLastError()
                  << "\n";
    }
    closesocket(sock);

    WSACleanup();
}

// 服务器端实现
void RunServer()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    try
    {
        SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        Endpoint ep;
        sockaddr_in addr = ep.WithHost("127.0.0.1").WithPort(12345).ResolveTCP();

        if (bind(server, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            throw std::runtime_error("Bind failed: " + std::to_string(WSAGetLastError()));
        }

        listen(server, 1);
        std::cout << "Server listening on 127.0.0.1:12345\n";

        SOCKET client = accept(server, nullptr, nullptr);
        char buffer[1024]{};

        int received = recv(client, buffer, sizeof(buffer), 0);
        std::cout << "Server received: " << buffer << "\n";

        send(client, "pong", 5, 0);
        std::cout << "Server sent: pong\n";

        closesocket(client);
        closesocket(server);
        WSACleanup();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << "\n";
        WSACleanup();
    }
}

// 客户端实现
void RunClient()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    try
    {
        SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        Endpoint ep;
        sockaddr_in addr = ep.WithHost("127.0.0.1").WithPort(12345).ResolveTCP();

        if (connect(client, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            throw std::runtime_error("Connect failed: " +
                                     std::to_string(WSAGetLastError()));
        }

        send(client, "ping", 5, 0);
        std::cout << "Client sent: ping\n";

        char buffer[1024]{};
        int received = recv(client, buffer, sizeof(buffer), 0);
        std::cout << "Client received: " << buffer << "\n";

        closesocket(client);
        WSACleanup();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Client error: " << e.what() << "\n";
        WSACleanup();
    }
}

int main()
{
    // 验证测试用例
    TestEndpoint("example.com", 443);
    TestEndpoint("8.8.8.8", 53);
    TestEndpoint("localhost", 8080); // 8080 对应16进制：1F90
    {
        TestInterfaceBinding("localhost"); // 本地IP or host 都行
        TestInterfaceBinding("192.168.69.1");
    }
    {
        /**
        如果 1F90 是你直接看到的（比如调试器、内存查看工具）：
            1F 在前，90 在后 → 大端存储（人类直观顺序）。
            90 在前，1F 在后 → 小端存储（x86/ARM 等常见 CPU 默认是小端）。

        如果是程序计算出来的（比如 printf("%X", value)）：
            打印的 1F90
        是逻辑值，不涉及字节序问题（字节序只影响内存存储，不影响数值计算）。
         */
        //      0001 1111 1001 0000
        // 0x   1F        90
        // NOTE: 肉眼是大端 ， 网络流一般是 小端。 x86 等CPU 也是小端
        uint16_t num = 0x1F90; // 8080 in hex
        uint8_t *p = reinterpret_cast<uint8_t *>(&num);

        if (p[0] == 0x1F && p[1] == 0x90)
        {
            std::cout << "Big-Endian" << std::endl;
        }
        else if (p[0] == 0x90 && p[1] == 0x1F)
        {
            std::cout << "Little-Endian" << std::endl;
        }
        else
        {
            std::cout << "Unknown" << std::endl;
        }
    }

    {
        // ping / pong
        std::cout << "\nping-pong" << std::endl;
        // 启动服务器线程
        std::thread serverThread(RunServer);
        Sleep(100); // 确保服务器先启动

        // 运行客户端
        RunClient();

        serverThread.join();
    }

    std::cout << "\nmain done\n";
    return 0;
}
/*
C:\Users\mcs>ping example.com

正在 Ping example.com [96.7.128.175] 具有 32 字节的数据:
来自 96.7.128.175 的回复: 字节=32 时间=259ms TTL=46
来自 96.7.128.175 的回复: 字节=32 时间=217ms TTL=46
来自 96.7.128.175 的回复: 字节=32 时间=257ms TTL=46

输出：
Testing: example.com:443
Resolved IP: [ ipv4 ]   96.7.128.175
Port in network byte order: 0xbb01

Testing: 8.8.8.8:35
Resolved IP: [ ipv4 ]   8.8.8.8
Port in network byte order: 0x3500

Testing: localhost:1f90
Resolved IP: [ ipv6 ]   ::1
Port in network byte order: 0x901f #8080的十六进制是0x1F90（网络序是0x901f）

main done

*/

// NOLINTEND

#else

int main()
{
    std::cout << "main done\n";
    return 0;
}

#endif