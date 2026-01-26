//admin.h

#pragma once
#include "user.h"
#include <vector>
#include <iomanip>

class Admin : public User {
public:
    Admin(std::string name) : User(name, "Admin") {}

    void showMenu() override;

    void viewSaleReport();
    void generateDailyReport();
    void getBestSellers();
    
    void manageUsers();
    void addUser();
    void changeRole();
    void deleteUser();

    void manageMenu();
    void addMenuItem();
    void updateMenuItem();
    void deleteMenuItem();
    void displayAllItems();

};
