// Order.h

#include <vector>
#include <string>
#include "Orderdei.h"

class Order {
public:
    int orderID;
    int tableNumber;
    std::string creationTime;
    double totalAmount;
    std::string cookingStatus;
    std::vector<Orderdei> items;
};
