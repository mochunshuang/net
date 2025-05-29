
#include <windows.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <type_traits>
#include <atomic>
#include <string_view>
#include <vector>

// 类型标识
enum class io_type : std::int8_t
{
    IO_CONNECT,
    IO_READ,
    IO_WRITE,
    IO_DISCONNECT
};

// 基础异步操作结构（标准布局）
#pragma pack(push, 1)
struct BaseOverlapped
{
    OVERLAPPED overlapped; // 必须为首成员
    io_type type;

  protected:
    BaseOverlapped(io_type t) noexcept : type(t)
    {
        ZeroMemory(&overlapped, sizeof(OVERLAPPED));
    }
};
#pragma pack(pop)

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

// 连接状态机
class Connection final
{
  public:
    enum class State
    {
        Connecting,
        Connected,
        Disconnecting,
        Closed
    };

    // 连接操作结构
    struct ConnectOp : BaseOverlapped
    {
        Connection &parent;
        SOCKET socket = INVALID_SOCKET;
        sockaddr_in target_addr{};

        ConnectOp(Connection &conn) : BaseOverlapped(io_type::IO_CONNECT), parent(conn) {}
    };

    // 读操作结构
    struct ReadOp : BaseOverlapped
    {
        Connection &parent;
        std::unique_ptr<char[]> buffer;
        DWORD bytes_transferred = 0;

        ReadOp(Connection &conn, size_t buf_size)
            : BaseOverlapped(io_type::IO_READ), parent(conn),
              buffer(std::make_unique<char[]>(buf_size))
        {
        }
    };

    // 写操作结构
    struct WriteOp : BaseOverlapped
    {
        Connection &parent;
        std::string_view data;
        DWORD bytes_transferred = 0;

        WriteOp(Connection &conn, std::string_view d)
            : BaseOverlapped(io_type::IO_WRITE), parent(conn), data(d)
        {
        }
    };

    // 断开操作结构
    struct DisconnectOp : BaseOverlapped
    {
        Connection &parent;

        DisconnectOp(Connection &conn)
            : BaseOverlapped(io_type::IO_DISCONNECT), parent(conn)
        {
        }
    };

    explicit Connection(SOCKET s = INVALID_SOCKET) : socket_(s), state_(State::Closed) {}

    // 启动异步连接
    bool StartConnect(const sockaddr_in &target)
    {
        if (state_.exchange(State::Connecting) != State::Closed)
            return false;

        auto op = std::make_unique<ConnectOp>(*this);
        op->socket = socket_;
        op->target_addr = target;

        // 模拟 ConnectEx 调用
        if (PostAsyncOperation(op->overlapped))
        {
            active_ops_.push_back(std::move(op));
            return true;
        }
        return false;
    }

    // 处理完成事件
    void HandleCompletion(BaseOverlapped *base)
    {
        switch (base->type)
        {
        case io_type::IO_CONNECT:
            HandleConnectComplete(static_cast<ConnectOp *>(base));
            break;
        case io_type::IO_READ:
            HandleReadComplete(static_cast<ReadOp *>(base));
            break;
        case io_type::IO_WRITE:
            HandleWriteComplete(static_cast<WriteOp *>(base));
            break;
        case io_type::IO_DISCONNECT:
            HandleDisconnectComplete(static_cast<DisconnectOp *>(base));
            break;
        default:
            Terminate();
        }
    }

    std::vector<std::unique_ptr<BaseOverlapped>> &activeOps()
    {
        return active_ops_;
    }

  private:
    // 状态处理核心逻辑
    void HandleConnectComplete(ConnectOp *op)
    {
        if (state_ != State::Connecting)
        {
            Terminate();
            return;
        }

        // 获取连接结果（模拟成功）
        state_ = State::Connected;
        StartReading(); // 连接成功后自动开始读操作
    }

    void HandleReadComplete(ReadOp *op)
    {
        if (state_ != State::Connected)
            return;

        // 处理接收数据（示例）
        std::cout << "Received " << op->bytes_transferred << " bytes\n";

        // 继续发起下一次读操作
        StartReading();
    }

    void HandleWriteComplete(WriteOp *op)
    {
        if (state_ != State::Connected)
            return;

        // 处理写入完成
        std::cout << "Sent " << op->bytes_transferred << "/" << op->data.size()
                  << " bytes\n";
    }

    void HandleDisconnectComplete(DisconnectOp *op)
    {
        state_ = State::Closed;
        CleanupResources();
    }

    // 操作发起方法
    void StartReading()
    {
        auto op = std::make_unique<ReadOp>(*this, 4096);
        if (PostAsyncOperation(op->overlapped))
        {
            active_ops_.push_back(std::move(op));
        }
    }

    void SendData(std::string_view data)
    {
        auto op = std::make_unique<WriteOp>(*this, data);
        if (PostAsyncOperation(op->overlapped))
        {
            active_ops_.push_back(std::move(op));
        }
    }

    void Disconnect()
    {
        state_ = State::Disconnecting;
        auto op = std::make_unique<DisconnectOp>(*this);
        if (PostAsyncOperation(op->overlapped))
        {
            active_ops_.push_back(std::move(op));
        }
    }

    // 资源清理
    void CleanupResources()
    {
        if (socket_ != INVALID_SOCKET)
        {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
        active_ops_.clear();
    }

    void Terminate()
    {
        state_ = State::Closed;
        CleanupResources();
    }

    // 模拟异步操作投递
    bool PostAsyncOperation(OVERLAPPED &ov)
    {
        // 实际应调用 PostQueuedCompletionStatus
        return true;
    }

    SOCKET socket_;
    std::atomic<State> state_;
    std::vector<std::unique_ptr<BaseOverlapped>> active_ops_;
};

// 使用示例
int main()
{
    // 验证内存布局
    static_assert(not std::is_standard_layout_v<Connection::ConnectOp>,
                  "ConnectOp must be standard layout");
    static_assert(offsetof(Connection::ConnectOp, overlapped) == 0,
                  "OVERLAPPED must be first member");

    // // 创建测试连接
    // SOCKET test_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Connection conn(test_socket);

    // // 模拟连接操作
    // sockaddr_in target{};
    // target.sin_family = AF_INET;
    // target.sin_addr.s_addr = inet_addr("127.0.0.1");
    // target.sin_port = htons(8080);

    // if (conn.StartConnect(target))
    // {
    //     std::cout << "Connection initiated\n";
    // }

    // // 模拟完成端口处理
    // auto &active_op = conn.activeOps();
    // conn.HandleCompletion(active_op[0].get());

    // 这个滑动窗口，目前认为有 3 个组成部分：
    // |已发送|已发送未完成|未发送|
    // 当以发送区间右边界索引 == 未发生区间的右边界索引，肯定是 这一组
    // 并非发送的数据完成了
    // 必须包装 已发送区间的包索引，从小到大，按序号排列，也就是保证有阻塞发送的功能
    // 已发送的右区间索引 每次 + k 右移动。 这个 k 大小 是由 右边界索引right确定的。
    // 当已发送未完成 确定：[right,k] 全部是已发送状态时，k就出来了。难点在k的计算
    // 目前k直觉是这么算的： k完成之后，遍历 [right,k] ，如果全都满足，则还得遍历
    // [k+1,max]  => k2 . 保证 [right,k2]全部是已发送状态时，此时k==k2的方式修改
    // 已发送区间的右边界
    // 同时我也感觉，不需要确定k。或许也是好的办法。反正目的是包的顺序性完成，必须按顺序通知上层
    // 但是底层是 并发的发送包，保证性能，同时保证顺序发送通知给上层。
    // 不需要计算 k 时：这个滑动窗口，目前认为有 2 个组成部分：
    // |已发送|已发送未完成+未发送|
    // 区别在于当且仅当 当前的 已发送完成的包序号是 【已发送区间右索引k+1
    // 时】才开始通知上层
    // 一致发送，知道 index 指向的包是未完成发送的状态。每次 已发送完成的包序号
    // k+1。都除非循环
    // 我觉得第二种更好你觉得呢
    // 发送： 1 2 3 4 5 6 7 8 9
    // 响应： 3 2 1 6 5 7 4 8 9
    // 那么应该调用 4次回调循环： 分别是 1 ， 4 ， 8 ，9 

    return 0;
}