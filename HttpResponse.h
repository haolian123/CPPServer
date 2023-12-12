#ifndef HTTP_RESPONSE 
#define HTTP_RESPONSE

#include <unordered_map>
#include <fcntl.h>    
#include <unistd.h>      
#include <sys/stat.h>  
#include <sys/mman.h>    

#include "Buffer.h"

class HttpResponse{
private:
    void addStateLine(Buffer& buffer);
    void addHeader(Buffer& buffer);
    void addContent(Buffer& buffer);
    void errorHTML();
    std::string getFileType();
private:
    int code;
    bool isKeepAlive;

    std::string path;
    std::string srcDir;

    char* mmFile;
    struct stat mmFileStat;

    static const std::unordered_map<std::string,std::string>SUFFIX_TYPE;
    static const std::unordered_map<int,std::string>CODE_STATUS;
    static const std::unordered_map<int,std::string>CODE_PATH;
public:
    HttpResponse();
    ~HttpResponse();
    
    void init(const std::string&srcDir, std::string&path, bool isKeepAlive=false, int code=-1);
    void makeResponse(Buffer& buffer);
    void unMapFile();
    char* file();
    size_t fileLen()const;
    void errorContent(Buffer&buffer, std::string message);
    int getCode()const{return code;}
};
#endif 