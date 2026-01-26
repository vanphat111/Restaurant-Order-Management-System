#pragma once
#include "user.h"

class Waiter : public User {
public:
    Waiter(std::string name) : User(name, "Wai") {}

    void showMenu() override;
    void AssignTable();
    void placeOrder();
    void updateOrder();
    void RequestPayment();
};