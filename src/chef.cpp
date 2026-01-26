#include "chef.h"
#include <iomanip>
#include <limits>

void Chef::showMenu() {
    int choice;
    do {
        std::cout << "\n==========================================";
        std::cout << "\n   KITCHEN SYSTEM - Chef: " << this->userName;
        std::cout << "\n==========================================";
        std::cout << "\n1. Start Working (Press Enter to Complete)";
        std::cout << "\n0. Logout";
        std::cout << "\n------------------------------------------";
        std::cout << "\nEnter choice: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) viewPendingOrders();
    } while (choice != 0);
}

void Chef::viewPendingOrders() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        
        while (true) {
            sql::Statement* stmt = con->createStatement();
            
            sql::ResultSet* res = stmt->executeQuery(
                "SELECT OrderID, TableNumber, CreationTime "
                "FROM Orders "
                "WHERE CookingStatus != 'READY' "
                "ORDER BY CreationTime ASC LIMIT 5"
            );

            std::cout << "\033[2J\033[1;1H";
            std::cout << "==========================================\n";
            std::cout << "       KITCHEN QUEUE - PENDING ORDERS     \n";
            std::cout << "==========================================\n";
            std::cout << std::left << std::setw(12) << "Order ID" 
                      << std::setw(10) << "Table" 
                      << "Order Time\n";
            std::cout << "------------------------------------------\n";

            std::string topOrderID = "";
            std::string topTable = "";

            int count = 0;
            while (res->next()) {
                std::string currentID = res->getString("OrderID");
                std::string currentTable = res->getString("TableNumber");
                std::string currentTime = res->getString("CreationTime");

                if (count == 0) {
                    topOrderID = currentID;
                    topTable = currentTable;
                    std::cout << ">>> " << std::left << std::setw(9) << currentID << std::setw(10) << currentTable << currentTime <<"\n";
                } else {
                    std::cout << "    " << std::left << std::setw(9) << currentID << std::setw(10) << currentTable << currentTime << "\n";
                }
                count++;
            }

            if (count == 0) {
                std::cout << "\n[ALL CLEAR] No pending orders. Press Enter to back.";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                delete res; delete stmt;
                break;
            }

            std::cout << "\n------------------------------------------";
            std::cout << "\n[ENTER] Mark Order " << topOrderID << " as READY";
            std::cout << "\n[q + ENTER] Back to Menu";
            std::cout << "\nChoice: ";

            delete res; delete stmt;

            std::string input;
            std::getline(std::cin, input); 

            if (input == "q" || input == "Q") break;

            
            updateCookingStatus(0, topOrderID, "READY");
        }
    } catch (sql::SQLException &e) {
        std::cerr << "DB Error: " << e.what() << std::endl;
    }
}

void Chef::updateCookingStatus(int dummy, std::string orderID, std::string status) {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE Orders SET CookingStatus = ? WHERE OrderID = ?"
        );
        pstmt->setString(1, status);
        pstmt->setString(2, orderID);
        pstmt->executeUpdate();
        delete pstmt;
    } catch (sql::SQLException &e) {
        std::cerr << "Update Error: " << e.what() << std::endl;
    }
}
