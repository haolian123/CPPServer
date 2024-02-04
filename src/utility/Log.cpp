
#include"Log.h"

Log::Log(){
    lineCount = 0;
    isAsync = false;
    writeThread = nullptr;
    deque = nullptr;
    toDay = 0;
    fp = nullptr;
}

Log::~Log(){
    if(writeThread && writeThread->joinable()){
        while(!deque->empty()){
            deque->flush();
        };
        deque->close();
        writeThread->join();
    }
    if(fp){
        std::lock_guard<std::mutex> locker(mutex);
        flush();
        fclose(fp);
    }
}

int Log::getLevel(){
    std::lock_guard<std::mutex> locker(mutex);
    return level;
}

void Log::setLevel(int level){
    std::lock_guard<std::mutex> locker(mutex);
    this->level=level;
}

void Log::init(int level=1, const char* path,const char* suffix,int maxQueueSize){
    open = true;
    this->level = level;
    if(maxQueueSize>0){
        isAsync = true;
        if(!deque){
            std::unique_ptr<BlockQueue<std::string>> newDeque(new BlockQueue<std::string>);
            deque=std::move(newDeque);

            std::unique_ptr<std::thread> newThread(new std::thread(flushLogThread));
            writeThread = std::move(newThread);
        }
    }else{
        isAsync = false;
    }

    lineCount = 0;
    time_t timer=time(nullptr);
    struct tm* sysTime = localtime(&timer);
    struct tm t = *sysTime;
    this->path=path;
    this->suffix = suffix;
    char fileName[LOG_NAME_LEN] = {0};
    std::snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", 
        path, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix);
    toDay = t.tm_mday;

    {
        std::lock_guard<std::mutex> locker(mutex);
        buffer.retrieveAll();
        if(fp){
            flush();
            std::fclose(fp);
        }
        fp=std::fopen(fileName,"a");
        if(fp==nullptr){
            mkdir(path,0777);
            fp = std::fopen(fileName,"a");
        }
        assert(fp!=nullptr);
    }
}

void Log::flush(){
    if(isAsync){
        deque->flush();
    }
    std::fflush(fp);
}

void Log::asyncWrite(){
    std::string str = "";
    while(deque -> pop(str)){
        std::lock_guard<std::mutex> locker(mutex);
        std::fputs(str.c_str(),fp);
    }
}

Log* Log::getInstance(){
    static Log inst;
    return &inst;
}

void Log::flushLogThread(){
    Log::getInstance()->asyncWrite();
}

void Log::appendLogLevelTitle(int level){
    switch(level){
        case 0:
        buffer.append("[debug]: ", 9);
        break;
    case 1:
        buffer.append("[info] : ", 9);
        break;
    case 2:
        buffer.append("[warn] : ", 9);
        break;
    case 3:
        buffer.append("[error]: ", 9);
        break;
    default:
        buffer.append("[info] : ", 9);
        break;
    }
}

void Log::write(int level, const char* format, ...) {
    struct timeval now = {0,0};
    gettimeofday(&now, nullptr); // 获取当前时间，精确到微秒
    time_t tSec = now.tv_sec; // 将时间转换为秒
    struct tm* sysTime = localtime(&tSec); // 将秒转换为本地时间
    struct tm t = *sysTime; // 从指针复制本地时间结构体
    va_list vaList; // 初始化一个可变参数列表

    // 检查是否需要创建新文件（基于日期更换或行数超过最大行数）
    if(toDay != t.tm_mday || (lineCount && (lineCount % MAX_LINES == 0))) {
        std::unique_lock<std::mutex> locker(mutex); // 对互斥量上锁
        locker.unlock(); // 立即解锁（可能是为了缩短锁持有时间）
        char newFile[LOG_NAME_LEN]; // 存储新文件名的数组
        char tail[36] = {0}; // 用于存储日期部分的字符串
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday); // 格式化日期字符串

        if (toDay != t.tm_mday) { // 如果日期更换
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path, tail, suffix); // 创建基于日期的新文件名
            toDay = t.tm_mday; // 更新当前日期变量
            lineCount = 0; // 重置行计数器
        } else {
            // 如果行数超过最大行数但日期未更换
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path, tail, (lineCount / MAX_LINES), suffix); // 创建基于日期和行数的新文件名
        }

        locker.lock(); // 再次上锁
        flush(); // 刷新缓冲区
        fclose(fp); // 关闭当前日志文件
        fp = std::fopen(newFile, "a"); // 打开新的日志文件
        assert(fp != nullptr); // 确保文件打开成功
    }

    {
        std::unique_lock<std::mutex> locker(mutex); // 对互斥量上锁
        lineCount++; // 行数加一
        // 将当前时间格式化并写入缓冲区
        int n = snprintf(buffer.beginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                         t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                         t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
        buffer.hasWritten(n); // 更新缓冲区已写入的数据长度
        appendLogLevelTitle(level); // 添加日志级别的标题

        va_start(vaList, format); // 初始化可变参数列表
        // 将格式化的日志信息写入缓冲区
        int m = vsnprintf(buffer.beginWrite(), buffer.writeableBytes(), format, vaList);
        va_end(vaList); // 结束可变参数列表

        buffer.hasWritten(m); // 更新缓冲区已写入的数据长度
        buffer.append("\n\0", 2); // 在缓冲区末尾添加换行符和字符串结束符
        if (isAsync && deque && !deque->full()) { // 如果处于异步模式且队列未满
            deque->push_back(buffer.retrieveAllToString()); // 将缓冲区内容添加到队列
        } else {
            std::fputs(buffer.peek(), fp); // 否则，将缓冲区内容写入文件
        }
        buffer.retrieveAll(); // 清空缓冲区
    }
}
