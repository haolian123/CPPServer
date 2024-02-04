#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <string>

class AuthManager {
public:
    AuthManager()=default;
    ~AuthManager()=default;

    static bool userVerify(const std::string& name, const std::string& pwd, bool isLogin);
};

#endif // AUTH_MANAGER_H
