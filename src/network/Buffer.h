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
    char* begin(); // 返回buffer的开始指针
    const char* begin() const; // 返回const类型的buffer开始指针
    void makeSpace(size_t len); // 确保buffer有足够空间
    std::vector<char>buffer; // 用于存储数据的动态数组
    std::atomic<std::size_t> readPosition; // 读取位置的原子变量
    std::atomic<std::size_t> writePosition; // 写入位置的原子变量

public:
    Buffer(int initBuffSize=1024); // 构造函数，初始化buffer大小
    ~Buffer()=default; // 默认析构函数

    size_t writeableBytes()const; // 返回可写入的字节数
    size_t readableBytes() const; // 返回可读取的字节数
    size_t prependableBytes() const; // 返回前置区域的字节数

    const char* peek() const; // 返回当前读取位置的指针
    void ensureWriteable(size_t len); // 确保有足够的写入空间
    void hasWritten(size_t len); // 更新写入位置

    void retrieve(size_t len); // 从buffer中取出数据
    void retrieveUntil(const char* end); // 取出直到某个位置的数据

    void retrieveAll(); // 取出所有数据
    std::string retrieveAllToString(); // 将所有数据转换为字符串

    const char* beginWriteConst() const; // 返回写入位置的const指针
    char* beginWrite(); // 返回写入位置的指针

    void append(const std::string&str); // 添加字符串数据
    void append(const char*str,size_t len); // 添加字符数组
    void append(const void* data,size_t len); // 添加任意类型的数据
    void append(const Buffer&buff); // 添加另一个Buffer的内容

    ssize_t readFd(int fd, int* saveErrno); // 从文件描述符读取数据
    ssize_t writeFd(int fd,int * saveErrno); // 向文件描述符写入数据

};

#endif