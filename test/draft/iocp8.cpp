
#if defined(_MSC_VER)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

// 控制台颜色枚举
enum ConsoleColor
{
    DEFAULT = 7,
    REQUEST_LINE = 10, // 绿色
    HEADERS = 11,      // 蓝色
    BODY = 14          // 黄色
};

// 设置控制台颜色
void SetConsoleColor(ConsoleColor color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// HTTP解析状态
enum ParseState
{
    PARSE_START_LINE,
    PARSE_HEADERS,
    PARSE_BODY,
    PARSE_COMPLETE
};

// 连接上下文结构
struct ConnectionContext
{
    SOCKET socket;
    OVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[4096];
    ParseState state;
    size_t bodyLength;
    std::string requestData;

    ConnectionContext() : state(PARSE_START_LINE), bodyLength(0)
    {
        memset(&overlapped, 0, sizeof(OVERLAPPED));
        wsaBuf.buf = buffer;
        wsaBuf.len = sizeof(buffer);
    }
};

class HttpServer
{
  public:
    HttpServer() : iocpHandle_(INVALID_HANDLE_VALUE), listenSocket_(INVALID_SOCKET) {}

    bool Start(short port)
    {
        // 初始化WSA
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            return false;

        // 创建IOCP
        iocpHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (iocpHandle_ == NULL)
            return false;

        // 创建监听Socket
        listenSocket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (listenSocket_ == INVALID_SOCKET)
            return false;

        // 绑定端口
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(port);
        if (bind(listenSocket_, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
            SOCKET_ERROR)
            return false;

        // 开始监听
        if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR)
            return false;

        // 关联IOCP
        CreateIoCompletionPort((HANDLE)listenSocket_, iocpHandle_, (ULONG_PTR)this, 0);

        // 开始接受连接
        PostAccept();

        // 工作线程处理IOCP事件
        for (int i = 0; i < 4; ++i)
        {
            CreateThread(NULL, 0, WorkerThread, this, 0, NULL);
        }

        return true;
    }

  private:
    static DWORD WINAPI WorkerThread(LPVOID lpParam)
    {
        HttpServer *server = (HttpServer *)lpParam;
        DWORD bytesTransferred;
        ULONG_PTR completionKey;
        LPOVERLAPPED overlapped;

        while (true)
        {
            BOOL result =
                GetQueuedCompletionStatus(server->iocpHandle_, &bytesTransferred,
                                          &completionKey, &overlapped, INFINITE);

            if (!result)
            {
                // 处理错误
                continue;
            }

            if (completionKey == (ULONG_PTR)server)
            {
                // 新连接到达
                server->HandleNewConnection((ConnectionContext *)overlapped);
                server->PostAccept();
            }
            else
            {
                // 处理I/O操作
                ConnectionContext *context =
                    CONTAINING_RECORD(overlapped, ConnectionContext, overlapped);
                if (bytesTransferred == 0)
                {
                    closesocket(context->socket);
                    delete context;
                    continue;
                }

                context->requestData.append(context->buffer, bytesTransferred);
                server->ProcessRequest(context);
                server->PostRecv(context);
            }
        }
        return 0;
    }

    void PostAccept()
    {
        ConnectionContext *context = new ConnectionContext();
        AcceptEx(listenSocket_, context->socket, context->buffer, 0,
                 sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL,
                 &context->overlapped);
    }

    void HandleNewConnection(ConnectionContext *context)
    {
        SOCKET clientSocket = context->socket;
        // 关联新socket到IOCP
        CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle_, (ULONG_PTR)this, 0);
        PostRecv(context);
    }

    void PostRecv(ConnectionContext *context)
    {
        DWORD flags = 0;
        WSARecv(context->socket, &context->wsaBuf, 1, NULL, &flags, &context->overlapped,
                NULL);
    }

    void ProcessRequest(ConnectionContext *context)
    {
        while (true)
        {
            switch (context->state)
            {
            case PARSE_START_LINE: {
                size_t pos = context->requestData.find("\r\n");
                if (pos == std::string::npos)
                    return;

                std::string startLine = context->requestData.substr(0, pos);
                context->requestData.erase(0, pos + 2);

                // 日志输出
                SetConsoleColor(REQUEST_LINE);
                std::cout << "[Start Line] " << startLine << std::endl;
                SetConsoleColor(DEFAULT);

                context->state = PARSE_HEADERS;
                break;
            }

            case PARSE_HEADERS: {
                size_t pos = context->requestData.find("\r\n\r\n");
                if (pos == std::string::npos)
                    return;

                std::string headers = context->requestData.substr(0, pos);
                context->requestData.erase(0, pos + 4);

                // 日志输出
                SetConsoleColor(HEADERS);
                std::cout << "[Headers]\n" << headers << std::endl;
                SetConsoleColor(DEFAULT);

                // 处理Content-Length
                size_t clPos = headers.find("Content-Length: ");
                if (clPos != std::string::npos)
                {
                    std::istringstream iss(headers.substr(clPos + 16));
                    iss >> context->bodyLength;
                    context->state = PARSE_BODY;
                }
                else
                {
                    context->state = PARSE_COMPLETE;
                }
                break;
            }

            case PARSE_BODY: {
                if (context->requestData.length() >= context->bodyLength)
                {
                    std::string body =
                        context->requestData.substr(0, context->bodyLength);
                    context->requestData.erase(0, context->bodyLength);

                    // 日志输出
                    SetConsoleColor(BODY);
                    std::cout << "[Body]\n" << body << std::endl;
                    SetConsoleColor(DEFAULT);

                    context->state = PARSE_COMPLETE;
                }
                else
                {
                    return;
                }
                break;
            }

            case PARSE_COMPLETE: {
                SendResponse(context);
                context->state = PARSE_START_LINE;
                context->requestData.clear();
                break;
            }
            }
        }
    }

    void SendResponse(ConnectionContext *context)
    {
        const char *response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 2\r\n"
                               "\r\n"
                               "OK";

        WSABUF wsaBuf{static_cast<ULONG>(strlen(response)), const_cast<char *>(response)};
        WSASend(context->socket, &wsaBuf, 1, NULL, 0, &context->overlapped, NULL);
    }

    HANDLE iocpHandle_;
    SOCKET listenSocket_;
};

int main()
{
    HttpServer server;
    if (server.Start(8080))
    {
        std::cout << "Server started on port 8080" << std::endl;
        while (true)
            Sleep(1000);
    }
    return 0;
}
#else

#include <iostream>

int main()
{
    std::cout << "main done\n";
    return 0;
}
#endif