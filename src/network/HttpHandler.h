
#ifndef HTTP_HANDLER
#define HTTP_HANDLER 
#include <sys/types.h>
#include <sys/uio.h>     
#include <arpa/inet.h>   
#include <stdlib.h>      
#include <errno.h>      
#include "../db/DBConnectionPool.h"
#include "../db/DBConnectionRAII.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Buffer.h"


class HttpHandler {
public:
    // 构造函数
    HttpHandler();

    // 析构函数
    ~HttpHandler();

    // 初始化 HTTP 处理器，传入套接字和地址信息
    void init(int sockFd, const sockaddr_in& addr);

    // 读取数据并保存错误码
    ssize_t read(int* saveErrno);

    // 写入数据并保存错误码
    ssize_t write(int* saveErrno);

    // 关闭 HTTP 处理器
    void closeHttp();

    // 获取文件描述符
    int getFd() const;

    // 获取端口号
    int getPort() const;

    // 获取 IP 地址
    const char* getIP() const;

    // 获取地址信息
    sockaddr_in getAddr() const;

    // 处理 HTTP 请求
    bool process();

    // 获取待写入的字节数
    int toWriteBytes();

    // 检查连接是否存活
    bool isAlive() const;

    // 是否采用边缘触发模式
    static bool isET;

    // 静态常量，指定资源目录
    static const char* srcDir;

    // 静态原子整数，表示用户计数
    static std::atomic<int> userCount;

private:
    int fd;                   // 文件描述符
    struct sockaddr_in addr;  // 地址信息
    bool isClose;             // 连接是否关闭

    int iovCnt;               // iovec 数量
    struct iovec iov[2];     // iovec 数组

    Buffer readBuffer;        // 读缓冲区
    Buffer writeBuffer;       // 写缓冲区

    HttpRequest httpRequest;  // HTTP 请求对象
    HttpResponse httpResponse; // HTTP 响应对象
};

#endif 