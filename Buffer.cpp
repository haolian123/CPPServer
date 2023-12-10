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

ssize_t  Buffer::readFd(int fd, int* saveErrno){
    char buff[65535];
    struct iovec iov[2];
    const size_t writable=writeableBytes();
    iov[0].iov_base=begin()+writePosition;
    iov[0].iov_len=writable;
    iov[1].iov_base=buff;
    iov[1].iov_len=sizeof(buff);

    const ssize_t len = readv(fd,iov,2);
    if(len<0){
        *saveErrno=errno;
    }else if(static_cast<size_t>(len)<=writable){
        writePosition+=len;
    } else{
        writePosition=buffer.size();
        append(buff,len-writable);
    }
    return len;
}
ssize_t  Buffer::writeFd(int fd,int * saveErrno){
    size_t readSize=readableBytes();
    ssize_t len=write(fd,peek(),readSize);
    if(len<0){
        *saveErrno=errno;
        return len;
    }
    readPosition+=len;
    return len;
}


char*  Buffer::begin(){
    return &*buffer.begin();
}
const char*  Buffer::begin() const{
    return &*buffer.begin();
}
void  Buffer::makeSpace(size_t len){
    if(writeableBytes()+prependableBytes()<len){
        buffer.resize(writePosition+len+1);
    }else{
        size_t readable=readableBytes();
        std::copy(begin()+readPosition,begin()+writePosition,begin());
        readPosition=0;
        writePosition= readPosition+ readable;
        assert(readable==readableBytes());
    }
}