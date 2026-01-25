#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

#include <string>
#include <functional>

class PasswordHasher {
public:
    static std::string hash(const std::string& password) {
        std::hash<std::string> hasher;
        return std::to_string(hasher(password));
    }
};

#endif
