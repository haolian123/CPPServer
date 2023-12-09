#ifndef DB_CONNECTION_RAII
#define DB_CONNECTION_RAII 
#include"DBConnectionPool.h"

class DBConnectionRAII{
public:
    DBConnectionRAII(MYSQL*&sql,DBConnectionPool*pool){
        assert(pool);
        sql=pool->getConnection();
        this->sql=sql;
        connectionPool=pool;
    }
    ~DBConnectionRAII(){
        if(sql){
            connectionPool->freeConnection(sql);
        }
    }
private:
    MYSQL* sql;
    DBConnectionPool* connectionPool;
};


#endif