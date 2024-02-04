#ifndef HTTP_REQUEST 
#define HTTP_REQUEST 

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>     
#include <mysql/mysql.h>  //mysql
#include "../Buffer.h"
#include "../../db/DBConnectionPool.h"
#include "../../db/DBConnectionRAII.h"
#include "HttpRequestParser.h"
#include "AuthManager.h"
#include "URLUtility.h"

class HttpRequest {
public:
    // HTTP 请求状态枚举
    enum HTTP_CODE {
        NO_REQUEST = 0,             // 无请求
        GET_REQUEST,                // GET 请求
        BAD_REQUEST,                // 请求错误
        NO_RESOURCE,                // 资源不存在
        FORBIDDEN_REQUEST,          // 禁止请求
        FILE_REQUEST,               // 文件请求
        INTERNAL_ERROR,             // 内部错误
        CLOSED_CONNECTION,          // 连接关闭
    };

    HttpRequest();
    ~HttpRequest();

    void init();
    bool parse(Buffer& buffer);

    std::string getPath() const;
    std::string& getPath();
    std::string getMethod() const;
    std::string getVersion() const;
    std::string getPost(const std::string& key) const;
    std::string getPost(const char* key) const;
    bool isAlive() const;

private:
    HttpRequestParser parser;
    AuthManager authManager;
    URLUtility urlUtility;
};

#endif 