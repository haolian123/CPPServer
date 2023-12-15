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

class HttpRequest{
public:
    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };

    enum HTTP_CODE{
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };
private:
    bool parseRequestLine( const std::string& line);
    void parseHeader( const std::string& line);
    void parseBody (const std::string& line);

    void parsePath();
    void parsePost();
    void parseFromURLEncoded();

    static bool userVerify(const std::string& name,const std::string& pwd,bool isLogin);

    PARSE_STATE state;
    std::string method,path,version,body;
    std::unordered_map<std::string,std::string>header;
    std::unordered_map<std::string,std::string>post;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string,int> DEFAULT_HTML_TAG;
    static int converHex(char ch);
public:
    HttpRequest(){init();}
    ~HttpRequest()= default;
    void init();
    bool parse(Buffer& buffer);

    std::string getPath() const;
    std::string& getPath();
    std::string getMethod() const;
    std::string getVersion() const;
    std::string getPost(const std::string& key) const;
    std::string getPost(const char* key) const;

    bool isAlive() const; 
};
#endif 