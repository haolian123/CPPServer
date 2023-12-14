#ifndef LOG 
#define LOG 
#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>           
#include <assert.h>
#include <sys/stat.h> 

#include"BlockQueue.h"
#include"Buffer.h"

class Log{
private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;
    const char* path;
    const char* suffix;

    int maxLines;
    int lineCount;
    int toDay;
    bool open;
    Buffer buffer;
    int level;
    bool isAsync;

    FILE* fp;
    std::unique_ptr<BlockQueue<std::string>> deque;
    std::unique_ptr<std::thread> writeThread;
    std::mutex mutex;
private:
    Log();
    void appendLogLevelTitle(int level);
    virtual ~Log();
    void asyncWrite();    
public:
    void init(int level, const char*path="./log",
        const char* suffix=".log",
        int maxQueueCapacity = 1024
    );
    static Log* getInstance();
    static void flushLogThread();

    void write(int level , const char* format, ...);
    void flush();

    int getLevel();
    void setLevel(int level);
    bool isOpen() {return open;}

};
#define LOG_BASE(level, format, ...)\
    do {\
        Log* log=Log::getInstance();\
        if(log->isOpen()&&log->getLevel()<=level){\
            log->write(level,format,##__VA_ARGS__);\
            log->flush();\
        }\
    }while(0);
#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);
#endif 