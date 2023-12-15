#include"Buffer.h"


Buffer::Buffer(int initBuffSize)
:buffer(initBuffSize),readPosition(0),writePosition(0)
{}

size_t  Buffer::writeableBytes()const{
    return buffer.size()-writePosition;
}
size_t  Buffer::readableBytes() const{
    return writePosition-readPosition;
}
size_t  Buffer::prependableBytes() const{
    return readPosition;
}

const char*  Buffer::peek() const{
    return begin()+readPosition;
}
void  Buffer::ensureWriteable(size_t len){
    if(writeableBytes()<len){
        makeSpace(len);
    }
    assert(writeableBytes()>len);
}
void  Buffer::hasWritten(size_t len){
    writePosition+=len;
}

void  Buffer::retrieve(size_t len){
    assert(len<=readableBytes());
    readPosition+=len;
}
void  Buffer::retrieveUntil(const char* end){
    assert(peek()<=end);
    retrieve(end-peek());
}

void  Buffer::retrieveAll(){
    bzero(&buffer[0],buffer.size());
    readPosition=writePosition=0;
}
std::string  Buffer::retrieveAllToString(){
    std::string str(peek(),readableBytes());
    retrieveAll();
    return str;
}

const char*  Buffer::beginWriteConst() const{
    return begin()+writePosition;
}
char*  Buffer::beginWrite(){
    return begin()+writePosition;
}

void  Buffer::append(const std::string&str){
    append(str.data(),str.length());
}
void  Buffer::append(const char*str,size_t len){
    assert(str);
    ensureWriteable(len);
    std::copy(str,str+len,beginWrite());
    hasWritten(len);
}

void  Buffer::append(const void* data,size_t len){
    assert(data);
    append(static_cast<const char*>(data),len);
}
void  Buffer::append(const Buffer&buff){
    append(buff.peek(),buff.readableBytes());
}

ssize_t Buffer::readFd(int fd, int* saveErrno){
    char buff[65535]; // 临时缓冲区
    struct iovec iov[2]; // iovec结构数组，用于readv操作
    const size_t writable = writeableBytes(); // 可写入字节数
    iov[0].iov_base = begin() + writePosition; // 指向Buffer中的写入位置
    iov[0].iov_len = writable; // 第一部分长度为Buffer的可写长度
    iov[1].iov_base = buff; // 第二部分为临时缓冲区
    iov[1].iov_len = sizeof(buff); // 第二部分长度

    const ssize_t len = readv(fd, iov, 2); // 使用readv从文件描述符读取数据
    if(len < 0){
        *saveErrno = errno; // 如果读取失败，设置错误号
    } else if(static_cast<size_t>(len) <= writable){
        writePosition += len; // 如果读取长度小于等于可写长度，更新写入位置
    } else {
        writePosition = buffer.size(); // 如果超出可写长度，填满Buffer
        append(buff, len - writable); // 将剩余数据追加到Buffer
    }
    return len;
}

ssize_t Buffer::writeFd(int fd, int * saveErrno){
    size_t readSize = readableBytes(); // 可读取的字节数
    ssize_t len = write(fd, peek(), readSize); // 将Buffer中的数据写入文件描述符
    if(len < 0){
        *saveErrno = errno; // 如果写入失败，设置错误号
        return len;
    }
    readPosition += len; // 更新读取位置
    return len;
}



char*  Buffer::begin(){
    return &*buffer.begin();
}
const char*  Buffer::begin() const{
    return &*buffer.begin();
}

void Buffer::makeSpace(size_t len) {
    // 如果可写字节数加上前置字节数小于需要的长度
    if (writeableBytes() + prependableBytes() < len) {
        // 调整缓冲区大小
        buffer.resize(writePosition + len + 1);
    } else {
        // 计算可读字节数
        size_t readable = readableBytes();
        
        // 将数据从读位置开始复制到写位置
        std::copy(begin() + readPosition, begin() + writePosition, begin());
        
        // 更新读位置和写位置
        readPosition = 0;
        writePosition = readPosition + readable;
        
        assert(readable == readableBytes());
    }
}
