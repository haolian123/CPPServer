#ifndef HTTP_RESPONSE 
#define HTTP_RESPONSE

#include <unordered_map>
#include <fcntl.h>    
#include <unistd.h>      
#include <sys/stat.h>  
#include <sys/mman.h>    

#include "Buffer.h"

class HttpResponse {
private:
    // 添加响应状态行
    void addStateLine(Buffer& buffer);

    // 添加响应头部
    void addHeader(Buffer& buffer);

    // 添加响应内容
    void addContent(Buffer& buffer);

    // 生成错误页面的 HTML 内容
    void errorHTML();

    // 获取文件的 MIME 类型
    std::string getFileType();

private:
    int code;                // HTTP 响应状态码
    bool isKeepAlive;        // 是否保持连接

    std::string path;        // 请求路径
    std::string srcDir;      // 资源目录

    char* mmFile;            // 内存映射文件
    struct stat mmFileStat;  // 内存映射文件的状态信息

    // 静态常量，文件后缀名与 MIME 类型的映射
    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;

    // 静态常量，HTTP 状态码与状态信息的映射
    static const std::unordered_map<int, std::string> CODE_STATUS;

    // 静态常量，HTTP 状态码与错误页面路径的映射
    static const std::unordered_map<int, std::string> CODE_PATH;

public:
    // 构造函数
    HttpResponse();

    // 析构函数
    ~HttpResponse();

    // 初始化 HTTP 响应
    void init(const std::string& srcDir, std::string& path, bool isKeepAlive = false, int code = -1);

    // 生成 HTTP 响应
    void makeResponse(Buffer& buffer);

    // 取消内存映射文件
    void unMapFile();

    // 获取内存映射文件的指针
    char* file();

    // 获取内存映射文件的长度
    size_t fileLen() const;

    // 生成错误响应内容
    void errorContent(Buffer& buffer, std::string message);

    // 获取响应状态码
    int getCode() const { return code; }
};

#endif 