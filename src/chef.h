//chef.h

#pragma once
#include "user.h"

class Chef : public User {
public:
    Chef(std::string name) : User(name, "Chef") {}
    
    void showMenu() override;
    void viewPendingOrders();
    void updateCookingStatus(int orderID, std::string status);
};
