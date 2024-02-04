#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <unordered_map>
#include "../Buffer.h"
#include "MimeTypes.h"
#include "FileMapper.h"

class HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();

    // 初始化 HTTP 响应
    void init(const std::string& srcDir, std::string& path, bool isKeepAlive = false, int code = -1);

    // 生成 HTTP 响应
    void makeResponse(Buffer& buffer);

    // 生成错误响应内容
    void errorContent(Buffer& buffer, const std::string& message);

    // 获取响应状态码
    int getCode() const;

    std::string getFileType();

    // 获取内存映射文件的指针
    char* file();

    // 获取内存映射文件的长度
    size_t fileLen() const;

    void unMapFile();
private:
    // 添加响应状态行
    void addStateLine(Buffer& buffer);

    // 添加响应头部
    void addHeader(Buffer& buffer);

    // 添加响应内容
    void addContent(Buffer& buffer);

    // 生成错误页面的 HTML 内容
    void errorHTML();

    int code;                // HTTP 响应状态码
    bool isKeepAlive;        // 是否保持连接
    std::string path;        // 请求路径
    std::string srcDir;      // 资源目录
    FileMapper fileMapper;   // 文件映射管理器
    MimeTypes mimeTypes;     // MIME类型管理器

    // 静态常量，HTTP 状态码与状态信息的映射
    static const std::unordered_map<int, std::string> CODE_STATUS;

    // 静态常量，HTTP 状态码与错误页面路径的映射
    static const std::unordered_map<int, std::string> CODE_PATH;



};

#endif // HTTP_RESPONSE_H
