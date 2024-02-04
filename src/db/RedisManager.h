#include <hiredis/hiredis.h>
#include <iostream>
#include <string>

class RedisManager {
private:
    redisContext *c;

    RedisManager(const std::string &host, int port, const std::string &password, int dbIndex) {
        c = redisConnect(host.c_str(), port);
        if (c == NULL || c->err) {
            if (c) {
                std::cerr << "Error: " << c->errstr << std::endl;
            } else {
                std::cerr << "Can't allocate redis context" << std::endl;
            }
        } else {
            if (!password.empty()) {
                auto *reply = (redisReply *)redisCommand(c, "AUTH %s", password.c_str());
                if (reply == NULL || c->err || reply->type == REDIS_REPLY_ERROR) {
                    std::cerr << "Redis AUTH failed" << std::endl;
                }
                freeReplyObject(reply);
            }

            auto *reply = (redisReply *)redisCommand(c, "SELECT %d", dbIndex);
            if (reply == NULL || c->err) {
                std::cerr << "Selecting database failed" << std::endl;
            }
            freeReplyObject(reply);
        }
    }

public:
    static RedisManager* getInstance(const std::string &host = "127.0.0.1", int port = 6379,
                                     const std::string &password = "123456", int dbIndex = 12) {

        static RedisManager instance(host, port, password, dbIndex);
        return &instance;
    }

    redisReply* executeCommand(const std::string &command) {
        return (redisReply*)redisCommand(c, command.c_str());
    }

    ~RedisManager() {
        redisFree(c);
    }
};
