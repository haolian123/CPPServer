#include"MimeTypes.h"

std::unordered_map<std::string, std::string> MimeTypes::suffixTypeMap = {
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

MimeTypes::MimeTypes(){

}
MimeTypes::~MimeTypes(){

}

// 获取文件的 MIME 类型
std::string MimeTypes::getFileType(const std::string& path){
    std::string::size_type idx = path.find_last_of('.');
    if(idx==std::string::npos){
        return "text/plain";
    }
    std::string suffix=path.substr(idx);
    if(suffixTypeMap.count(suffix)==1){
        return suffixTypeMap.find(suffix)->second;
    }
    return "text/plain";
}