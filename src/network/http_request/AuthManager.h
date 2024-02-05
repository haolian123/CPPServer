#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H
#include <mysql/mysql.h>  //mysql
#include <cassert>
#include "../../db/DBConnectionPool.h"
#include "../../db/DBConnectionRAII.h"
#include <string.h>
#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
#include "../../db/RedisManager.h"
class AuthManager {
    static RedisManager* redisManager;

    static bool verifyFromCache(const std::string& name, std::string& correctPassword);
    static bool verifyOrRegisterFromDB(MYSQL*& dbConnection, bool isLogin, const std::string& name, const std::string& passWord);
    static bool insertUserIntoDB(MYSQL*& dbConnection, const std::string& name, const std::string& passWord);
    static bool updateCache(const std::string& name, const std::string& passWord);

public:
    AuthManager()=default;
    ~AuthManager()=default;

    static bool userVerify(const std::string& name, const std::string& pwd, bool isLogin);
};

#endif 
