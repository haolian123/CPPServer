#ifndef DB_CONNECTION_POOL
#define DB_CONNECTION_POOL

#include<mysql/mysql.h>
#include<string>
#include<queue>
#include<mutex>
#include<semaphore.h>
#include<memory>
#include<thread>
#include<cassert>
class DBConnectionPool{
public:
    static DBConnectionPool* getInstance();
    
    void freeConnection(MYSQL* connection);

    MYSQL* getConnection();

    void Init(const char* host, int port,
            const char* user,const char* pwd, 
            const char* dbName,int connectionSize
            );

    void closePool();

    int getFreeConnectionNumber();
private:
    DBConnectionPool();
    ~DBConnectionPool();
    int maxConnection;
    int useNumber;
    int freeNumber;

    std::queue<MYSQL *>connectionQueue;
    std::mutex mutex;
    sem_t semId;
};



#endif