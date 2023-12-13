#include"HttpHandler.h"

const char* HttpHandler::srcDir;

std::atomic<int>HttpHandler::userCount;

bool HttpHandler::isET;

HttpHandler::HttpHandler(){
    fd=-1;
    addr={0};
    isClose=true;
}

HttpHandler::~HttpHandler(){
    closeHttp();
}

void HttpHandler::init(int fd, const sockaddr_in&addr){
    assert(fd>0);
    userCount++;
    this->addr=addr;
    this->fd=fd;
    writeBuffer.retrieveAll();
    readBuffer.retrieveAll();
    isClose=false;
}

void HttpHandler::closeHttp(){
    httpResponse.unMapFile();
    if(isClose==false){
        isClose=true;
        userCount--;
        close(fd);
    }
}


int HttpHandler::getFd() const{
    return fd;
}


struct sockaddr_in HttpHandler::getAddr() const{
    return addr;
}

const char* HttpHandler::getIP() const{
    return inet_ntoa(addr.sin_addr);
}

int HttpHandler::getPort() const{
    return addr.sin_port;
}

ssize_t HttpHandler::read(int* saveErrno){
    ssize_t len=-1;
    do{
        len=readBuffer.readFd(fd,saveErrno);
        if(len<=0){
            break;
        }
    }while(isET);
    return len;
}

ssize_t HttpHandler::write(int* saveErrno){
    ssize_t len=-1;
    do{
        len=writev(fd,iov,iovCnt);
        if(len<=0){
            *saveErrno = errno;
            break;
        }
        if(iov[0].iov_len+ iov[1].iov_len == 0){
            break;
        }else if(static_cast<size_t>(len)>iov[0].iov_len){
            iov[1].iov_base=(uint8_t*) iov[1].iov_base + (len - iov[0].iov_len);
            iov[1].iov_len-=(len-iov[0].iov_len);
            if(iov[0].iov_len){
                writeBuffer.retrieveAll();
                iov[0].iov_len = 0;
            }
        }else{
            iov[0].iov_base = (uint8_t*)iov[0].iov_base + len;
            iov[0].iov_len -= len;
            writeBuffer.retrieve(len);
        }
    }while(isET||toWriteBytes()>10240);
    return len;
}

bool HttpHandler::process(){
    httpRequest.init();
    if(readBuffer.readableBytes()<=0){
        return false;
    }else if(httpRequest.parse(readBuffer)){
        httpResponse.init(srcDir,httpRequest.getPath(),httpRequest.isAlive(),200);
    }else{
        httpResponse.init(srcDir,httpRequest.getPath(),false,400);
    }
    httpResponse.makeResponse(writeBuffer);

    iov[0].iov_base = const_cast<char*>(writeBuffer.peek());
    iov[0].iov_len = writeBuffer.readableBytes();
    iovCnt = 1;

    if(httpResponse.fileLen()>0 && httpResponse.file()){
        iov[1].iov_base = httpResponse.file();
        iov[1].iov_len = httpResponse.fileLen();
        iovCnt=2;
    }
    return true;
}

int HttpHandler::toWriteBytes(){
    return iov[0].iov_len + iov[1].iov_len;
}

bool HttpHandler::isAlive()const {
    return httpRequest.isAlive();
}