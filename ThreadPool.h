#ifndef THREADPOOL
#define THREADPOOL
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include<cassert>
#include <functional>
struct Pool{
    std::mutex mutex;
    std::condition_variable condition;
    bool isClose;
    std::queue<std::function<void()>> tasks;
};
class ThreadPool{
private:
    std::shared_ptr<Pool>pool;
public:
    explicit ThreadPool(size_t threadNumber=8):pool(std::make_shared<Pool>()){
        assert(threadNumber>0);
        for(size_t i=0;i<threadNumber;i++){
            std::thread(
                [pool=this->pool]{
                    std::unique_lock<std::mutex>locker(pool->mutex);
                    while(true){
                        if(!pool->tasks.empty()){
                            auto task=std::move(pool->tasks.front());
                            pool->tasks.pop();
                            locker.unlock();
                            task();
                            locker.lock();
                        }else if(pool->isClose){
                            break;
                        }else{
                            pool->condition.wait(locker);
                        }
                    }
                }
            ).detach();
        }
    }
    ThreadPool()=default;
    ThreadPool(ThreadPool&&) = default;

    ~ThreadPool(){
        if(pool!=nullptr){
            std::lock_guard<std::mutex>locker(pool->mutex);
            pool->isClose=true;
        }
        pool->condition.notify_all();
    }
    template<class F>
    void addTask(F&& task){
        {
            std::lock_guard<std::mutex>locker(pool->mutex);
            pool->tasks.emplace(std::forward<F>(task));
        }
        pool->condition.notify_one();
    }
};







#endif 