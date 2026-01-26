//admin.h

#pragma once
#include "user.h"
#include <vector>
#include <iomanip>

class Admin : public User {
public:
    Admin(std::string name) : User(name, "Admin") {}

    void showMenu() override;

    void manageUsers();
    void viewRevenueReport();
    
    void addUser();
    void changeRole();
    void deleteUser();

};
