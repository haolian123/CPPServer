#include"WebServer.h"

WebServer::WebServer(
        int port , int trigMode, int timeoutMS , bool optLinger,
        int sqlPort, const char* sqlUser, const char* sqlPassword,
        const char* dbName, int connPoolNum , int threadNum,
        bool openLog, int logLevel, int logQueSize):
        port(port),openLinger(optLinger),timeoutMS(timeoutMS),isClose(false),
        timer(new TimerHandler()), threadPool(new ThreadPool(threadNum)),epollManager(new EpollManager()){

        srcDir = getcwd(nullptr,256);
        assert(srcDir);
        std::strncat(srcDir,"/resources/",16);
        HttpHandler::userCount = 0;
        HttpHandler::srcDir = srcDir;
        DBConnectionPool::getInstance()->Init("localhost",sqlPort,sqlUser,sqlPassword,dbName,connPoolNum);
        initEventMode(trigMode);
        if(!initSocket()){isClose = true;}

        if(openLog){
            Log::getInstance()->init(logLevel,"./log",".log",logQueSize);
            if(isClose){
                LOG_ERROR("========== Server init error!==========");
            }else{
                LOG_INFO("========== Server init ==========");
                LOG_INFO("Port:%d, OpenLinger: %s", port, optLinger? "true":"false");
                LOG_INFO("Listen Mode: %s, OpenConn Mode: %s",
                                (listenEvent & EPOLLET ? "ET": "LT"),
                                (connectionEvent & EPOLLET ? "ET": "LT"));
                LOG_INFO("LogSys level: %d", logLevel);
                LOG_INFO("srcDir: %s", HttpHandler::srcDir);
                LOG_INFO("SqlConnPool num: %d, ThreadPool num: %d", connPoolNum, threadNum);
            }
        }
}

WebServer::~WebServer(){
    close(listenFd);
    isClose= true;
    std::free(srcDir);
    DBConnectionPool::getInstance()->closePool();
}

void WebServer::initEventMode(int trigMode){
    listenEvent = EPOLLRDHUP;
    connectionEvent = EPOLLONESHOT | EPOLLRDHUP;
    switch(trigMode){
        case 0:
            break;
        case 1:
            connectionEvent |= EPOLLET;
            break;
        case 2:
            listenEvent |= EPOLLET;
            break;
        case 3:
            listenEvent |= EPOLLET;
            connectionEvent |= EPOLLET;
            break;
        default:
            listenEvent |= EPOLLET;
            connectionEvent |=EPOLLET;
            break;
    }
    HttpHandler::isET = (connectionEvent& EPOLLET);
}

void WebServer::start(){
    int timeMS = -1;
    if(!isClose){
        LOG_INFO("========== Server start ==========");
    }
    while(!isClose){
        if(timeoutMS > 0){
            timeMS= timer->getNextTick();
        }
        int eventCnt = epollManager->wait(timeMS);
        for(int i=0;i<eventCnt;i++){
            int fd = epollManager->getEventFd(i);
            uint32_t events = epollManager->getEvents(i);
            if(fd == listenFd){
                handleListen();
            }else if(events&(EPOLLRDHUP|EPOLLHUP|EPOLLERR)){
                assert(users.count(fd)>0);
                closeConnection(&users[fd]);
            }else if(events&EPOLLIN){
                assert(users.count(fd)>0);
                handleRead(&users[fd]);
            }else if(events& EPOLLOUT){
                assert(users.count(fd)>0);
                handleWrite(&users[fd]);
            }else{
                LOG_ERROR("Unexpected event");
            }
        }
    }
}

void WebServer::sendError(int fd, const char* info){
    assert(fd>0);
    int ret = send(fd,info,strlen(info),0);
    if(ret<0){
        LOG_WARN("send error to client[%d] error!", fd);
    }
    close(fd);
}

void WebServer::closeConnection(HttpHandler* client){
    assert(client);
    LOG_INFO("Client[%d] quit!",client->getFd());
    epollManager->deleteFd(client->getFd());
    client->closeHttp();
}

void WebServer::addClient(int fd, sockaddr_in addr){
    assert(fd>0);
    users[fd].init(fd,addr);
    if(timeoutMS>0){
        timer->add(fd,timeoutMS,std::bind(&WebServer::closeConnection, this, &users[fd]));
    }
    epollManager->addFd(fd,EPOLLIN|connectionEvent);
    setFdNonBlock(fd);
    LOG_INFO("Client[%d] in!", users[fd].getFd());
}

void WebServer::handleListen(){
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    do{
        int fd = accept(listenFd,(struct sockaddr*)&addr,&len);
        if(fd<=0) {return ;}
        else if(HttpHandler::userCount>=MAX_FD){
            sendError(fd,"Server busy!");
            LOG_WARN("Clients is full!");
            return;
        }
        addClient(fd,addr);
    }while(listenEvent&EPOLLET);
}

void WebServer::handleRead(HttpHandler* client){
    assert(client);
    extentTime(client);
    threadPool->addTask(std::bind(&WebServer::onRead, this, client));
}

void WebServer::handleWrite(HttpHandler* client){
    assert(client);
    extentTime(client);
    threadPool->addTask(std::bind(&WebServer::onWrite, this, client));
}

void WebServer::extentTime(HttpHandler* client){
    assert(client);
    if(timeoutMS>0){
        timer->adjust(client->getFd(),timeoutMS);
    }
}

void WebServer::onRead(HttpHandler* client){
    assert(client);
    int ret = -1;
    int readErrno = 0;
    ret = client->read(&readErrno);
    if(ret<=0&& readErrno!=EAGAIN){
        closeConnection(client);
        return;
    }
    onProcess(client);
}

void WebServer::onWrite(HttpHandler* client){
    assert(client);
    int ret = -1;
    int writeErrno = 0;
    ret = client->write(&writeErrno);
    if(client->toWriteBytes()==0){
        //传输完成
        if(client->isAlive()){
            onProcess(client);
            return;
        }
        
    }else if(ret<0){
        if(writeErrno == EAGAIN){
            //继续传输
            epollManager->modifyFd(client->getFd(),connectionEvent|EPOLLOUT);
            return;
        }
    }
    closeConnection(client);
}
void WebServer::onProcess(HttpHandler* client){
    if(client->process()){
        epollManager->modifyFd(client->getFd(), connectionEvent | EPOLLOUT);
    }else{
        epollManager->modifyFd(client->getFd(), connectionEvent | EPOLLIN);
    }
}
bool WebServer::initSocket(){
    int ret;
    struct sockaddr_in addr;
    if(port>65535||port<1024){
        LOG_ERROR("Port:%d error!",  port);
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    struct linger optLinger = {0};
    if(openLinger){
        //指导所剩数据发送完毕或超时
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }
    listenFd = socket(AF_INET,SOCK_STREAM,0);
    if(listenFd<0){
        LOG_ERROR("Create socket error!", port);
        return false;
    }
    ret = setsockopt(listenFd,SOL_SOCKET,SO_LINGER,&optLinger,sizeof(optLinger));
    if(ret<0){
        close(listenFd);
        LOG_ERROR("Init linger error!", port);
        return false;
    }
    int optval = 1;
    ret = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if(ret == -1){
        LOG_ERROR("set socket setsockopt error !");
        close(listenFd);
        return false;
    }
    ret = bind(listenFd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0) {
        LOG_ERROR("Bind Port:%d error!", port);
        close(listenFd);
        return false;
    }
    ret = listen(listenFd,6);
    if(ret < 0) {
        LOG_ERROR("Listen port:%d error!", port);
        close(listenFd);
        return false;
    }
    ret = epollManager->addFd(listenFd,listenEvent|EPOLLIN);
    if(ret == 0){
        LOG_ERROR("Add listen error!");
        close(listenFd);
        return false;
    }
    setFdNonBlock(listenFd);
    LOG_INFO("Server port:%d", port);
    return true;
}

int WebServer::setFdNonBlock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}