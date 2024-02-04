#include "HttpRequestParser.h"

const std::unordered_set<std::string> HttpRequestParser::DEFAULT_HTML{
    "/index", "/register", "/login",
    "/welcome", "/video", "/picture", 
};

const std::unordered_map<std::string,int> HttpRequestParser::DEFAULT_HTML_TAG{
    {"/register.html",0} , {"/login.html" ,1}
};


bool HttpRequestParser::parseRequestLine(const std::string& line) {
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, patten)) {   
        method = subMatch[1];
        path = subMatch[2];
        version = subMatch[3];
        state=  HEADERS;
        return true;
    }
    return false;
}

void HttpRequestParser::parseHeader(const std::string& line) {
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(std::regex_match(line,subMatch,patten)){
        header[subMatch[1]] = subMatch[2];
    }else{
        state = BODY;
    }
}

void HttpRequestParser::parseBody(const std::string& line) {
    body=line;
    parsePost();
    state=FINISH;
}

void HttpRequestParser::parsePath() {
    if(path=="/"){
        path= "/index.html";
    }else{
        for(auto& item:DEFAULT_HTML){
            if(item==path){
                path+=".html";
                break;
            }
        }
    }
}

void HttpRequestParser::parsePost() {
    if(method=="POST" && header["Content-Type"]=="application/x-www-form-urlencoded"){
        parseFromURLEncoded();
        if(DEFAULT_HTML_TAG.count(path)){
            int tag = DEFAULT_HTML_TAG.find(path)->second;
            if(tag==0 || tag==1){
                bool isLogin=(tag==1);
                if(AuthManager::userVerify(post["username"],post["password"],isLogin)){
                    path = "/welcome.html";
                }else{
                    path = "/error.html";
                }
            }
        }
    }
}

void HttpRequestParser::parseFromURLEncoded() {
    if(body.size()==0){
        return;
    }
    std::string key,value;
    int num=0;
    int n=body.size();
    int i=0, j=0;

    for(;i<n;i++){
        char ch = body[i];
        switch(ch){
            case  '=':
                key = body.substr(j,i-j);
                j=i+1;
                break;
            case '+':
                body[i] = ' ';
                break;
            case '%':
                num = URLUtility::convertHex(body[i+1])*16 + URLUtility::convertHex(body[i+2]);
                body[i+2] = num%10+'0';
                body[i+1] = num/10 + '0';
                i+=2;
                break;
            case '&':
                value = body.substr(j,i-j);
                j=i+1;
                post[key]=value;
                break;
            default:
                break;
        }

    }
    assert(j<=i);
    if(post.count(key)==0&& j<i){
        value=body.substr(j,i-j);
        post[key]=value;
    }
}

// 获取请求路径
std::string HttpRequestParser::getPath() const{
    return path;
}
std::string& HttpRequestParser::getPath(){
    return path;
}

// 获取请求方法
std::string HttpRequestParser::getMethod() const{
    return method;
}

// 获取请求版本
std::string HttpRequestParser::getVersion() const{
    return version;
}

// 获取 POST 请求数据
std::string HttpRequestParser::getPost(const std::string& key) const{
    assert(key!= "");
    if(post.count(key)==1){
        return post.find(key)->second;
    }
    return "";
}
std::string HttpRequestParser::getPost(const char* key) const{
    assert(key!= nullptr);
    if(post.count(key)==1){
        return post.find(key)->second;
    }
    return "";
}

