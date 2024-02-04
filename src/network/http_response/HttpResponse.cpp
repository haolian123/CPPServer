#include"HttpResponse.h"
#include<iostream>


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
}

HttpResponse::~HttpResponse(){
    fileMapper.unMapFile();
}


void HttpResponse::init(const std::string& srcDir, std::string&path,bool isKeepAlive,int code){
    assert(srcDir!="");
    if(fileMapper.file()){fileMapper.unMapFile();}
    this->code=code;
    this->isKeepAlive=isKeepAlive;
    this->path=path;
    this->srcDir=srcDir;
}

void HttpResponse::makeResponse(Buffer& buffer){
    struct stat& mmFileStat=fileMapper.fileStat();
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
    return fileMapper.file();
}

size_t HttpResponse::fileLen()const{
    return fileMapper.fileLen();
}

void HttpResponse::errorHTML(){
    if(CODE_PATH.count(code)==1){
        path=CODE_PATH.find(code)->second;
        stat((srcDir+path).data(),&fileMapper.fileStat());
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
    buffer.append("Content-type: " +  getFileType() + "\r\n");
}

void HttpResponse::addContent(Buffer&buffer){
    int srcFd = open((srcDir+path).data(),O_RDONLY);
    if(srcFd<0){
        errorContent(buffer,"File NotFound!");
        return;
    }
    
    void* mmRet = mmap(0, fileMapper.fileStat().st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    
    if (mmRet == MAP_FAILED) {
        errorContent(buffer, "File NotFound");
        close(srcFd); 
        return;
    }
    fileMapper.setFile((char*)mmRet);
    close(srcFd);
    buffer.append("Content-length: " + std::to_string(fileMapper.fileStat().st_size) + "\r\n\r\n");
}



std::string HttpResponse::getFileType(){
    return mimeTypes.getFileType(path);
}

void HttpResponse::errorContent(Buffer& buff,const std::string& message){
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

void HttpResponse::unMapFile(){
    fileMapper.unMapFile();
}