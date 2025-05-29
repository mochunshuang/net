#include <WinSock2.h>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <iostream>

// NOLINTBEGIN

// 操作类型标签
enum class io_type : int8_t
{
    IO_ACCEPT, // 接受连接
    IO_READ,   // 读取数据
    IO_WRITE,  // 写入数据
    IO_UNKNOWN // 错误类型
};

// 基础重叠操作结构（必须标准布局）
struct BaseOverlapped
{
    OVERLAPPED overlapped; // Windows 重叠 I/O 结构
    io_type op_type;       // 操作类型标签
    SOCKET related_socket; // 关联的套接字

    explicit BaseOverlapped(io_type type, SOCKET sock = INVALID_SOCKET) noexcept
        : overlapped{}, op_type(type), related_socket(sock)
    {
    }
};
static_assert(std::is_standard_layout_v<BaseOverlapped>, "Must be standard layout");
static_assert(offsetof(BaseOverlapped, overlapped) == 0,
              "OVERLAPPED must be first member");

// 接受连接操作
struct AcceptOp
{
    BaseOverlapped base;                              // 必须为首成员
    SOCKET client_socket;                             // 接受的客户端套接字
    sockaddr_in client_addr;                          // 客户端地址
    char accept_buffer[2 * sizeof(sockaddr_in) + 32]; // WSAAccept 所需缓冲区

    explicit AcceptOp(SOCKET listen_sock) noexcept
        : base(io_type::IO_ACCEPT, listen_sock), client_socket(INVALID_SOCKET)
    {
    }
    OVERLAPPED *getOVERLAPPED() noexcept
    {
        return &base.overlapped;
    }
};
static_assert(offsetof(AcceptOp, base) == 0, "Base must be first");
static_assert(std::is_standard_layout_v<AcceptOp>, "Must be standard layout");
static_assert(offsetof(AcceptOp, base.overlapped) == 0,
              "OVERLAPPED must be first member");

// 读取操作
struct ReadOp
{
    BaseOverlapped base;
    char buffer[4096]; // 接收缓冲区
    DWORD bytes_transferred = 0;

    explicit ReadOp(SOCKET sock) noexcept : base(io_type::IO_READ, sock) {}
    OVERLAPPED *getOVERLAPPED() noexcept
    {
        return &base.overlapped;
    }
};
static_assert(offsetof(ReadOp, base) == 0, "Base must be first");
static_assert(std::is_standard_layout_v<ReadOp>, "Must be standard layout");
static_assert(offsetof(ReadOp, base.overlapped) == 0, "OVERLAPPED must be first member");

// 写入操作
struct WriteOp
{
    BaseOverlapped base;
    WSABUF wsa_buf; // 发送缓冲区描述
    DWORD bytes_sent = 0;

    explicit WriteOp(SOCKET sock, const char *data, size_t len) noexcept
        : base(io_type::IO_WRITE, sock),
          wsa_buf{static_cast<ULONG>(len), const_cast<char *>(data)}
    {
    }
    OVERLAPPED *getOVERLAPPED() noexcept
    {
        return &base.overlapped;
    }
};
static_assert(offsetof(WriteOp, base) == 0, "Base must be first");
static_assert(std::is_standard_layout_v<WriteOp>, "Must be standard layout");
static_assert(offsetof(WriteOp, base.overlapped) == 0, "OVERLAPPED must be first member");

// 从 OVERLAPPED* 安全转换为具体操作类型
template <typename OpType>
OpType *SafeCastFromOverlapped(OVERLAPPED *pov) noexcept
{
    static_assert(std::is_standard_layout_v<OpType>,
                  "Target type must be standard layout");
    static_assert(offsetof(OpType, base) == 0, "OpType.base must be at offset 0");

    auto *pBase = reinterpret_cast<BaseOverlapped *>(pov);
    return reinterpret_cast<OpType *>(pBase);
}

// 测试 AcceptOp 转换
void TestAcceptOpConversion()
{
    SOCKET listenSock = 0;
    AcceptOp op(listenSock);

    // 模拟 Windows 返回的 OVERLAPPED*
    OVERLAPPED *pov = op.getOVERLAPPED();

    // 转换并验证
    auto *pConverted = SafeCastFromOverlapped<AcceptOp>(pov);
    assert(pConverted == &op);
    assert(pConverted->base.op_type == io_type::IO_ACCEPT);
}

void TestReadOpConversion()
{
    ReadOp op(INVALID_SOCKET);
    OVERLAPPED *pov = op.getOVERLAPPED();
    auto *p = SafeCastFromOverlapped<ReadOp>(pov);
    assert(p != nullptr);
    assert(p == &op);
}

void TestWriteOpConversion()
{
    WriteOp op(INVALID_SOCKET, nullptr, 0);
    OVERLAPPED *pov = op.getOVERLAPPED();
    auto *p = SafeCastFromOverlapped<WriteOp>(pov);
    assert(p != nullptr);
    assert(p == &op);

    auto *p2 = reinterpret_cast<WriteOp *>(reinterpret_cast<BaseOverlapped *>(pov));
    assert(p2 == &op);

    auto *p3 = reinterpret_cast<WriteOp *>(pov);
    assert(p3 == &op);
    assert(p3->bytes_sent == 0);
}

int main()
{
    TestAcceptOpConversion();
    TestReadOpConversion();
    TestWriteOpConversion();
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND