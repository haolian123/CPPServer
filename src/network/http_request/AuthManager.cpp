#include"AuthManager.h"
#include <mysql/mysql.h>  //mysql
#include <cassert>
#include "../../db/DBConnectionPool.h"
#include "../../db/DBConnectionRAII.h"
#include <string.h>
#include <hiredis/hiredis.h>
#include <iostream>

bool AuthManager::userVerify(const std::string& name, const std::string& passWord, bool isLogin){
    if(name.empty() || passWord.empty()){
        return false;
    }
    
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            std::cerr << "Error: " << c->errstr << std::endl;
            // handle error
        } else {
            std::cerr << "Can't allocate redis context" << std::endl;
            // handle error
        }
    } else {
        redisReply *reply = (redisReply *)redisCommand(c, "AUTH %s", "123456");
        reply = (redisReply *)redisCommand(c, "SELECT %d", 12);
        if (reply == NULL || c->err) {
            std::cerr << "Selecting database failed" << std::endl;
            // handle error
        }
        if (reply) freeReplyObject(reply);
    }

    // 尝试从Redis缓存中获取密码
    redisReply *reply = (redisReply *)redisCommand(c, "GET %s", name.c_str());
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
                    redisCommand(c, "SET %s %s", name.c_str(), correctPassword.c_str());
                } else {
                    flag = false;
                }
            } else {
                flag = false;
            }
        }
        std::cout<<"缓存！"<<std::endl;
        mysql_free_result(res);

        if (!isLogin && flag) {
            bzero(order, 256);
            std::snprintf(order, 256, "INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), passWord.c_str());
            if (!mysql_query(sql, order)) {
                // 插入数据库成功，同时更新缓存
                redisCommand(c, "SET %s %s", name.c_str(), passWord.c_str());
                flag = true;
            } else {
                flag = false;
            }
        }
        DBConnectionPool::getInstance()->freeConnection(sql);
    }
    
    // 关闭Redis连接
    redisFree(c);

    return flag;
}












// bool AuthManager::userVerify(const std::string& name, const std::string& passWord, bool isLogin){
//     if(name == ""|| passWord== ""){
//         return false;
//     }
//     MYSQL *sql;
//     DBConnectionRAII(sql,DBConnectionPool::getInstance());
//     assert(sql);
//     bool flag=false;
//     unsigned int j = 0;
//     char order[256] = {0};
//     MYSQL_FIELD * fields= nullptr;
//     MYSQL_RES* res= nullptr;

//     if(!isLogin){
//         flag = true ;
//     }

//     std::snprintf(order,256,"SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());

//     if(mysql_query(sql,order)){
//         mysql_free_result(res);
//         return false;
//     }
//     res=mysql_store_result(sql);
//     j = mysql_num_fields(res);
//     fields = mysql_fetch_field(res);
    
//     while(MYSQL_ROW row = mysql_fetch_row(res)){
//         std::string correctPassword(row[1]);
//         if(isLogin){
//             if(passWord==correctPassword){
//                 flag=true;
//             }else{
//                 flag=false;
//             }
//         }else{
//             flag=false;
//         }
//     }
//     mysql_free_result(res);

//     if(!isLogin && flag == true){
//         bzero(order,256);
//         std::snprintf(order,256,"INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), passWord.c_str());
//         if(mysql_query(sql,order)){
//             flag=false;
//         }
//         flag=true;
//     }
//     DBConnectionPool::getInstance()->freeConnection(sql);
//     return flag;
// }