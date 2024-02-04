#ifndef FILE_MAPPER_H
#define FILE_MAPPER_H

#include <string>
#include <fcntl.h>    
#include <unistd.h>      
#include <sys/stat.h>  
#include <sys/mman.h> 

class FileMapper {
public:
    FileMapper();
    ~FileMapper();

    // 映射文件到内存
    bool mapFile(const std::string& srcDir, const std::string& path);

    // 取消内存映射文件
    void unMapFile();

    // 获取内存映射文件的指针
    char* file();

    // 获取内存映射文件的长度
    size_t fileLen() const;

    void setFile(char* file);

    struct stat& fileStat();

private:
    char* mmFile;            // 内存映射文件
    struct stat mmFileStat;  // 内存映射文件的状态信息
};

#endif // FILE_MAPPER_H
