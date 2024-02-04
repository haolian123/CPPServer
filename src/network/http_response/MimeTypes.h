#ifndef MIME_TYPES_H
#define MIME_TYPES_H

#include <unordered_map>
#include <string>

class MimeTypes {
public:
    MimeTypes();
    ~MimeTypes();

    // 获取文件的 MIME 类型
    std::string getFileType(const std::string& path);

private:
    // 文件后缀名与 MIME 类型的映射
    static std::unordered_map<std::string, std::string> suffixTypeMap;
};

#endif 
