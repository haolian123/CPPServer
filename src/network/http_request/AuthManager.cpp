#include "AuthManager.h"

// 初始化静态成员变量，用于Redis操作
RedisManager* AuthManager::redisManager = RedisManager::getInstance();

// 从Redis缓存中验证用户密码
bool AuthManager::verifyFromCache(const std::string& name, std::string& correctPassword) {
    // 执行Redis命令，尝试获取密码
    redisReply* reply = redisManager->executeCommand("GET " + name);
    
    bool passwordInCache = false;
    if (reply->type == REDIS_REPLY_STRING) {
        correctPassword = reply->str;
        passwordInCache = true;
    }
    freeReplyObject(reply);
    return passwordInCache;
}

// 从数据库中验证用户或注册新用户
bool AuthManager::verifyOrRegisterFromDB(MYSQL*& dbConnection, bool isLogin, const std::string& name, const std::string& passWord) {
    char sqlQuery[256] = {0};
    MYSQL_RES* res = nullptr;
    bool operationSuccess = false;

    // 准备SQL查询语句
    std::snprintf(sqlQuery, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());

    // 执行SQL查询
    if (mysql_query(dbConnection, sqlQuery)) {
        return false;
    }
    res = mysql_store_result(dbConnection);
    std::string correctPassword = "";
    while (MYSQL_ROW row = mysql_fetch_row(res)) {
        correctPassword = row[1];
        if (isLogin) {
            // 如果是登录操作，检查密码是否匹配
            if (passWord == correctPassword) {
                operationSuccess = true;
                // 密码验证成功，更新Redis缓存
                updateCache(name, correctPassword);
            } else {
                operationSuccess = false;
            }
        }
    }
    mysql_free_result(res);
    // 如果是注册操作，并且用户名未被使用，则插入新用户到数据库
    if (!isLogin && !operationSuccess) {
        operationSuccess = insertUserIntoDB(dbConnection, name, passWord);
    }
    return operationSuccess;
}

// 向数据库插入新用户
bool AuthManager::insertUserIntoDB(MYSQL*& dbConnection, const std::string& name, const std::string& passWord) {
    char sqlQuery[256];
    bool operationSuccess = false;
    std::snprintf(sqlQuery, 256, "INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), passWord.c_str());
    if (!mysql_query(dbConnection, sqlQuery)) {
        // 插入数据库成功，同时更新Redis缓存
        operationSuccess = updateCache(name, passWord);
    } else {
        operationSuccess = false;
    }
    return operationSuccess;
}

// 更新Redis缓存
bool AuthManager::updateCache(const std::string& name, const std::string& passWord) {
    char redisCommand[256];
    std::snprintf(redisCommand, 256, "SET %s %s", name.c_str(), passWord.c_str());
    redisReply* reply = redisManager->executeCommand(redisCommand);
    freeReplyObject(reply);
    return true;
}

// 用户验证主流程
bool AuthManager::userVerify(const std::string& name, const std::string& passWord, bool isLogin) {
    if (name.empty() || passWord.empty()) {
        return false;
    }

    std::string correctPassword;
    bool passwordInCache = verifyFromCache(name, correctPassword);

    bool operationSuccess = false;

    if (passwordInCache) {
        // 如果密码在缓存中，直接验证
        operationSuccess = (isLogin && passWord == correctPassword);
    } else {
        // 密码不在缓存中，连接数据库进行验证
        MYSQL* dbConnection;
        DBConnectionRAII(dbConnection, DBConnectionPool::getInstance());
        assert(dbConnection);
        operationSuccess = verifyOrRegisterFromDB(dbConnection, isLogin, name, passWord);

        // 如果是注册操作，更新数据库和缓存
        if (!isLogin && operationSuccess) {
            operationSuccess = insertUserIntoDB(dbConnection, name, passWord);
        }
        // 释放数据库连接
        DBConnectionPool::getInstance()->freeConnection(dbConnection);
    }

    return operationSuccess;
}
