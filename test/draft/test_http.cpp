#if defined(_MSC_VER)

// NOLINTBEGIN

#include <winsock2.h>
#include <MSWSock.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <string>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

// 定义AcceptEx和GetAcceptExSockaddrs函数类型
typedef BOOL(WINAPI *LPFN_ACCEPTEX)(SOCKET sListenSocket, SOCKET sAcceptSocket,
                                    PVOID lpOutputBuffer, DWORD dwReceiveDataLength,
                                    DWORD dwLocalAddressLength,
                                    DWORD dwRemoteAddressLength,
                                    LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped);

typedef void(WINAPI *LPFN_GETACCEPTEXSOCKADDRS)(
    PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength,
    DWORD dwRemoteAddressLength, LPSOCKADDR *LocalSockaddr, LPINT LocalSockaddrLength,
    LPSOCKADDR *RemoteSockaddr, LPINT RemoteSockaddrLength);

// 上下文结构
enum class OperationType
{
    Accept,
    Read,
    Write
};

struct PerIoData
{
    WSAOVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[1024];
    OperationType operation;
    SOCKET clientSocket;
};

// HTTP响应
const char *httpResponse = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 12\r\n"
                           "Connection: close\r\n\r\n"
                           "Hello World!";

// 初始化Winsock
bool InitWinsock()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

// 创建监听socket
SOCKET CreateListenSocket(short port)
{
    SOCKET listenSocket =
        WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "WSASocket failed: " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }

    // 允许地址重用
    int reuse = 1;
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse,
                   sizeof(reuse)) == SOCKET_ERROR)
    {
        std::cerr << "setsockopt(SO_REUSEADDR) failed: " << WSAGetLastError()
                  << std::endl;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "bind failed on port " << port << ": " << WSAGetLastError()
                  << std::endl;
        closesocket(listenSocket);
        return INVALID_SOCKET;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "listen failed on port " << port << ": " << WSAGetLastError()
                  << std::endl;
        closesocket(listenSocket);
        return INVALID_SOCKET;
    }

    std::cout << "Listening on port " << port << std::endl;
    return listenSocket;
}

// 工作线程函数
void WorkerThread(HANDLE hIOCP)
{
    while (true)
    {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED overlapped = nullptr;

        // 从IOCP获取完成事件
        BOOL status = GetQueuedCompletionStatus(hIOCP, &bytesTransferred, &completionKey,
                                                &overlapped, INFINITE);

        // 检查退出信号
        if (completionKey == 0 && overlapped == nullptr)
        {
            break;
        }

        if (!status)
        {
            DWORD error = GetLastError();
            if (overlapped == nullptr)
            {
                std::cerr << "GetQueuedCompletionStatus failed: " << error << std::endl;
                continue;
            }
        }

        // 获取操作上下文
        PerIoData *perIoData = reinterpret_cast<PerIoData *>(overlapped);

        if (perIoData->operation == OperationType::Accept)
        {
            SOCKET listenSocket = static_cast<SOCKET>(completionKey);
            SOCKET clientSocket = perIoData->clientSocket;

            // 设置客户端socket选项
            if (setsockopt(clientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                           (char *)&listenSocket, sizeof(listenSocket)) == SOCKET_ERROR)
            {
                std::cerr << "setsockopt(SO_UPDATE_ACCEPT_CONTEXT) failed: "
                          << WSAGetLastError() << std::endl;
            }

            // 准备读取客户端请求
            PerIoData *readData = new PerIoData{};
            readData->operation = OperationType::Read;
            readData->clientSocket = clientSocket;
            readData->wsaBuf.buf = readData->buffer;
            readData->wsaBuf.len = sizeof(readData->buffer);

            DWORD flags = 0;
            if (WSARecv(clientSocket, &readData->wsaBuf, 1, nullptr, &flags,
                        &readData->overlapped, nullptr) == SOCKET_ERROR)
            {
                int error = WSAGetLastError();
                if (error != WSA_IO_PENDING)
                {
                    std::cerr << "WSARecv failed: " << error << std::endl;
                    closesocket(clientSocket);
                    delete readData;
                }
            }

            // 重新发起AcceptEx请求
            perIoData->clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL,
                                                0, WSA_FLAG_OVERLAPPED);
            if (perIoData->clientSocket == INVALID_SOCKET)
            {
                std::cerr << "WSASocket for new accept failed: " << WSAGetLastError()
                          << std::endl;
                delete perIoData;
            }
            else
            {
                DWORD bytes = 0;
                if (!AcceptEx(listenSocket, perIoData->clientSocket, perIoData->buffer, 0,
                              sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &bytes,
                              &perIoData->overlapped))
                {
                    int error = WSAGetLastError();
                    if (error != WSA_IO_PENDING)
                    {
                        std::cerr << "AcceptEx failed: " << error << std::endl;
                        closesocket(perIoData->clientSocket);
                        delete perIoData;
                    }
                }
            }
        }
        else if (perIoData->operation == OperationType::Read)
        {
            if (bytesTransferred == 0)
            { // 连接关闭
                closesocket(perIoData->clientSocket);
                delete perIoData;
                continue;
            }

            // 准备HTTP响应
            PerIoData *writeData = new PerIoData{};
            writeData->operation = OperationType::Write;
            writeData->clientSocket = perIoData->clientSocket;
            writeData->wsaBuf.buf = const_cast<char *>(httpResponse); // 安全转换
            writeData->wsaBuf.len = static_cast<ULONG>(strlen(httpResponse));

            // 发送响应
            if (WSASend(perIoData->clientSocket, &writeData->wsaBuf, 1, nullptr, 0,
                        &writeData->overlapped, nullptr) == SOCKET_ERROR)
            {
                int error = WSAGetLastError();
                if (error != WSA_IO_PENDING)
                {
                    std::cerr << "WSASend failed: " << error << std::endl;
                    closesocket(perIoData->clientSocket);
                    delete writeData;
                }
            }

            delete perIoData;
        }
        else if (perIoData->operation == OperationType::Write)
        {
            // 写入完成后关闭连接
            closesocket(perIoData->clientSocket);
            delete perIoData;
        }
    }
}

int main()
{
    if (!InitWinsock())
        return 1;

    // 创建IOCP
    HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (hIOCP == NULL)
    {
        std::cerr << "CreateIoCompletionPort failed: " << GetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 创建监听socket
    SOCKET listenSocket8080 = CreateListenSocket(8080);
    SOCKET listenSocket8081 = CreateListenSocket(8081);

    if (listenSocket8080 == INVALID_SOCKET || listenSocket8081 == INVALID_SOCKET)
    {
        WSACleanup();
        return 1;
    }

    // 使用临时socket加载扩展函数
    SOCKET tempSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (tempSocket == INVALID_SOCKET)
    {
        std::cerr << "Temporary socket creation failed: " << WSAGetLastError()
                  << std::endl;
        closesocket(listenSocket8080);
        closesocket(listenSocket8081);
        WSACleanup();
        return 1;
    }

    // 获取AcceptEx函数指针
    LPFN_ACCEPTEX fnAcceptEx = nullptr;
    GUID guidAcceptEx = WSAID_ACCEPTEX;
    DWORD bytes = 0;
    if (WSAIoctl(tempSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx,
                 sizeof(guidAcceptEx), &fnAcceptEx, sizeof(fnAcceptEx), &bytes, NULL,
                 NULL) == SOCKET_ERROR)
    {
        std::cerr << "WSAIoctl for AcceptEx failed: " << WSAGetLastError() << std::endl;
        closesocket(tempSocket);
        closesocket(listenSocket8080);
        closesocket(listenSocket8081);
        WSACleanup();
        return 1;
    }

    // 获取GetAcceptExSockaddrs函数指针
    LPFN_GETACCEPTEXSOCKADDRS fnGetAcceptExSockaddrs = nullptr;
    GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
    if (WSAIoctl(tempSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                 &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),
                 &fnGetAcceptExSockaddrs, sizeof(fnGetAcceptExSockaddrs), &bytes, NULL,
                 NULL) == SOCKET_ERROR)
    {
        std::cerr << "WSAIoctl for GetAcceptExSockaddrs failed: " << WSAGetLastError()
                  << std::endl;
        closesocket(tempSocket);
        closesocket(listenSocket8080);
        closesocket(listenSocket8081);
        WSACleanup();
        return 1;
    }

    closesocket(tempSocket);

    // 将监听socket绑定到IOCP
    CreateIoCompletionPort((HANDLE)listenSocket8080, hIOCP, (ULONG_PTR)listenSocket8080,
                           0);
    CreateIoCompletionPort((HANDLE)listenSocket8081, hIOCP, (ULONG_PTR)listenSocket8081,
                           0);

    // 为每个监听socket创建AcceptEx操作
    auto SetupAcceptEx = [&](SOCKET listenSocket) {
        PerIoData *acceptData = new PerIoData{};
        acceptData->operation = OperationType::Accept;
        acceptData->clientSocket =
            WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

        if (acceptData->clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Client socket creation failed: " << WSAGetLastError()
                      << std::endl;
            delete acceptData;
            return false;
        }

        DWORD bytes = 0;
        if (!fnAcceptEx(listenSocket, acceptData->clientSocket, acceptData->buffer, 0,
                        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &bytes,
                        &acceptData->overlapped))
        {
            int error = WSAGetLastError();
            if (error != WSA_IO_PENDING)
            {
                std::cerr << "AcceptEx failed: " << error << std::endl;
                closesocket(acceptData->clientSocket);
                delete acceptData;
                return false;
            }
        }

        // 将客户端socket也绑定到同一个IOCP
        CreateIoCompletionPort((HANDLE)acceptData->clientSocket, hIOCP,
                               (ULONG_PTR)listenSocket, 0);
        return true;
    };

    SetupAcceptEx(listenSocket8080);
    SetupAcceptEx(listenSocket8081);

    // 创建工作线程 (数量 = CPU核心数 * 2)
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int threadCount = sysInfo.dwNumberOfProcessors * 2;
    if (threadCount < 2)
        threadCount = 2; // 至少2个线程
    std::vector<std::thread> threads;

    std::cout << "Starting HTTP server on ports 8080 and 8081..." << std::endl;
    std::cout << "Using " << threadCount << " worker threads" << std::endl;

    for (int i = 0; i < threadCount; ++i)
    {
        threads.emplace_back(WorkerThread, hIOCP);
    }

    // 等待退出
    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();

    // 通知工作线程退出
    for (int i = 0; i < threadCount; ++i)
    {
        PostQueuedCompletionStatus(hIOCP, 0, 0, nullptr);
    }

    // 等待线程结束
    for (auto &t : threads)
    {
        t.join();
    }

    // 清理资源
    closesocket(listenSocket8080);
    closesocket(listenSocket8081);
    CloseHandle(hIOCP);
    WSACleanup();

    return 0;
}

// NOLINTEND

#else
#include <iostream>

int main()
{
    std::cout << "main done\n";
    return 0;
}
#endif