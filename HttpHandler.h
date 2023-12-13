
#ifndef HTTP_HANDLER
#define HTTP_HANDLER 
#include <sys/types.h>
#include <sys/uio.h>     
#include <arpa/inet.h>   
#include <stdlib.h>      
#include <errno.h>      
#include "DBConnectionPool.h"
#include "DBConnectionRAII.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Buffer.h"


class HttpHandler{
public:
    HttpHandler();

    ~HttpHandler();

    void init(int sockFd, const sockaddr_in&addr);

    ssize_t read(int* saveErrno);

    ssize_t write(int* saveErrno);

    void closeHttp();

    int getFd() const;

    int getPort()const;

    const char* getIP() const;

    sockaddr_in getAddr() const;

    bool process();

    int toWriteBytes();

    bool isAlive() const;

    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;
private:
    int fd;
    struct sockaddr_in addr;
    bool isClose;

    int iovCnt;
    struct iovec iov[2];
    
    Buffer readBuffer;
    Buffer writeBuffer;

    HttpRequest httpRequest;
    HttpResponse httpResponse;
};
#endif 