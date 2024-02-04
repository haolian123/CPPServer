#ifndef HTTP_REQUEST_PARSER_H
#define HTTP_REQUEST_PARSER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <errno.h>     
#include <regex>
#include "AuthManager.h"
#include <cassert>
#include "URLUtility.h"
class HttpRequestParser {
public:
    friend struct HttpRequest;
    // HTTP 请求解析状态枚举
    enum PARSE_STATE {
        REQUEST_LINE,       // 请求行解析状态
        HEADERS,            // 请求头解析状态
        BODY,               // 请求体解析状态
        FINISH,             // 解析完成状态
    };

    HttpRequestParser()=default;
    ~HttpRequestParser()=default;

    bool parseRequestLine(const std::string& line);
    void parseHeader(const std::string& line);
    void parseBody(const std::string& line);
    void parsePath();
    void parsePost();
    void parseFromURLEncoded();
    
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

protected:
    PARSE_STATE state; // 请求解析状态
    std::string method, path, version, body; // 请求方法、路径、版本、请求体
    std::unordered_map<std::string, std::string> header; // 请求头信息
    std::unordered_map<std::string, std::string> post;   // POST 请求数据
    // 静态常量，包含默认 HTML 页面的集合
    static const std::unordered_set<std::string> DEFAULT_HTML;

    // 静态常量，包含默认 HTML 标签和对应的状态码
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;

};

#endif // HTTP_REQUEST_PARSER_H
