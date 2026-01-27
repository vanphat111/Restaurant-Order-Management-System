// waiter.cpp

#include "waiter.h"
#include "database.h"
#include "colors.h"
#include <iomanip>
#include <limits>
#include <iostream>

void Waiter::showMenu()
{
    int choice;
    do {
        std::cout << CLEAR_SCREEN;
        std::cout << COLOR_TITLE << "==========================================" << RESET;
        std::cout << "\n   " << BOLD << "WAITER DASHBOARD" << RESET << " - User: " << COLOR_HIGHLIGHT << this->userName << RESET;
        std::cout << COLOR_TITLE << "\n==========================================" << RESET;
        
        std::cout << "\n" << COLOR_HIGHLIGHT << "1." << RESET << " Assign Table (Check Status)";
        std::cout << "\n" << COLOR_HIGHLIGHT << "2." << RESET << " Place Order";
        std::cout << "\n" << COLOR_HIGHLIGHT << "3." << RESET << " Update Order";
        std::cout << "\n" << COLOR_HIGHLIGHT << "4." << RESET << " Request Payment";
        std::cout << "\n" << COLOR_ERROR     << "0. Logout" << RESET;
        
        std::cout << COLOR_TITLE << "\n------------------------------------------" << RESET;
        std::cout << "\nEnter choice: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: AssignTable(); break;
        case 2: placeOrder(); break;
        case 3: updateOrder(); break;
        case 4: RequestPayment(); break;
        case 0: break;
        default: 
            std::cout << COLOR_ERROR << "[!] Invalid choice!" << RESET;
            std::cin.get();
        }
    } while (choice != 0);
}

void Waiter::AssignTable() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT TableNumber, Section, Status "
            "FROM Tables "
            "ORDER BY TableNumber ASC"
        );

        std::cout << CLEAR_SCREEN;
        std::cout << COLOR_TITLE << "==========================================" << RESET << "\n";
        std::cout << BOLD << "              LIST OF TABLES              " << RESET << "\n";
        std::cout << COLOR_TITLE << "==========================================" << RESET << "\n";
        std::cout << COLOR_SUBTITLE << std::left << std::setw(15) << "Table"
            << std::setw(10) << "Section"
            << "Status" << RESET << "\n";
        std::cout << GRAY << "------------------------------------------" << RESET << "\n";

        while (res->next()) {
            std::string status = res->getString("Status");
            std::string colorStatus = (status == "Available" || status == "available") ? COLOR_SUCCESS : COLOR_ERROR;
            
            std::cout << std::left << std::setw(15)
                << res->getString("TableNumber")
                << std::setw(10)
                << res->getString("Section")
                << colorStatus << status << RESET << "\n";
        }

        std::cout << GRAY << "------------------------------------------" << RESET << "\n";
        std::string chooseTableNumber, chooseStatus;
        std::cout << "\nChoose table number (or 0 to back): ";
        std::cin >> chooseTableNumber;
        if(chooseTableNumber == "0") return;

        std::cout << "Choose status ([" << COLOR_SUCCESS << "1" << RESET << "] Available / [" << COLOR_ERROR << "2" << RESET << "] Occupied): ";
        std::cin >> chooseStatus;
        
        if (chooseStatus == "1") chooseStatus = "Available";
        else if (chooseStatus == "2") chooseStatus = "Occupied";
        else {
            std::cout << COLOR_ERROR << "[!] Invalid status." << RESET;
            std::cin.ignore(); std::cin.get();
            return;
        }

        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE Tables SET Status = ? WHERE TableNumber = ?"
        );

        pstmt->setString(1, chooseStatus);
        pstmt->setString(2, chooseTableNumber);
        pstmt->executeUpdate();

        std::cout << "\n" << COLOR_SUCCESS << "[SUCCESS] Table updated!" << RESET << "\n";

        delete pstmt; delete res; delete stmt;
        std::cout << "Press Enter to return..."; std::cin.ignore(); std::cin.get();

    }
    catch (sql::SQLException& e) {
        std::cerr << COLOR_ERROR << "DB Error: " << e.what() << RESET << std::endl;
        std::cin.get();
    }
}

void Waiter::placeOrder() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        
        std::cout << CLEAR_SCREEN;

        sql::Statement* stmtTable = con->createStatement();
        sql::ResultSet* resTable = stmtTable->executeQuery("SELECT TableNumber, Status FROM Tables ORDER BY TableNumber");

        std::cout << COLOR_TITLE << "================ TABLE STATUS ================" << RESET << "\n";
        int count = 0;
        while (resTable->next()) {
            std::string tNum = resTable->getString("TableNumber");
            std::string status = resTable->getString("Status");
            std::string color = (status == "Available" || status == "available") ? COLOR_SUCCESS : COLOR_ERROR;
            
            std::cout << "[ " << BOLD << "T." << std::left << std::setw(3) << tNum << RESET 
                      << ": " << color << std::setw(10) << status << RESET << "]  ";
            
            count++;
            if (count % 3 == 0) std::cout << "\n";
        }
        std::cout << "\n" << COLOR_TITLE << "==============================================" << RESET << "\n";
        delete resTable; delete stmtTable;
        // -------------------------------------------

        std::cout << "\n" << COLOR_TITLE << "--- NEW ORDER ---" << RESET << "\n";
        std::string tableNumber;
        std::cout << "Enter Table number to book: "; std::cin >> tableNumber;


        sql::PreparedStatement* pstmtOrder = con->prepareStatement(
            "INSERT INTO Orders (TableNumber, CreationTime, TotalAmount, CookingStatus) VALUES (?, NOW(), 0, 'PENDING')"
        );
        pstmtOrder->setString(1, tableNumber);
        pstmtOrder->executeUpdate();
        delete pstmtOrder;
        
        sql::PreparedStatement* pstmtUpdateTable = con->prepareStatement("UPDATE Tables SET Status = 'Occupied' WHERE TableNumber = ?");
        pstmtUpdateTable->setString(1, tableNumber);
        pstmtUpdateTable->executeUpdate();
        delete pstmtUpdateTable;

        sql::Statement* stmtID = con->createStatement();
        sql::ResultSet* resID = stmtID->executeQuery("SELECT LAST_INSERT_ID()");
        int currentOrderID = 0;
        if (resID->next()) currentOrderID = resID->getInt(1);
        delete resID; delete stmtID;

        std::cout << COLOR_SUCCESS << "[SYSTEM] New Order Created. ID: " << COLOR_HIGHLIGHT << currentOrderID << RESET << "\n";

        displayMenu();

        double totalAmount = 0;
        std::cout << "\n" << COLOR_SUBTITLE << "[ ADD ITEMS ]" << RESET << " (Enter Code '0' to finish)\n";
        
        while (true) {
            std::string itemCode; int qty;
            std::cout << "Item code: "; std::cin >> itemCode;
            if (itemCode == "0") break;
            std::cout << "Quantity: "; std::cin >> qty;

            sql::PreparedStatement* pstmtPrice = con->prepareStatement("SELECT Price, ItemName FROM MenuItem WHERE ItemCode=?");
            pstmtPrice->setString(1, itemCode);
            sql::ResultSet* res = pstmtPrice->executeQuery();
            
            if (!res->next()) {
                std::cout << COLOR_ERROR << "  [!] Item not found!" << RESET << "\n";
                delete res; delete pstmtPrice; continue;
            }

            double price = res->getDouble("Price");
            std::string iName = res->getString("ItemName");
            double sub = price * qty;
            totalAmount += sub;
            
            std::cout << "  -> Added: " << iName << " (" << sub << ")\n";
            delete res; delete pstmtPrice;

            sql::PreparedStatement* pstmtDetail = con->prepareStatement("INSERT INTO OrderDetail VALUES (?, ?, ?, ?)");
            pstmtDetail->setInt(1, currentOrderID);
            pstmtDetail->setString(2, itemCode);
            pstmtDetail->setInt(3, qty);
            pstmtDetail->setDouble(4, sub);
            pstmtDetail->executeUpdate();
            delete pstmtDetail;
        }

        sql::PreparedStatement* pstmtTotal = con->prepareStatement("UPDATE Orders SET TotalAmount=? WHERE OrderID=?");
        pstmtTotal->setDouble(1, totalAmount);
        pstmtTotal->setInt(2, currentOrderID);
        pstmtTotal->executeUpdate();
        delete pstmtTotal;

        std::cout << "\n" << COLOR_SUCCESS << "[SUCCESS] Order placed for Table " << tableNumber << "!" << RESET << "\n";
        std::cout << "Total Estimated: " << COLOR_HIGHLIGHT << totalAmount << RESET << "\n";
        std::cout << "Press Enter..."; std::cin.ignore(); std::cin.get();

    } catch (sql::SQLException& e) { 
        std::cerr << COLOR_ERROR << "Error: " << e.what() << RESET; 
        std::cin.ignore(); std::cin.get();
    }
}

void Waiter::updateOrder() {
    std::cout << CLEAR_SCREEN;

    sql::Connection* con;
    try {
        con = DatabaseManager::getInstance()->getConnection();
    } catch (sql::SQLException &e) {
        std::cerr << COLOR_ERROR << "DB Connection Error." << RESET << "\n";
        std::cin.get(); return;
    }

    try {
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT OrderID, TableNumber, TotalAmount, DATE_FORMAT(CreationTime, '%H:%i') as ShortTime "
            "FROM Orders WHERE CookingStatus = 'PENDING' ORDER BY OrderID DESC"
        );

        std::cout << COLOR_TITLE << "========= PENDING ORDERS LIST =========" << RESET << "\n";
        std::cout << COLOR_SUBTITLE << std::left 
                  << std::setw(10) << "ID" 
                  << std::setw(12) << "Table"
                  << std::setw(15) << "Total" 
                  << "Time" << RESET << "\n";
        std::cout << GRAY << "---------------------------------------" << RESET << "\n";

        bool hasPending = false;
        while (res->next()) {
            hasPending = true;
            std::cout << std::left 
                      << COLOR_HIGHLIGHT << std::setw(10) << res->getInt("OrderID") << RESET
                      << BOLD            << std::setw(12) << res->getString("TableNumber") << RESET 
                      <<                    std::setw(15) << std::fixed << std::setprecision(0) << res->getDouble("TotalAmount")
                      << res->getString("ShortTime") << "\n";
        }
        delete res; delete stmt;
        std::cout << GRAY << "=======================================" << RESET << "\n";

        if (!hasPending) {
            std::cout << YELLOW << "[!] No pending orders found to update." << RESET << "\n";
            std::cout << "Press Enter to return...";
            std::cin.ignore(); std::cin.get();
            return;
        }

    } catch (sql::SQLException &e) {
        std::cerr << COLOR_ERROR << "List Error: " << e.what() << RESET << "\n";
    }

    int orderID;
    std::cout << "\nEnter Order ID to update (0 to back): ";
    if (!(std::cin >> orderID)) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    if (orderID == 0) return;

    try {
        while (true) {
            std::cout << CLEAR_SCREEN;
            std::cout << COLOR_TITLE << "--- ORDER DETAILS (ID: " << COLOR_HIGHLIGHT << orderID << COLOR_TITLE << ") ---" << RESET << "\n";
            
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "SELECT od.ItemCode, m.ItemName, od.Quantity, od.SubTotal "
                "FROM OrderDetail od "
                "JOIN MenuItem m ON od.ItemCode = m.ItemCode "
                "WHERE od.OrderID = ?"
            );
            pstmt->setInt(1, orderID);
            sql::ResultSet* res = pstmt->executeQuery();

            std::cout << COLOR_SUBTITLE << std::left << std::setw(10) << "Code" 
                      << std::setw(25) << "Name" 
                      << std::setw(8) << "Qty" 
                      << "SubTotal" << RESET << "\n";
            std::cout << GRAY << "------------------------------------------------" << RESET << "\n";

            bool hasItems = false;
            while (res->next()) {
                hasItems = true;
                std::cout << std::left << std::setw(10) << res->getString("ItemCode")
                          << std::setw(25) << res->getString("ItemName")
                          << std::setw(8) << res->getInt("Quantity")
                          << std::fixed << std::setprecision(0) << res->getDouble("SubTotal") << "\n";
            }
            delete res; delete pstmt;

            if (!hasItems) std::cout << YELLOW << "  (Empty Order)" << RESET << "\n";
            std::cout << GRAY << "------------------------------------------------" << RESET << "\n";

            std::cout << "\n" << COLOR_HIGHLIGHT << "1." << RESET << " Add New Item";
            std::cout << "\n" << COLOR_HIGHLIGHT << "2." << RESET << " Change Quantity";
            std::cout << "\n" << COLOR_HIGHLIGHT << "3." << RESET << " Remove Item";
            std::cout << "\n" << COLOR_ERROR     << "0. Save & Exit" << RESET;
            std::cout << "\nChoice: ";

            int choice;
            if (!(std::cin >> choice)) {
                std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (choice == 0) break;

            if (choice == 1) {
                displayMenu();
                std::string itemCode; int qty;
                std::cout << "Enter Item Code: "; std::cin >> itemCode;
                std::cout << "Enter Quantity: "; std::cin >> qty;

                sql::PreparedStatement* pPrice = con->prepareStatement("SELECT Price FROM MenuItem WHERE ItemCode=?");
                pPrice->setString(1, itemCode);
                sql::ResultSet* rPrice = pPrice->executeQuery();
                
                if (rPrice->next()) {
                    double price = rPrice->getDouble("Price");
                    sql::PreparedStatement* pIns = con->prepareStatement(
                        "INSERT INTO OrderDetail (OrderID, ItemCode, Quantity, SubTotal) VALUES (?, ?, ?, ?) "
                        "ON DUPLICATE KEY UPDATE Quantity = Quantity + ?, SubTotal = SubTotal + ?"
                    );
                    double sub = price * qty;
                    pIns->setInt(1, orderID); pIns->setString(2, itemCode);
                    pIns->setInt(3, qty); pIns->setDouble(4, sub);
                    pIns->setInt(5, qty); pIns->setDouble(6, sub);
                    pIns->executeUpdate();
                    delete pIns;
                    std::cout << COLOR_SUCCESS << "Item added/merged!" << RESET << "\n";
                } else {
                    std::cout << COLOR_ERROR << "Item code not found!" << RESET << "\n";
                }
                delete rPrice; delete pPrice;
                std::cout << "Press Enter..."; std::cin.ignore(); std::cin.get();
            }
            else if (choice == 2) {
                std::string itemCode; int qty;
                std::cout << "Enter Item Code to update: "; std::cin >> itemCode;
                std::cout << "Enter NEW Quantity (0 to delete): "; std::cin >> qty;

                if (qty <= 0) {
                     sql::PreparedStatement* pDel = con->prepareStatement("DELETE FROM OrderDetail WHERE OrderID=? AND ItemCode=?");
                     pDel->setInt(1, orderID); pDel->setString(2, itemCode);
                     pDel->executeUpdate();
                     delete pDel;
                     std::cout << COLOR_SUCCESS << "Item removed." << RESET << "\n";
                } else {
                     sql::PreparedStatement* pPrice = con->prepareStatement("SELECT Price FROM MenuItem WHERE ItemCode=?");
                     pPrice->setString(1, itemCode);
                     sql::ResultSet* rPrice = pPrice->executeQuery();
                     if (rPrice->next()) {
                         double price = rPrice->getDouble("Price");
                         sql::PreparedStatement* pUpd = con->prepareStatement("UPDATE OrderDetail SET Quantity=?, SubTotal=? WHERE OrderID=? AND ItemCode=?");
                         pUpd->setInt(1, qty);
                         pUpd->setDouble(2, price * qty);
                         pUpd->setInt(3, orderID);
                         pUpd->setString(4, itemCode);
                         
                         if (pUpd->executeUpdate() > 0) std::cout << COLOR_SUCCESS << "Quantity updated!" << RESET << "\n";
                         else std::cout << COLOR_ERROR << "Item not found!" << RESET << "\n";
                         delete pUpd;
                     }
                     delete rPrice; delete pPrice;
                }
                std::cout << "Press Enter..."; std::cin.ignore(); std::cin.get();
            }
            else if (choice == 3) {
                std::string itemCode;
                std::cout << "Enter Item Code to remove: "; std::cin >> itemCode;
                sql::PreparedStatement* pDel = con->prepareStatement("DELETE FROM OrderDetail WHERE OrderID=? AND ItemCode=?");
                pDel->setInt(1, orderID); pDel->setString(2, itemCode);
                
                if (pDel->executeUpdate() > 0) std::cout << COLOR_SUCCESS << "Item removed!" << RESET << "\n";
                else std::cout << COLOR_ERROR << "Item not found." << RESET << "\n";
                
                delete pDel;
                std::cout << "Press Enter..."; std::cin.ignore(); std::cin.get();
            }

            sql::PreparedStatement* pTotal = con->prepareStatement(
                "UPDATE Orders SET TotalAmount = (SELECT IFNULL(SUM(SubTotal),0) FROM OrderDetail WHERE OrderID=?) WHERE OrderID=?"
            );
            pTotal->setInt(1, orderID); pTotal->setInt(2, orderID);
            pTotal->executeUpdate();
            delete pTotal;
        }

    } catch (sql::SQLException &e) {
        std::cerr << COLOR_ERROR << "DB Error: " << e.what() << RESET << "\n";
        std::cout << "Press Enter to return..."; std::cin.ignore(); std::cin.get();
    }
}

void Waiter::RequestPayment()
{
    std::cout << CLEAR_SCREEN;
    int orderID;

    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();

        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT o.OrderID, o.TableNumber, o.TotalAmount "
            "FROM Orders o "
            "JOIN Tables t ON o.TableNumber = t.TableNumber "
            "WHERE t.Status = 'Occupied' "
            "ORDER BY o.OrderID DESC"
        );

        std::cout << COLOR_TITLE << "========= PAYMENT QUEUE (OCCUPIED) =========" << RESET << "\n";
        std::cout << COLOR_SUBTITLE << std::left 
                  << std::setw(10) << "Order ID" 
                  << std::setw(12) << "Table" 
                  << "Est. Total" << RESET << "\n";
        std::cout << GRAY << "--------------------------------------------" << RESET << "\n";

        bool hasUnpaid = false;
        while (res->next()) {
            hasUnpaid = true;
            std::cout << std::left 
                      << COLOR_HIGHLIGHT << std::setw(10) << res->getInt("OrderID") << RESET
                      << BOLD            << std::setw(12) << res->getString("TableNumber") << RESET
                      << std::fixed << std::setprecision(0) << res->getDouble("TotalAmount") << " VND\n";
        }
        delete res; delete stmt;
        std::cout << GRAY << "============================================" << RESET << "\n";

        if (!hasUnpaid) {
            std::cout << COLOR_SUCCESS << "[ALL CLEAR] No occupied tables pending payment." << RESET << "\n";
            std::cout << "Press Enter to return...";
            std::cin.ignore(); std::cin.get();
            return;
        }

        std::cout << "\nEnter Order ID to pay (0 to back): ";
        if (!(std::cin >> orderID)) {
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        if (orderID == 0) return;

        sql::PreparedStatement* checkStmt = con->prepareStatement("SELECT TableNumber FROM Orders WHERE OrderID = ?");
        checkStmt->setInt(1, orderID);
        sql::ResultSet* rs = checkStmt->executeQuery();

        if (!rs->next()) {
            std::cout << COLOR_ERROR << "[!] Order not found!" << RESET << "\n";
            delete rs; delete checkStmt;
            std::cin.ignore(); std::cin.get();
            return;
        }

        std::string tableNumber = rs->getString("TableNumber");
        delete rs; delete checkStmt;

        sql::PreparedStatement* totalStmt = con->prepareStatement("SELECT IFNULL(SUM(SubTotal),0) AS Total FROM OrderDetail WHERE OrderID = ?");
        totalStmt->setInt(1, orderID);
        rs = totalStmt->executeQuery();
        rs->next();
        double totalAmount = rs->getDouble("Total");
        delete rs; delete totalStmt;

        std::cout << "\n" << COLOR_TITLE << "--- BILL CONFIRMATION ---" << RESET << "\n";
        std::cout << "Table: " << BOLD << tableNumber << RESET << "\n";
        std::cout << "Total Amount: " << COLOR_HIGHLIGHT << std::fixed << std::setprecision(0) << totalAmount << " VND" << RESET << "\n";
        
        char confirm;
        std::cout << "Confirm payment? (y/n): ";
        std::cin >> confirm;

        if (confirm != 'y' && confirm != 'Y') {
            std::cout << COLOR_ERROR << "Payment cancelled.\n" << RESET;
            std::cin.ignore(); std::cin.get();
            return;
        }

        sql::PreparedStatement* payStmt = con->prepareStatement("UPDATE Orders SET TotalAmount = ? WHERE OrderID = ?");
        payStmt->setDouble(1, totalAmount);
        payStmt->setInt(2, orderID);
        payStmt->execute();
        delete payStmt;

        sql::PreparedStatement* tableStmt = con->prepareStatement("UPDATE Tables SET Status = 'Available' WHERE TableNumber = ?");
        tableStmt->setString(1, tableNumber);
        tableStmt->execute();
        delete tableStmt;

        std::cout << "\n" << COLOR_SUCCESS << "[SUCCESS] Payment complete! Table " << tableNumber << " is now Available.\n" << RESET;
        std::cout << "Press Enter to return...";
        std::cin.ignore(); std::cin.get();

    }
    catch (sql::SQLException& e) {
        std::cerr << COLOR_ERROR << "DB Error: " << e.what() << RESET;
        std::cin.ignore(); std::cin.get();
    }
}

void Waiter::displayMenu() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT ItemCode, ItemName, Price, Category FROM MenuItem ORDER BY Category"
        );

        std::cout << "\n" << GRAY << "---------------- MENU ----------------" << RESET << "\n";
        std::cout << COLOR_SUBTITLE << std::left << std::setw(8) << "Code" 
                  << std::setw(20) << "Name" 
                  << std::setw(10) << "Price" << RESET << "\n";
        std::cout << GRAY << "--------------------------------------" << RESET << "\n";

        while (res->next()) {
            std::cout << std::left << std::setw(8) << res->getString("ItemCode")
                      << std::setw(20) << res->getString("ItemName")
                      << std::fixed << std::setprecision(0) << std::setw(10) << res->getDouble("Price") << "\n";
        }

        delete res; delete stmt;
        std::cout << GRAY << "--------------------------------------" << RESET << "\n";
    } catch (sql::SQLException& e) {
        std::cerr << COLOR_ERROR << "Error: " << e.what() << RESET << std::endl;
    }
}