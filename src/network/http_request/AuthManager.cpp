#include"AuthManager.h"
#include <mysql/mysql.h>  //mysql
#include <cassert>
#include "../../db/DBConnectionPool.h"
#include "../../db/DBConnectionRAII.h"
#include <string.h>
AuthManager::AuthManager(){

}
AuthManager::~AuthManager(){

}

bool AuthManager::userVerify(const std::string& name, const std::string& passWord, bool isLogin){
    if(name == ""|| passWord== ""){
        return false;
    }
    MYSQL *sql;
    DBConnectionRAII(sql,DBConnectionPool::getInstance());
    assert(sql);
    bool flag=false;
    unsigned int j = 0;
    char order[256] = {0};
    MYSQL_FIELD * fields= nullptr;
    MYSQL_RES* res= nullptr;

    if(!isLogin){
        flag = true ;
    }

    std::snprintf(order,256,"SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());

    if(mysql_query(sql,order)){
        mysql_free_result(res);
        return false;
    }
    res=mysql_store_result(sql);
    j = mysql_num_fields(res);
    fields = mysql_fetch_field(res);
    
    while(MYSQL_ROW row = mysql_fetch_row(res)){
        std::string correctPassword(row[1]);
        if(isLogin){
            if(passWord==correctPassword){
                flag=true;
            }else{
                flag=false;
            }
        }else{
            flag=false;
        }
    }
    mysql_free_result(res);

    if(!isLogin && flag == true){
        bzero(order,256);
        std::snprintf(order,256,"INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), passWord.c_str());
        if(mysql_query(sql,order)){
            flag=false;
        }
        flag=true;
    }
    DBConnectionPool::getInstance()->freeConnection(sql);
    return flag;
}