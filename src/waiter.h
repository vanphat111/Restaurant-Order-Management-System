#pragma once
#include "user.h"

class Waiter : public User {
public:
    Waiter(std::string name) : User(name, "Waiter") {}

    void showMenu() override;
    void displayMenu();
    void AssignTable();
    void placeOrder();
    void updateOrder();
    void RequestPayment();
};
