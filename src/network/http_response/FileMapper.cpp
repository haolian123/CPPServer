#include"FileMapper.h"


FileMapper::FileMapper() : mmFile(nullptr), mmFileStat({}) {
}

FileMapper::~FileMapper() {
    unMapFile(); // 确保资源被释放
}

bool FileMapper::mapFile(const std::string& srcDir, const std::string& path) {
    std::string fullPath = srcDir + path;
    int srcFd = open(fullPath.data(), O_RDONLY);
    if (srcFd < 0) {
        return false;
    }

    // 获取文件的状态信息
    if (fstat(srcFd, &mmFileStat) < 0) {
        close(srcFd);
        return false;
    }

    void* mmRet = mmap(0, mmFileStat.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    close(srcFd); // 映射完成后，可以关闭文件描述符

    if (mmRet == MAP_FAILED) {
        return false;
    }

    mmFile = static_cast<char*>(mmRet);
    return true;
}

// 取消内存映射文件
void FileMapper::unMapFile(){
    if(mmFile){
        munmap(mmFile,mmFileStat.st_size);
        mmFile=nullptr;
    }
}

// 获取内存映射文件的指针
char* FileMapper::file(){
    return mmFile;
}

// 获取内存映射文件的长度
size_t FileMapper::fileLen() const{
    return mmFileStat.st_size;
}

struct stat& FileMapper::fileStat(){
    return this->mmFileStat;
}

void FileMapper::setFile(char* file){
    this->mmFile = file;
}