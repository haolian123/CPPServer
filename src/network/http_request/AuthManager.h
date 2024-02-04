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
public:
    AuthManager()=default;
    ~AuthManager()=default;

    static bool userVerify(const std::string& name, const std::string& pwd, bool isLogin);
};

#endif 
