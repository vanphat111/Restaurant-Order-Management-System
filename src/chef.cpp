// chef.cpp

#include "chef.h"
#include "colors.h"
#include <iomanip>
#include <limits>
#include <iostream>

void Chef::showMenu() {
    int choice;
    do {
        // Xóa màn hình cho sạch
        std::cout << CLEAR_SCREEN;

        // Header
        std::cout << COLOR_TITLE << "==========================================" << RESET;
        std::cout << "\n   " << BOLD << "CHEF DASHBOARD" << RESET << " - User: " << COLOR_HIGHLIGHT << this->userName << RESET;
        std::cout << COLOR_TITLE << "\n==========================================" << RESET;
        
        // Menu
        std::cout << "\n" << COLOR_HIGHLIGHT << "1." << RESET << " Start Working (Kitchen Queue)";
        std::cout << "\n" << COLOR_ERROR     << "0. Logout" << RESET;
        
        std::cout << COLOR_TITLE << "\n------------------------------------------" << RESET;
        std::cout << "\nEnter choice: ";
            
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

            // Vẽ giao diện Queue
            std::cout << CLEAR_SCREEN;
            std::cout << COLOR_TITLE << "==============================================" << RESET;
            std::cout << "\n        " << BOLD << "KITCHEN QUEUE - DETAILED VIEW" << RESET << "         ";
            std::cout << COLOR_TITLE << "\n==============================================" << RESET << "\n";
            
            // Header bảng
            std::cout << COLOR_SUBTITLE << std::left << std::setw(12) << "Order ID" 
                      << std::setw(10) << "Table" 
                      << "Order Time" << RESET << "\n";
            std::cout << GRAY << "----------------------------------------------" << RESET << "\n";

            int topOrderID = 0;
            int count = 0;

            while (res->next()) {
                int currentID = res->getInt("OrderID");
                std::string currentTable = res->getString("TableNumber");
                std::string currentTime = res->getString("CreationTime");

                // Đơn đầu tiên (quan trọng nhất) in màu nổi
                if (count == 0) {
                    topOrderID = currentID;
                    std::cout << COLOR_HIGHLIGHT << ">>> " << std::left << std::setw(8) << currentID 
                              << std::setw(10) << currentTable << currentTime << RESET << "\n";
                } else {
                    // Các đơn sau in màu thường
                    std::cout << "    " << std::left << std::setw(8) << currentID 
                              << std::setw(10) << currentTable << currentTime << "\n";
                }

                sql::PreparedStatement* pstmtItems = con->prepareStatement(
                    "SELECT m.ItemName, od.Quantity "
                    "FROM OrderDetail od JOIN MenuItem m ON od.ItemCode = m.ItemCode "
                    "WHERE od.OrderID = ?"
                );
                pstmtItems->setInt(1, currentID);
                sql::ResultSet* resItems = pstmtItems->executeQuery();

                while (resItems->next()) {
                    // In món ăn lùi vào trong một chút
                    std::cout << GRAY << "       - " << RESET << std::left << std::setw(20) << resItems->getString("ItemName") 
                              << " x" << COLOR_HIGHLIGHT << resItems->getInt("Quantity") << RESET << "\n";
                }
                std::cout << GRAY << "----------------------------------------------" << RESET << "\n";

                delete resItems; delete pstmtItems;
                count++;
            }

            if (count == 0) {
                std::cout << "\n" << COLOR_SUCCESS << "[ALL CLEAR] No pending orders." << RESET;
                std::cout << "\nPress Enter to back.";
                std::cin.get();
                delete res; delete stmt;
                break;
            }

            // Footer hướng dẫn
            std::cout << "\n[ENTER] Mark Order " << COLOR_HIGHLIGHT << topOrderID << RESET << " as " << COLOR_SUCCESS << "READY" << RESET;
            std::cout << "\n[q + ENTER] Back to Menu";
            std::cout << "\nChoice: ";

            delete res; delete stmt;

            std::string input;
            std::getline(std::cin, input); 

            if (input == "q" || input == "Q") break;
            
            updateCookingStatus(topOrderID, "READY");
        }
    } catch (sql::SQLException &e) {
        std::cerr << COLOR_ERROR << "DB Error: " << e.what() << RESET << std::endl;
        std::cin.get();
    }
}

void Chef::updateCookingStatus(int orderID, std::string status) {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE Orders SET CookingStatus = ? WHERE OrderID = ?"
        );
        pstmt->setString(1, status);
        pstmt->setInt(2, orderID);
        pstmt->executeUpdate();
        delete pstmt;
        // Không cần in thông báo success ở đây để loop nó tự vẽ lại cho mượt
    } catch (sql::SQLException &e) {
        std::cerr << COLOR_ERROR << "Update Error: " << e.what() << RESET << std::endl;
        std::cin.get();
    }
}