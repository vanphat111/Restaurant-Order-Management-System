// user.h

#pragma once

#include <string>
#include "colors.h"
#include <iostream>
#include <openssl/sha.h>
#include <sstream> 
#include <iomanip>
#include "database.h"

class User {
protected:
    std::string userName;
    std::string password;
    std::string role;

public:
    User(std::string name, std::string r) : userName(name), role(r) {}
    virtual ~User() {}

    virtual void showMenu() = 0;
    static bool recoverPassword(std::string _userName, std::string _newPass);
    static User* login(std::string _userName, std::string _password);
    static std::string hashPassword(const std::string& plainText);
};

