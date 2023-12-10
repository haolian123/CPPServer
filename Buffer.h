#ifndef BUFFER
#define BUFFER 
#include <cstring> 
#include <iostream>
#include <unistd.h>  
#include <sys/uio.h> 
#include <vector> 
#include <atomic>
#include<cassert>
#include <assert.h>
class Buffer{
private:
    char* begin();
    const char* begin() const;
    void makeSpace(size_t len);
    std::vector<char>buffer;
    std::atomic<std::size_t> readPosition;
    std::atomic<std::size_t> writePosition;
public:
    Buffer(int initBuffSize=1024);
    ~Buffer()=default;

    size_t writeableBytes()const;
    size_t readableBytes() const;
    size_t prependableBytes() const;

    const char* peek() const;
    void ensureWriteable(size_t len);
    void hasWritten(size_t len);

    void retrieve(size_t len);
    void retrieveUntil(const char* end);

    void retrieveAll();
    std::string retrieveAllToString();

    const char* beginWriteConst() const;
    char* beginWrite();

    void append(const std::string&str);
    void append(const char*str,size_t len);
    void append(const void* data,size_t len);
    void append(const Buffer&buff);

    ssize_t readFd(int fd, int* saveErrno);
    ssize_t writeFd(int fd,int * saveErrno);

};


#endif