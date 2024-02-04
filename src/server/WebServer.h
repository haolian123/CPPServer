#ifndef WEBSERVER
#define WEBSERVER

#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../network/EpollManager.h"
#include "../utility/Log.h"
#include "../utility/TimerHandler.h"
#include "../db/DBConnectionPool.h"
#include "../utility/ThreadPool.h"
#include "../db/DBConnectionRAII.h"
#include "../network/HttpHandler.h"

class WebServer {
private:
    static const int MAX_FD = 65536;
    int port;               // 服务器端口
    bool openLinger;        // 是否开启 linger
    int timeoutMS;          // 超时时间（毫秒）
    bool isClose;           // 服务器是否关闭
    int listenFd;           // 监听套接字
    char* srcDir;           // 资源目录
    uint32_t listenEvent;   // 监听事件类型
    uint32_t connectionEvent; // 连接事件类型
    std::unique_ptr<TimerHandler> timer; // 定时器
    std::unique_ptr<ThreadPool> threadPool; // 线程池
    std::unique_ptr<EpollManager> epollManager; // epoll 管理器
    std::unordered_map<int, HttpHandler> users; // 连接用户映射

private:
    bool initSocket();                      // 初始化监听套接字
    void initEventMode(int trigMode);       // 初始化事件模式
    void addClient(int fd, sockaddr_in addr); // 添加客户端连接

    void handleListen();                    // 处理监听事件
    void handleWrite(HttpHandler* client);  // 处理写事件
    void handleRead(HttpHandler* client);   // 处理读事件

    void sendError(int fd, const char* info); // 发送错误响应
    void extentTime(HttpHandler* client);    // 延长定时任务时间
    void closeConnection(HttpHandler* client); // 关闭连接

    void onRead(HttpHandler* client);        // 读事件回调
    void onWrite(HttpHandler* client);       // 写事件回调
    void onProcess(HttpHandler* client);     // 处理事件回调

    static int setFdNonBlock(int fd);         // 设置套接字为非阻塞模式

public:
    WebServer(
        int port, int trigMode, int timeoutMS, bool optLinger,
        int sqlPort, const char* sqlUser, const char* sqlPassword,
        const char* dbName, int connPoolNum, int threadNum,
        bool openLog, int logLevel, int logQueSize
    );
    ~WebServer();
    void start(); // 启动服务器
};

#endif
