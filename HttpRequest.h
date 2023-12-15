#ifndef HTTP_REQUEST 
#define HTTP_REQUEST 

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>     
#include <mysql/mysql.h>  //mysql
#include "Buffer.h"
#include "DBConnectionPool.h"
#include "DBConnectionRAII.h"

class HttpRequest {
public:
    // HTTP 请求解析状态枚举
    enum PARSE_STATE {
        REQUEST_LINE,       // 请求行解析状态
        HEADERS,            // 请求头解析状态
        BODY,               // 请求体解析状态
        FINISH,             // 解析完成状态
    };

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

private:
    // 解析请求行
    bool parseRequestLine(const std::string& line);

    // 解析请求头
    void parseHeader(const std::string& line);

    // 解析请求体
    void parseBody(const std::string& line);

    // 解析请求路径
    void parsePath();

    // 解析 POST 请求数据
    void parsePost();

    // 从 URL 编码中解析 POST 数据
    void parseFromURLEncoded();

    // 静态方法，用户验证
    static bool userVerify(const std::string& name, const std::string& pwd, bool isLogin);

    PARSE_STATE state; // 请求解析状态
    std::string method, path, version, body; // 请求方法、路径、版本、请求体
    std::unordered_map<std::string, std::string> header; // 请求头信息
    std::unordered_map<std::string, std::string> post;   // POST 请求数据

    // 静态常量，包含默认 HTML 页面的集合
    static const std::unordered_set<std::string> DEFAULT_HTML;

    // 静态常量，包含默认 HTML 标签和对应的状态码
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;

    // 静态方法，将字符转换为十六进制数值
    static int convertHex(char ch);

public:
    // 构造函数
    HttpRequest() { init(); }

    // 析构函数
    ~HttpRequest() = default;

    // 初始化请求对象
    void init();

    // 解析 HTTP 请求
    bool parse(Buffer& buffer);

    // 获取请求路径
    std::string getPath() const;
    std::string& getPath();

    // 获取请求方法
    std::string getMethod() const;

    // 获取请求版本
    std::string getVersion() const;

    // 获取 POST 请求数据
    std::string getPost(const std::string& key) const;
    std::string getPost(const char* key) const;

    // 检查连接是否存活
    bool isAlive() const;
};

#endif 