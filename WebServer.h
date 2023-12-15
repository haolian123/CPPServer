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

#include"EpollManager.h"
#include"Log.h"
#include"TimerHandler.h"
#include"DBConnectionPool.h"
#include"ThreadPool.h"
#include"DBConnectionRAII.h"
#include "HttpHandler.h"

class WebServer{
private:
    static const int MAX_FD = 65536;
    int port;
    bool openLinger;
    int timeoutMS;
    bool isClose;
    int listenFd;
    char* srcDir;
    uint32_t listenEvent;
    uint32_t connectionEvent;
    std::unique_ptr<TimerHandler> timer;
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<EpollManager> epollManager;
    std::unordered_map<int,HttpHandler> users;
private:
    bool initSocket();
    void initEventMode(int trigMode);
    void addClient(int fd, sockaddr_in addr);

    void handleListen();
    void handleWrite(HttpHandler* client);
    void handleRead(HttpHandler* client);

    void sendError(int fd, const char* info);
    void extentTime (HttpHandler* client);
    void closeConnection(HttpHandler* client);

    void onRead(HttpHandler* client);
    void onWrite(HttpHandler* client);
    void onProcess(HttpHandler* client);

    static int setFdNonBlock(int fd);
public:
    WebServer(
        int port , int trigMode, int timeoutMS , bool optLinger,
        int sqlPort, const char* sqlUser, const char* sqlPassword,
        const char* dbName, int connPoolNum , int threadNum,
        bool openLog, int logLevel, int logQueSize
    );
    ~WebServer();
    void start();
};
#endif 