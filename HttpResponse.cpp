#include"HttpResponse.h"

const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};

const std::unordered_map<int, std::string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};


const std::unordered_map<int, std::string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

HttpResponse::HttpResponse(){
    code=-1;
    path=srcDir="";
    isKeepAlive=false;
    mmFile=nullptr;
    mmFileStat={0};
}

HttpResponse::~HttpResponse(){
    unMapFile();
}


void HttpResponse::init(const std::string& srcDir, std::string&path,bool isKeepAlive,int code){
    assert(srcDir!="");
    if(mmFile){unMapFile();}
    this->code=code;
    this->isKeepAlive=isKeepAlive;
    this->path=path;
    this->srcDir=srcDir;
    mmFile=nullptr;
    mmFileStat={0};
}

void HttpResponse::makeResponse(Buffer& buffer){
    if(stat((srcDir+path).data(),&mmFileStat)<0||S_ISDIR(mmFileStat.st_mode)){
        code=404;
    }else if(!(mmFileStat.st_mode&S_IROTH)){
        code = 403;
    }else if(code==-1){
        code=200;
    }
    errorHTML();
    addStateLine(buffer);
    addHeader(buffer);
    addContent(buffer);
}

char* HttpResponse::file(){
    return mmFile;
}

size_t HttpResponse::fileLen()const{
    return mmFileStat.st_size;
}

void HttpResponse::errorHTML(){
    if(CODE_PATH.count(code)==1){
        path=CODE_PATH.find(code)->second;
        stat((srcDir+path).data(),&mmFileStat);
    }
}

void HttpResponse::addStateLine(Buffer& buffer){
    std::string status;
    if(CODE_STATUS.count(code)==1){
        status = CODE_STATUS.find(code)->second;
    }else{
        code=400;
        status = CODE_STATUS.find(400)->second;
    }
    buffer.append("HTTP/1.1 " + std::to_string(code) + " " + status + "\r\n");
}

void HttpResponse::addHeader(Buffer& buffer){
    buffer.append("Connection: ");
    if(isKeepAlive){
        buffer.append("keep-alive\r\n");
        buffer.append("keep-alive: max=6, timeout=120\r\n");
    }else{
        buffer.append("close\r\n");
    }
    buffer.append("Content-type: " + getFileType() + "\r\n");
}

void HttpResponse::addContent(Buffer&buffer){
    int srcFd = open((srcDir+path).data(),O_RDONLY);
    if(srcFd<0){
        errorContent(buffer,"File NotFound!");
        return;
    }
    
    void* mmRet = mmap(0, mmFileStat.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    
    if (mmRet == MAP_FAILED) {
        errorContent(buffer, "File NotFound");
        close(srcFd); 
        return;
    }
    mmFile = (char*)mmRet;
    close(srcFd);
    buffer.append("Content-length: " + std::to_string(mmFileStat.st_size) + "\r\n\r\n");
}

void HttpResponse::unMapFile(){
    if(mmFile){
        munmap(mmFile,mmFileStat.st_size);
        mmFile=nullptr;
    }
}

std::string HttpResponse::getFileType(){
    std::string::size_type idx = path.find_last_of('.');
    if(idx==std::string::npos){
        return "text/plain";
    }
    std::string suffix=path.substr(idx);
    if(SUFFIX_TYPE.count(suffix)==1){
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

void HttpResponse::errorContent(Buffer& buff,std::string message){
    std::string body;
    std::string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if(CODE_STATUS.count(code)==1){
        status = CODE_STATUS.find(code)->second;
    }else{
        status = "Bad Request";
    }
    body += std::to_string(code) + " : " + status  + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>myServer</em></body></html>";

    buff.append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buff.append(body);
}