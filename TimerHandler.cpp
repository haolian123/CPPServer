#include"TimerHandler.h"

void TimerHandler::deleteNode(size_t index){
    assert(!heap.empty()&& index>=0&&index<heap.size());
    size_t i=index;
    size_t n=heap.size()-1;
    assert(i<=n);
    if(i<n){
        swapNode(i,n);
        if(!siftDown(i,n)){
            siftUp(i);
        }
    }
    ref.erase(heap.back().id);
    heap.pop_back();
}

void TimerHandler::siftUp(size_t i){
    assert(i>=0 && i<heap.size());
    size_t j=(i-1)/2;
    while(j>=0){
        if(heap[j]<heap[i])
            break;
        swapNode(i,j);
        i=j;
        j=(i-1)/2;
    }
}

bool TimerHandler::siftDown(size_t index, size_t n){
    assert(index>=0 && index<heap.size());
    assert(n>=0&&n<=heap.size());
    size_t i=index;
    size_t j=i*2+1;
    while(j<n){
        if(j+1<n&&heap[j+1]<heap[j]) j++;
        if(heap[i]<heap[j])
            break;
        swapNode(i,j);
        i=j;
        j=i*2+1;
    }
    return i>index;
}

void TimerHandler::swapNode(size_t i, size_t j){
    assert(i>=0 && i < heap.size());
    assert(j>=0 && j < heap.size());
    std::swap(heap[i],heap[j]);
    ref[heap[i].id]=i;
    ref[heap[j].id]=j;
}


void TimerHandler::adjust(int id, int newExpires){
    assert(!heap.empty()&& ref.count(id)>0);
    heap[ref[id]].expires=Clock::now()+MilliSeconds(newExpires);
    siftDown(ref[id],heap.size());

}

void TimerHandler::add(int id, int timeOut, const TimeoutCallBackFunction& cb){
    assert(id>=0);
    size_t i;
    if(ref.count(id)==0){
        i=heap.size();
        ref[id]=i;
        heap.push_back({id,Clock::now()+MilliSeconds(timeOut),cb});
        siftUp(i);
    }else{
        i=ref[id];
        heap[i].expires=Clock::now()+MilliSeconds(timeOut);
        heap[i].callBackFunction=cb;
        if(!siftDown(i,heap.size())){
            siftUp(i);
        }
    }
}

void TimerHandler::doWork(int id){
    if(heap.empty()||ref.count(id)==0){
        return;
    }
    size_t i= ref[id];
    TimerNode node=heap[i];
    node.callBackFunction();
    deleteNode(i);
}

void TimerHandler::clear(){
    ref.clear();
    heap.clear();
}

void TimerHandler::tick(){
    if(heap.empty()){
        return;
    }
    while(!heap.empty()){
        TimerNode node=heap.front();
        if(std::chrono::duration_cast<MilliSeconds>(node.expires - Clock::now()).count() > 0) { 
            break; 
        }
        node.callBackFunction();
        pop();
    }
}

void TimerHandler::pop(){
    assert(!heap.empty());
    deleteNode(0);
}

int TimerHandler::getNextTick(){
    tick();
    size_t res=-1;
    if(!heap.empty()){
        res = std::chrono::duration_cast<MilliSeconds>(heap.front().expires - Clock::now()).count();
        if(res<0){
            res=0;
        }
    }
    return res;
}