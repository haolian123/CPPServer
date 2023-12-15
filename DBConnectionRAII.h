#ifndef DB_CONNECTION_RAII
#define DB_CONNECTION_RAII 
#include"DBConnectionPool.h"

class DBConnectionRAII {
public:
    // 构造函数，获取数据库连接
    DBConnectionRAII(MYSQL*& sql, DBConnectionPool* pool) {
        assert(pool);
        sql = pool->getConnection();
        this->sql = sql;
        connectionPool = pool;
    }

    // 析构函数，释放数据库连接
    ~DBConnectionRAII() {
        if (sql) {
            connectionPool->freeConnection(sql);
        }
    }

private:
    MYSQL* sql;                    // 数据库连接
    DBConnectionPool* connectionPool; // 连接池指针
};


#endif