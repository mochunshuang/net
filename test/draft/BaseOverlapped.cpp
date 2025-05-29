#include <windows.h>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <memory>

// NOLINTBEGIN

enum class io_type
{
    IO_UNKNOWN,
    IO_ACCEPT,
    IO_READ,
    IO_WRITE,
    IO_DISCONNECT,
};

// NOTE: 继承回破坏 standard-layout。即使 OVERLAPPED 是标准布局类型。组合可以解决
//  基类必须为标准布局类型 (standard-layout)
struct BaseOverlapped
{
    OVERLAPPED overlapped;
    io_type type;
    BaseOverlapped(io_type t) noexcept : overlapped{}, type{t} {}
};

// 显式验证内存布局
static_assert(offsetof(BaseOverlapped, overlapped) == 0, "Overlapped misaligned");
static_assert(offsetof(BaseOverlapped, type) == sizeof(OVERLAPPED),
              "Type field offset error");
// 类型安全验证测试
static_assert(std::is_standard_layout_v<OVERLAPPED>, "Must be standard layout");
static_assert(std::is_standard_layout_v<BaseOverlapped>, "Must be standard layout");
static_assert(offsetof(BaseOverlapped, type) == sizeof(OVERLAPPED),
              "Type field must follow OVERLAPPED layout");

struct IocpReadOp : BaseOverlapped
{
    static constexpr std::size_t BUFF_SIZE = 8;
    char buffer[BUFF_SIZE];
    // ::WSABUF wsabuf;
};
static_assert(not std::is_standard_layout_v<IocpReadOp>, "Must be standard layout");

struct IocpReadOp2
{
    BaseOverlapped base;
    static constexpr std::size_t BUFF_SIZE = 8;
    char buffer[BUFF_SIZE];
    // ::WSABUF wsabuf;
};
static_assert(std::is_standard_layout_v<IocpReadOp2>, "Must be standard layout");

struct AcceptOverlapped : BaseOverlapped
{
    const SOCKET listen_socket;
    SOCKET client_socket;
    sockaddr_in client_addr{};
    int addr_len = sizeof(sockaddr_in);

    AcceptOverlapped(SOCKET listen)
        : BaseOverlapped(io_type::IO_ACCEPT), listen_socket(listen)
    {
    }
};

struct ReadOverlapped : BaseOverlapped
{
    std::unique_ptr<char[]> buffer;
    DWORD bytes_transferred = 0;

    ReadOverlapped(size_t buf_size) noexcept
        : BaseOverlapped(io_type::IO_READ), buffer(std::make_unique<char[]>(buf_size))
    {
    }
};

// 模拟 IOCP 完成处理
void HandleCompletion(BaseOverlapped *base)
{
    switch (base->type)
    {
    case io_type::IO_ACCEPT: {
        auto *accept = static_cast<AcceptOverlapped *>(base);
        std::cout << "Accept completed on socket: " << accept->client_socket << "\n";
        break;
    }
    case io_type::IO_READ: {
        auto *read = static_cast<ReadOverlapped *>(base);
        std::cout << "Read completed, bytes: " << read->bytes_transferred << "\n";
        break;
    }
    // 其他类型处理...
    default:
        std::terminate(); // 不应该出现未知类型
    }
}

// 模拟 IOCP 投递操作测试
void test_iocp_flow()
{
    // 模拟 accept 投递
    AcceptOverlapped accept_ov{INVALID_SOCKET};
    accept_ov.client_socket = 1234; // 模拟 accept 成功

    // 模拟完成通知
    HandleCompletion(&accept_ov);

    // 模拟 read 投递
    ReadOverlapped read_ov(1024);
    read_ov.bytes_transferred = 512; // 模拟读取完成

    // 模拟完成通知
    HandleCompletion(&read_ov);

    // 模拟 read 投递
    ReadOverlapped *read_ov2 = new ReadOverlapped(1024);
    read_ov2->bytes_transferred = 512; // 模拟读取完成

    // 模拟完成通知
    HandleCompletion(read_ov2);

    delete read_ov2;
}

int main()
{
    test_iocp_flow();
    return 0;
}
// NOLINTEND