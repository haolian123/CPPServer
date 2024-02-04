#ifndef TIMER_HANDLER
#define TIMER_HANDLER 

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <cassert> 
#include <chrono>

typedef std::function<void()> TimeoutCallBackFunction;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MilliSeconds;
typedef Clock::time_point TimeStamp;

class TimerNode {
public:
    int id;                      // 定时任务的ID
    TimeStamp expires;           // 定时任务的过期时间点
    TimeoutCallBackFunction callBackFunction; // 定时任务的回调函数

    bool operator<(const TimerNode& t) {
        return expires < t.expires;
    }
};

class TimerHandler {
private:
    void deleteNode(size_t i);

    void siftUp(size_t i);

    bool siftDown(size_t index, size_t n);

    void swapNode(size_t i, size_t j);

    std::vector<TimerNode> heap; // 最小堆，用于管理定时任务
    std::unordered_map<int, size_t> ref; // 用于查找定时任务在堆中的位置

public:
    TimerHandler() { heap.reserve(64); }
    ~TimerHandler() { clear(); }

    // 调整定时任务的过期时间
    void adjust(int id, int newExpires);

    // 添加定时任务
    void add(int id, int timeOut, const TimeoutCallBackFunction& cb);

    // 执行定时任务的回调函数
    void doWork(int id);

    // 清空定时任务
    void clear();

    // 定时任务的主循环，检查是否有任务过期
    void tick();

    // 弹出最早过期的任务
    void pop();

    // 获取下一个需要等待的时间
    int getNextTick();
};

#endif
