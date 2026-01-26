#include "waiter.h"
#include "database.h"
#include <iomanip>
#include <limits>

void Waiter::AssignTable() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT TableNumber, Section, Status "
            "FROM Tables "
            "ORDER BY TableNumber ASC"
        );

        std::cout << "\033[2J\033[1;1H";
        std::cout << "==========================================\n";
        std::cout << "              LIST OF TABLES              \n";
        std::cout << "==========================================\n";
        std::cout << std::left << std::setw(15) << "Table"
            << std::setw(10) << "Section"
            << "Status\n";
        std::cout << "------------------------------------------\n";

        while (res->next()) {
            std::cout << std::left << std::setw(15)
                << res->getString("TableNumber")
                << std::setw(10)
                << res->getString("Section")
                << res->getString("Status") << "\n";
        }

        std::string chooseTableNumber, chooseStatus;
        std::cout << "\nChoose table number: ";
        std::cin >> chooseTableNumber;

        std::cout << "Choose status (Available / Occupied): ";
        std::cin >> chooseStatus;

        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE Tables SET Status = ? WHERE TableNumber = ?"
        );

        pstmt->setString(1, chooseStatus);
        pstmt->setString(2, chooseTableNumber);
        pstmt->executeUpdate();

        std::cout << "\n[SUCCESS] Table updated!\n";

        delete pstmt;
        delete res;
        delete stmt;

    }
    catch (sql::SQLException& e) {
        std::cerr << "DB Error: " << e.what() << std::endl;
    }
}

void Waiter::placeOrder() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();

        // 1. Show menu
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT ItemCode, ItemName, Price, InventoryQty, Category "
            "FROM MenuItem ORDER BY Category, ItemName"
        );

        while (res->next()) {
            std::cout << res->getString("ItemCode")
                << res->getString("ItemName")
                << res->getDouble("Price")
                << res->getInt("InventoryQty") << "\n";
        }
        delete res; 
        delete stmt;

        std::string tableNumber;
        std::cout << "Table number: ";
        std::cin >> tableNumber;

        std::string orderID = "O" + std::to_string(time(nullptr));
        sql::PreparedStatement* pstmtOrder = con->prepareStatement(
            "INSERT INTO Orders VALUES (?, ?, NOW(), 0, 'PENDING')"
        );
        pstmtOrder->setString(1, orderID);
        pstmtOrder->setString(2, tableNumber);
        pstmtOrder->executeUpdate();
        delete pstmtOrder;

        double totalAmount = 0;

        while (true) {
            std::string itemCode;
            int qty;

            std::cout << "Item code (0 to stop): ";
            std::cin >> itemCode;
            if (itemCode == "0") break;

            std::cout << "Quantity: ";
            std::cin >> qty;

            sql::PreparedStatement* pstmtPrice =
                con->prepareStatement("SELECT Price FROM MenuItem WHERE ItemCode=?");
            pstmtPrice->setString(1, itemCode);
            res = pstmtPrice->executeQuery();

            if (!res->next()) {
                std::cout << "Item not found!\n";
                delete res;
                delete pstmtPrice;
                continue;
            }

            double price = res->getDouble("Price");
            double sub = price * qty;
            totalAmount += sub;

            delete res; 
            delete pstmtPrice;

            sql::PreparedStatement* pstmtDetail = con->prepareStatement(
                "INSERT INTO OrderDetail VALUES (?, ?, ?, ?)"
            );
            pstmtDetail->setString(1, orderID);
            pstmtDetail->setString(2, itemCode);
            pstmtDetail->setInt(3, qty);
            pstmtDetail->setDouble(4, sub);
            pstmtDetail->executeUpdate();
            delete pstmtDetail;

            sql::PreparedStatement* pstmtStock = con->prepareStatement(
                "UPDATE MenuItem SET InventoryQty = InventoryQty - ? WHERE ItemCode = ?"
            );
            pstmtStock->setInt(1, qty);
            pstmtStock->setString(2, itemCode);
            pstmtStock->executeUpdate();
            delete pstmtStock;
        }

        sql::PreparedStatement* pstmtTotal = con->prepareStatement(
            "UPDATE Orders SET TotalAmount=? WHERE OrderID=?"
        );
        pstmtTotal->setDouble(1, totalAmount);
        pstmtTotal->setString(2, orderID);
        pstmtTotal->executeUpdate();
        delete pstmtTotal;

        std::cout << "[SUCCESS] Order placed!\n";

    }
    catch (sql::SQLException& e) {
        std::cerr << e.what();
    }
}



