#include "HttpRequest.h"

const std::unordered_set<std::string> HttpRequest::DEFAULT_HTML{
    "/index", "/register", "/login",
    "/welcome", "/video", "/picture", 
};

const std::unordered_map<std::string,int> HttpRequest::DEFAULT_HTML_TAG{
    {"/register.html",0} , {"/login.html" ,1}
};

void HttpRequest::init(){
    method=path=version=body="";
    state = REQUEST_LINE;
    header.clear();
    post.clear();
}

bool HttpRequest::isAlive() const{
    if(header.count("Connection")==1){
        return header.find("Connection")->second=="keep-alive"&&
        version=="1.1";
    }
    return false;
}

bool HttpRequest::parse(Buffer& buffer){
    const char CRLF[] = "\r\n";
    if(buffer.readableBytes()<=0){
        return false;
    }
    while(buffer.readableBytes()&&state!=FINISH){
        const char* lineEnd=std::search(buffer.peek(),buffer.beginWriteConst(),CRLF,CRLF+2);
        std::string line(buffer.peek(),lineEnd);
        switch(state){
            case REQUEST_LINE:
                if(!parseRequestLine(line)){
                    return false;
                }
                parsePath();
                break;
            case HEADERS:
                parseHeader(line);
                if(buffer.readableBytes()<=2){
                    state = FINISH;
                }
                break;
            case BODY:
                parseBody(line);
                break;
            default:
                break;
        }
        if(lineEnd == buffer.beginWrite()) break;
        buffer.retrieveUntil(lineEnd+2);
    }
    return true;
}

void HttpRequest::parsePath(){
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
bool HttpRequest::parseRequestLine(const std::string& line){
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
void HttpRequest::parseHeader(const std::string& line){
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(std::regex_match(line,subMatch,patten)){
        header[subMatch[1]] = subMatch[2];
    }else{
        state = BODY;
    }
}

void HttpRequest::parseBody(const std::string& line){
    body=line;
    parsePost();
    state=FINISH;
}

int HttpRequest::converHex(char ch){
    if(ch >= 'A' && ch <= 'F') return ch -'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch -'a' + 10;
    return ch;
}


void HttpRequest::parsePost(){
    if(method=="POST" && header["Content-Type"]=="application/x-www-form-urlencoded"){
        parseFromURLEncoded();
        if(DEFAULT_HTML_TAG.count(path)){
            int tag = DEFAULT_HTML_TAG.find(path)->second;
            if(tag==0 || tag==1){
                bool isLogin=(tag==1);
                if(userVerify(post["username"],post["password"],isLogin)){
                    path = "/welcome.html";
                }else{
                    path = "/error.html";
                }
            }
        }
    }
}

void HttpRequest::parseFromURLEncoded(){
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
                num = converHex(body[i+1])*16 + converHex(body[i+2]);
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

bool HttpRequest::userVerify(const std::string& name ,const std::string& passWord,bool isLogin){
    if(name == ""|| passWord== ""){
        return false;
    }
    MYSQL *sql;
    DBConnectionRAII(sql,DBConnectionPool::getInstance());
    assert(sql);
    bool flag=false;
    unsigned int j = 0;
    char order[256] = {0};
    MYSQL_FIELD * fields= nullptr;
    MYSQL_RES* res= nullptr;

    if(!isLogin){
        flag = true ;
    }

    std::snprintf(order,256,"SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());

    if(mysql_query(sql,order)){
        mysql_free_result(res);
        return false;
    }
    res=mysql_store_result(sql);
    j = mysql_num_fields(res);
    fields = mysql_fetch_field(res);
    
    while(MYSQL_ROW row = mysql_fetch_row(res)){
        std::string correctPassword(row[1]);
        if(isLogin){
            if(passWord==correctPassword){
                flag=true;
            }else{
                flag=false;
            }
        }else{
            flag=false;
        }
    }
    mysql_free_result(res);

    if(!isLogin && flag == true){
        bzero(order,256);
        std::snprintf(order,256,"INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), passWord.c_str());
        if(mysql_query(sql,order)){
            flag=false;
        }
        flag=true;
    }
    DBConnectionPool::getInstance()->freeConnection(sql);
    return flag;
}

std::string HttpRequest::getPath() const{
    return path;
}

std::string& HttpRequest::getPath(){
    return path;
}

std::string HttpRequest::getMethod() const{
    return method;
}

std::string HttpRequest::getVersion() const{
    return version;
}

std::string HttpRequest::getPost(const std::string& key) const{
    assert(key!= "");
    if(post.count(key)==1){
        return post.find(key)->second;
    }
    return "";
}

std::string HttpRequest::getPost(const char* key) const{
    assert(key!= nullptr);
    if(post.count(key)==1){
        return post.find(key)->second;
    }
    return "";
}