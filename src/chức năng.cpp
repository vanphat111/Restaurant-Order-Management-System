#include <iostream>
using namespace std;

int main() {
    int choice;
    bool tableOpen = false;
    int total = 0;

    do {
        cout << "\n=== WAITER MENU ===\n";
        cout << "1. Open Table\n";
        cout << "2. Place Order\n";
        cout << "3. Cancel Order\n";
        cout << "4. Payment\n";
        cout << "0. Exit\n";
        cout << "Choose: ";
        cin >> choice;

        if (choice == 1) {
            tableOpen = true;
            cout << "Table OPENED\n";
        }

        else if (choice == 2) {
            if (!tableOpen) {
                cout << "Table not open!\n";
            } else {
                int price, qty;
                cout << "Enter item price: ";
                cin >> price;
                cout << "Quantity: ";
                cin >> qty;

                if (qty > 0) {
                    total += price * qty;
                    cout << "Added to order\n";
                } else {
                    cout << "Invalid quantity\n";
                }
            }
        }

        else if (choice == 3) {
            total = 0;
            tableOpen = false;
            cout << "Order CANCELED\n";
        }

        else if (choice == 4) {
            cout << "TOTAL: " << total << " VND\n";
            total = 0;
            tableOpen = false;
        }

    } while (choice != 0);

    return 0;
}
