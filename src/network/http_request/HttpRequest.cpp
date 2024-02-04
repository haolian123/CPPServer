#include"HttpRequest.h"

HttpRequest::HttpRequest(){
    init();
}
HttpRequest::~HttpRequest(){}

void HttpRequest::init(){
    parser.method="";
    parser.path="";
    parser.version="";
    parser.body="";
    parser.state = parser.REQUEST_LINE;
    parser.header.clear();
    parser.post.clear();
}
bool HttpRequest::parse(Buffer& buffer){
    const char CRLF[] = "\r\n";
    if(buffer.readableBytes()<=0){
        return false;
    }
    while(buffer.readableBytes()&&parser.state!=parser.FINISH){
        const char* lineEnd=std::search(buffer.peek(),buffer.beginWriteConst(),CRLF,CRLF+2);
        std::string line(buffer.peek(),lineEnd);
        switch(parser.state){
            case parser.REQUEST_LINE:
                if(!parser.parseRequestLine(line)){
                    return false;
                }
                parser.parsePath();
                break;
            case parser.HEADERS:
                parser.parseHeader(line);
                if(buffer.readableBytes()<=2){
                    parser.state = parser.FINISH;
                }
                break;
            case parser.BODY:
                parser.parseBody(line);
                break;
            default:
                break;
        }
        if(lineEnd == buffer.beginWrite()) break;
        buffer.retrieveUntil(lineEnd+2);
    }
    return true;
}

std::string HttpRequest::getPath() const{
    return parser.getPath();
}
std::string& HttpRequest::getPath(){
    return parser.getPath();
}
std::string HttpRequest::getMethod() const{
    return parser.getMethod();
}
std::string HttpRequest::getVersion() const{
    return parser.getVersion();
}
std::string HttpRequest::getPost(const std::string& key) const{
    return parser.getPost(key);
}
std::string HttpRequest::getPost(const char* key) const{
    return parser.getPost(key);
}
bool HttpRequest::isAlive() const{
    if(parser.header.count("Connection")==1){
        return parser.header.find("Connection")->second=="keep-alive"&&
        parser.version=="1.1";
    }
    return false;
}