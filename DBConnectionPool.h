#ifndef DB_CONNECTION_POOL
#define DB_CONNECTION_POOL

#include<mysql/mysql.h>
#include<string>
#include<queue>
#include<mutex>
#include<memory>
#include<thread>
#include<cassert>
#include<condition_variable>
class DBConnectionPool{
public:
    static DBConnectionPool* getInstance();

    void freeConnection(MYSQL* connection);

    MYSQL* getConnection();

    void Init(const char* host, int port,
              const char* user, const char* pwd,
              const char* dbName, int connectionSize);

    void closePool();

    int getFreeConnectionNumber();

    DBConnectionPool(const DBConnectionPool& other) =delete;
    DBConnectionPool& operator=(const DBConnectionPool& other) =delete;
    DBConnectionPool(DBConnectionPool&& other) = delete;
    DBConnectionPool& operator=(DBConnectionPool&& other) = delete;

private:
    DBConnectionPool();
    ~DBConnectionPool();
    int maxConnection;
    int useNumber;
    int freeNumber;
    std::queue<MYSQL*> connectionQueue;
    std::mutex mutex;
    std::condition_variable condition;
};



#endif 