#ifndef EPOLL_MANAGER
#define EPOLL_MANAGER

#include<iostream>
#include<sys/epoll.h>
#include<fcntl.h>
#include<vector>
#include<errno.h>
#include<unistd.h>
#include<cassert>
#define MAXEVENT 1024

class EpollManager{
private:
    int epollFd;
    std::vector<struct epoll_event> events;   
public:
    EpollManager(int maxEvent=MAXEVENT);
    ~EpollManager();

    bool addFd(int fd, uint32_t events);

    bool modifyFd(int fd, uint32_t events);

    bool deleteFd(int fd);

    int wait(int timeout=-1);

    int getEventFd(size_t i) const;

    uint32_t getEvents(size_t i) const;

};

#endif 