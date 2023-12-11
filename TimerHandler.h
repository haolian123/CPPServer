#ifndef TIMER_HANDLER
#define TIMER_HANDLER 

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <assert.h> 
#include <cassert>
#include <chrono>

typedef std::function<void()> TimeoutCallBackFunction;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MilliSeconds;
typedef Clock::time_point TimeStamp;

class timerNode{
public:
    int id;
    TimeStamp expires;
    TimeoutCallBackFunction  callBackFunction;
    bool operator<(const timerNode&t){
        return expires<t.expires;
    }
};


class TimerHandler{
private:
    void deleteNode(size_t i);

    void siftUp(size_t i);

    bool siftDown(size_t index, size_t n);

    void swapNode(size_t i, size_t j);

    std::vector<timerNode> heap;

    std::unordered_map<int,size_t> ref;
public:
    TimerHandler(){heap.reserve(64);}
    ~TimerHandler(){clear();}

    void adjust(int id, int newExpires);

    void add(int id, int timeOut, const TimeoutCallBackFunction& cb);

    void doWork(int id);

    void clear();

    void tick();

    void pop();

    int getNextTick();
};

#endif 