#ifndef BLOCKQUEUE 
#define BLOCKQUEUE 

#include <mutex>
#include <deque>
#include <condition_variable>
#include <sys/time.h>

template<class T>
class BlockQueue{
private:
    std::deque<T> deque;
    size_t capacity;
    std::mutex mutex;
    bool isClose;
    std::condition_variable condConsumer;
    std::condition_variable condProducer;
public:
    explicit BlockQueue(size_t maxCapacity=1000);
    ~BlockQueue();
    void clear();
    bool empty();
    bool full();
    void close();
    size_t size();
    size_t getCapacity();
    T front();
    T back();
    void push_back(const T&item);
    void push_front(const T&item);
    bool pop(T&item);
    bool pop(T&item, int timeout);
    void flush();
};

template<class T>
BlockQueue<T>::BlockQueue(size_t maxCapacity):capacity(maxCapacity){
    assert(maxCapacity>0);
    isClose=false;
}

template<class T>
BlockQueue<T>::~BlockQueue(){
    this->close();
}

template<class T>
void BlockQueue<T>::close(){
    {
        std::lock_guard<std::mutex> locker(mutex);
        deque.clear();
        isClose=true;
    }
    condProducer.notify_all();
    condConsumer.notify_all();
}

template<class T>
void BlockQueue<T>::flush(){
    condConsumer.notify_one();
}

template<class T>
void BlockQueue<T>::clear(){
    std::lock_guard<std::mutex> locker(mutex);
    deque.clear();
}

template<class T>
T BlockQueue<T>::front(){
    std::lock_guard<std::mutex> locker(mutex);
    return deque.front();
}

template<class T>
T BlockQueue<T>::back(){
    std::lock_guard<std::mutex> locker(mutex);
    return deque.back();
}

template<class T>
size_t BlockQueue<T>::size(){
    std::lock_guard<std::mutex> locker(mutex);
    return deque.size();
}

template<class T>
size_t BlockQueue<T>::getCapacity(){
    std::lock_guard<std::mutex> locker(mutex);
    return capacity;
}

template<class T>
void BlockQueue<T>::push_back(const T &item){
    std::unique_lock<std::mutex> locker(mutex);
    while(deque.size()>=capacity){
        condProducer.wait(locker);
    }
    deque.emplace_back(item);
    condConsumer.notify_one();
}
template<class T>
void BlockQueue<T>::push_front(const T &item){
    std::unique_lock<std::mutex> locker(mutex);
    while(deque.size()>=capacity){
        condProducer.wait(locker);
    }
    deque.emplace_front(item);
    condConsumer.notify_one();
}

template<class T>
bool BlockQueue<T>::empty(){
    std::lock_guard<std::mutex> locker(mutex);
    return deque.empty();
}
template<class T>
bool BlockQueue<T>::full(){
    std::lock_guard<std::mutex> locker(mutex);
    return deque.size()>=capacity;
}
template<class T>
bool BlockQueue<T>::pop(T&item){
    std::unique_lock<std::mutex> locker(mutex);
    while(deque.empty()){
        condConsumer.wait(locker);
        if(isClose){
            return false;
        }
    }
    item=deque.front();
    deque.pop_front();
    condProducer.notify_one();
    return true;
}
template<class T>
bool BlockQueue<T>::pop(T&item,int timeout){
    std::unique_lock<std::mutex> locker(mutex);
    while(deque.empty()){
        if(condConsumer.wait_for(locker, std::chrono::seconds(timeout)) 
                == std::cv_status::timeout){
            return false;
        }
        if(isClose){
            return false;
        }
    }
    item=deque.front();
    deque.pop_front();
    condProducer.notify_one();
    return true;
}


#endif 