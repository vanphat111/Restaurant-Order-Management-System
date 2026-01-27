#include "waiter.h"
#include "database.h"
#include <iomanip>
#include <limits>

void Waiter::showMenu()
{
    int choice;
    do {
        std::cout << "\n==========================================";
        std::cout << "\n   WAITER DASHBOARD - waiter: " << this->userName;
        std::cout << "\n==========================================";
        std::cout << "\n1. Assign Table";
        std::cout << "\n2. Place Order";
        std::cout << "\n3. Update Order";
        std::cout << "\n4. Request Payment";
        std::cout << "\n0. Logout";
        std::cout << "\n------------------------------------------";
        std::cout << "\nEnter choice: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: assignTable(); break;
        case 2: placeOrder(); break;
        case 3: updateOrder(); break;
        case 4: requestPayment(); break;
        case 0: break;
        default: std::cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void Waiter::assignTable() {
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

        sql::PreparedStatement* pstmtOrder = con->prepareStatement(
            "INSERT INTO Orders (TableNumber,CreationTime,TotalAmount,CookingStatus)  VALUES ( ?, NOW(), 0, 'PENDING')"
        );
        pstmtOrder->setString(1, tableNumber);
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

void Waiter::updateOrder()
{
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();

        std::string orderID;
        std::cout << "Enter Order ID: ";
        std::cin >> orderID;

        int choice;
        do {
            std::cout << "==========================================\n";
            std::cout << "               UPDATE ORDER \n";
            std::cout << "==========================================\n";
            std::cout << "1. Add item\n";
            std::cout << "2. Update item \n";
            std::cout << "3. Remove item\n";
            std::cout << "0. Exit\n";
            std::cout << "Choose: ";
            std::cin >> choice;

            if (choice == 1) {
                std::string itemCode;
                int qty;
                double price;


                std::cout << "Item code: ";
                std::cin >> itemCode;
                std::cout << "Quantity: ";
                std::cin >> qty;


                sql::PreparedStatement* stmt =
                    con->prepareStatement(
                        "SELECT Price FROM MenuItem WHERE ItemCode = ?");
                stmt->setString(1, itemCode);
                sql::ResultSet* rs = stmt->executeQuery();


                if (!rs->next()) {
                    std::cout << "Item not found!\n";
                }
                else {
                    price = rs->getDouble("Price");


                    sql::PreparedStatement* insertStmt = con->prepareStatement("INSERT INTO OrderDetail VALUES (?, ?, ?, ?)");


                    insertStmt->setString(1, orderID);
                    insertStmt->setString(2, itemCode);
                    insertStmt->setInt(3, qty);
                    insertStmt->setDouble(4, price * qty);
                    insertStmt->execute();


                    delete insertStmt;
                    std::cout << "Item added successfully!\n";
                }
                delete rs;
                delete stmt;
            }
            else if (choice == 2) {
                std::string itemCode;
                int qty;
                double price;


                std::cout << "Item code: ";
                std::cin >> itemCode;
                std::cout << "New quantity: ";
                std::cin >> qty;


                sql::PreparedStatement* stmt =
                    con->prepareStatement(
                        "SELECT Price FROM MenuItem WHERE ItemCode = ?");
                stmt->setString(1, itemCode);
                sql::ResultSet* rs = stmt->executeQuery();


                if (rs->next()) {
                    price = rs->getDouble("Price");


                    sql::PreparedStatement* updateStmt =
                        con->prepareStatement(
                            "UPDATE OrderDetail SET Quantity = ?, SubTotal = ? WHERE OrderID = ? AND ItemCode = ?");


                    updateStmt->setInt(1, qty);
                    updateStmt->setDouble(2, price * qty);
                    updateStmt->setString(3, orderID);
                    updateStmt->setString(4, itemCode);
                    updateStmt->execute();


                    delete updateStmt;
                    std::cout << "Item updated!\n";
                }
                else {
                    std::cout << "Item not found!\n";
                }


                delete rs;
                delete stmt;
            }
            else if (choice == 3) {
                std::string itemCode;
                std::cout << "Item code: ";
                std::cin >> itemCode;


                sql::PreparedStatement* delStmt = con->prepareStatement("DELETE FROM OrderDetail WHERE OrderID = ? AND ItemCode = ?");
                delStmt->setString(1, orderID);
                delStmt->setString(2, itemCode);
                delStmt->execute();


                delete delStmt;
                std::cout << "Item removed!\n";
            }

        } while (choice != 0);
        
        sql::PreparedStatement* totalStmt = con->prepareStatement("UPDATE Orders SET TotalAmount = (SELECT IFNULL(SUM(SubTotal),0) FROM OrderDetail WHERE OrderID = ?) WHERE OrderID = ?");
        totalStmt->setString(1, orderID);
        totalStmt->setString(2, orderID);
        totalStmt->execute();


        delete totalStmt;
        std::cout << "Order updated successfully!\n";
        

    }
    catch (sql::SQLException& e) {
        std::cerr << e.what();
    }
}

void Waiter::requestPayment()
{
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();

        std::string orderID;
        std::cout << "Enter Order ID to pay: ";
        std::cin >> orderID;

        sql::PreparedStatement* checkStmt =
            con->prepareStatement(
                "SELECT TableNumber FROM Orders WHERE OrderID = ?");
        checkStmt->setString(1, orderID);
        sql::ResultSet* rs = checkStmt->executeQuery();


        if (!rs->next()) {
            std::cout << "Order not found!\n";
            delete rs;
            delete checkStmt;
            return;
        }

        std::string tableNumber = rs->getString("TableNumber");
        delete rs;
        delete checkStmt;


        sql::PreparedStatement* totalStmt =
            con->prepareStatement(
                "SELECT IFNULL(SUM(SubTotal),0) AS Total "
                "FROM OrderDetail WHERE OrderID = ?");
        totalStmt->setString(1, orderID);
        rs = totalStmt->executeQuery();
        rs->next();
        double totalAmount = rs->getDouble("Total");


        delete rs;
        delete totalStmt;

        std::cout << "Total amount: " << totalAmount << " VND\n";

        char confirm;
        std::cout << "Confirm payment? (y/n): ";
        std::cin >> confirm;


        if (confirm != 'y' && confirm != 'Y') {
            std::cout << "Payment cancelled.\n";
            return;
        }


        sql::PreparedStatement* payStmt =
            con->prepareStatement(
                "UPDATE Orders "
                "SET TotalAmount = ? "
                "WHERE OrderID = ?");
        payStmt->setDouble(1, totalAmount);
        payStmt->setString(2, orderID);
        payStmt->execute();
        delete payStmt;


        sql::PreparedStatement* tableStmt =
            con->prepareStatement(
                "UPDATE Tables SET Status = 'available' "
                "WHERE TableNumber = ?");
        tableStmt->setString(1, tableNumber);
        tableStmt->execute();
        delete tableStmt;

        std::cout << "Payment successful! Table is now available.\n";

    }
    catch (sql::SQLException& e) {
        std::cerr << e.what();
    }
}
