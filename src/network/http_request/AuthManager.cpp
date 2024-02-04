#include"AuthManager.h"


bool AuthManager::userVerify(const std::string& name, const std::string& passWord, bool isLogin){
    if(name.empty() || passWord.empty()){
        return false;
    }
    
     // 获取 RedisManager 实例
    RedisManager* redisManager = RedisManager::getInstance();

    // 尝试从Redis缓存中获取密码
    redisReply *reply = redisManager->executeCommand("GET " + name);
    std::string correctPassword;
    bool isCached = false;
    if(reply->type == REDIS_REPLY_STRING) {
        correctPassword = reply->str;
        isCached = true;
    }
    freeReplyObject(reply);

    bool flag = false;
    if (isCached) {
        flag = (isLogin && passWord == correctPassword);
    } else {
        MYSQL *sql;
        DBConnectionRAII(sql, DBConnectionPool::getInstance());
        assert(sql);

        char order[256] = {0};
        MYSQL_RES* res = nullptr;

        if (!isLogin) {
            flag = true;
        }

        std::snprintf(order, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());

        if (mysql_query(sql, order)) {
            mysql_free_result(res);
            return false;
        }
        res = mysql_store_result(sql);
        
        while (MYSQL_ROW row = mysql_fetch_row(res)) {
            correctPassword = row[1];
            if (isLogin) {
                if (passWord == correctPassword) {
                    flag = true;
                    // 用户密码验证成功，将其添加到缓存中
                    char redisCommand[256];
                    std::snprintf(redisCommand, 256,  "SET %s %s", name.c_str(), correctPassword.c_str());
                    reply=redisManager->executeCommand(redisCommand);
                    freeReplyObject(reply);
                } else {
                    flag = false;
                }
            } else {
                flag = false;
            }
        }
        mysql_free_result(res);

        if (!isLogin && flag) {
            bzero(order, 256);
            std::snprintf(order, 256, "INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), passWord.c_str());
            if (!mysql_query(sql, order)) {
                // 插入数据库成功，同时更新缓存
                char redisCommand[256];
                std::snprintf(redisCommand, 256,  "SET %s %s", name.c_str(), passWord.c_str());
                reply=redisManager->executeCommand(redisCommand);
                freeReplyObject(reply);
                flag = true;
            } else {
                flag = false;
            }
        }
        DBConnectionPool::getInstance()->freeConnection(sql);
    }
    

    return flag;
}