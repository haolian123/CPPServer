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

class Log {
private:
    static const int LOG_PATH_LEN = 256;     // 日志文件路径的最大长度
    static const int LOG_NAME_LEN = 256;     // 日志文件名的最大长度
    static const int MAX_LINES = 50000;      // 最大日志行数

    const char* path;                        // 日志文件路径
    const char* suffix;                      // 日志文件后缀

    int maxLines;                            // 最大日志行数
    int lineCount;                           // 当前已记录的日志行数
    int toDay;                               // 记录日志的日期
    bool open;                               // 是否已打开日志
    Buffer buffer;                           // 日志缓冲区
    int level;                               // 日志级别
    bool isAsync;                            // 是否异步写入日志

    FILE* fp;                                 // 文件指针，用于打开日志文件
    std::unique_ptr<BlockQueue<std::string>> deque;  // 阻塞队列，用于异步写入日志
    std::unique_ptr<std::thread> writeThread;       // 异步写入日志的线程
    std::mutex mutex;                              // 互斥锁，用于保护日志的写入

private:
    // 构造函数
    Log();

    // 添加日志级别标题
    void appendLogLevelTitle(int level);

    // 析构函数
    virtual ~Log();

    // 异步写入日志
    void asyncWrite();

public:
    // 初始化日志
    void init(int level, const char* path = "./log",
              const char* suffix = ".log",
              int maxQueueCapacity = 1024
    );

    // 获取 Log 实例
    static Log* getInstance();

    // 刷新日志线程
    static void flushLogThread();

    // 写入日志
    void write(int level, const char* format, ...);

    // 刷新日志
    void flush();

    // 获取日志级别
    int getLevel();

    // 设置日志级别
    void setLevel(int level);

    // 检查日志是否打开
    bool isOpen() { return open; }
};
// 日志宏定义，用于方便记录日志
#define LOG_BASE(level, format, ...)\
    do {\
        Log* log = Log::getInstance();\
        if (log->isOpen() && log->getLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__);\
            log->flush();\
        }\
    } while (0);

// 不同日志级别的宏定义
#define LOG_DEBUG(format, ...) do { LOG_BASE(0, format, ##__VA_ARGS__) } while (0);
#define LOG_INFO(format, ...) do { LOG_BASE(1, format, ##__VA_ARGS__) } while (0);
#define LOG_WARN(format, ...) do { LOG_BASE(2, format, ##__VA_ARGS__) } while (0);
#define LOG_ERROR(format, ...) do { LOG_BASE(3, format, ##__VA_ARGS__) } while (0);
#endif 