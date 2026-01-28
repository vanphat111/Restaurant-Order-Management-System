// admin.cpp

#include "admin.h"
#include "colors.h"
#include <limits>
#include <iostream>
#include <iomanip>

void Admin::showMenu() {
    int choice;
    do {
        // Xóa màn hình cho sạch sẽ mỗi lần load lại menu
        std::cout << CLEAR_SCREEN;
        
        // Header chính
        std::cout << COLOR_TITLE << "==========================================";
        std::cout << "\n   " << BOLD << "ADMIN DASHBOARD" << RESET << " - User: " << COLOR_HIGHLIGHT << this->userName << RESET;
        std::cout << COLOR_TITLE << "\n==========================================" << RESET;
        
        // Menu options
        std::cout << "\n" << COLOR_HIGHLIGHT << "1." << RESET << " Manage System Users";
        std::cout << "\n" << COLOR_HIGHLIGHT << "2." << RESET << " View Revenue Reports";
        std::cout << "\n" << COLOR_HIGHLIGHT << "3." << RESET << " Generate Daily Report";
        std::cout << "\n" << COLOR_HIGHLIGHT << "4." << RESET << " Get Best Seller";
        std::cout << "\n" << COLOR_HIGHLIGHT << "5." << RESET << " Manage Menu";
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
            case 1: manageUsers(); break;
            case 2: viewSaleReport(); break;
            case 3: generateDailyReport(); break;
            case 4: getBestSellers(); break;
            case 5: manageMenu(); break;
            case 0: break;
            default: 
                std::cout << COLOR_ERROR << "[!] Invalid choice! Press Enter to try again." << RESET;
                std::cin.get();
        }
    } while (choice != 0);
}

void Admin::manageUsers() {
    std::cout << CLEAR_SCREEN;
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT UserID, UserName, Role FROM User");

        std::cout << COLOR_SUBTITLE << "\n--- CURRENT SYSTEM USERS ---\n" << RESET;
        // Header bảng màu Cyan đậm
        std::cout << COLOR_SUBTITLE << std::left << std::setw(10) << "ID" << std::setw(15) << "Username" << "Role" << RESET << "\n";
        std::cout << GRAY << "------------------------------------------" << RESET << "\n";
        
        while (res->next()) {
            std::cout << std::left << std::setw(10) << res->getString("UserID") 
                      << std::setw(15) << res->getString("UserName") 
                      << COLOR_HIGHLIGHT << res->getString("Role") << RESET << "\n";
        }
        delete res; delete stmt;

        std::cout << GRAY << "------------------------------------------" << RESET << "\n";
        std::cout << "1. Add User | 2. Delete User | 3. Change role | " << COLOR_ERROR << "0. Back" << RESET << ": ";
        int sub; 
        if(!(std::cin >> sub)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (sub == 1) addUser();
        else if (sub == 2) deleteUser();
        else if (sub == 3) changeRole();

    } catch (sql::SQLException &e) { std::cerr << COLOR_ERROR << "DB Error: " << e.what() << RESET << "\n"; std::cin.get(); }
}

void Admin::addUser() {
    std::string id, name, pass, role;
    std::cout << "\n" << COLOR_SUBTITLE << "[ ADD NEW USER ]" << RESET << "\n";
    std::cout << "New UserID: "; std::getline(std::cin, id);
    std::cout << "Username: "; std::getline(std::cin, name);
    std::cout << "Password: "; std::getline(std::cin, pass);
    std::cout << "Role [Admin/Waiter/Chef]: "; std::getline(std::cin, role);

    if (!(role == "Admin" || role == "Chef" || role == "Waiter")) {
        std::cout << COLOR_ERROR << "[FAILED] This system does not have a/an " << role << " role." << RESET << "\n";
        std::cout << "Press Enter to continue..."; std::cin.get();
        return;
    }

    try {
        std::string hashedPassword = User::hashPassword(pass);

        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO User(UserID, UserName, Password, Role) VALUES(?, ?, ?, ?)"
        );
        pstmt->setString(1, id);
        pstmt->setString(2, name);
        pstmt->setString(3, hashedPassword);
        pstmt->setString(4, role);
        pstmt->executeUpdate();
        std::cout << COLOR_SUCCESS << "[SUCCESS] User added successfully." << RESET << "\n";
        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << COLOR_ERROR << "[ERROR] Could not add user (Duplicate ID?)." << RESET << "\n"; }
    
    std::cout << "Press Enter to return..."; std::cin.get();
}

void Admin::deleteUser() {
    std::string id;
    std::cout << "\n" << COLOR_ERROR << "[ DELETE USER ]" << RESET << "\n";
    std::cout << "Enter UserID to delete: "; std::getline(std::cin, id);
    
    char confirm;
    std::cout << "Are you sure? (y/n): "; std::cin >> confirm;
    std::cin.ignore(); // Xóa buffer

    if (confirm != 'y' && confirm != 'Y') return;

    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement("DELETE FROM User WHERE UserID = ?");
        pstmt->setString(1, id);
        int rows = pstmt->executeUpdate();
        if (rows > 0)
            std::cout << COLOR_SUCCESS << "[SUCCESS] User deleted." << RESET << "\n";
        else 
            std::cout << COLOR_ERROR << "[FAILED] User ID not found." << RESET << "\n";
        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << COLOR_ERROR << "[ERROR] Cannot delete user." << RESET << "\n"; }
    
    std::cout << "Press Enter to return..."; std::cin.get();
}

void Admin::changeRole() {
    std::string id, _role;
    std::cout << "\n" << COLOR_SUBTITLE << "[ CHANGE ROLE ]" << RESET << "\n";
    std::cout << "Enter UserID to change role: "; std::getline(std::cin, id);
    std::cout << "Enter new role [Admin/Chef/Waiter]: "; std::getline(std::cin, _role);

    if (!(_role == "Admin" || _role == "Chef" || _role == "Waiter")) {
        std::cout << COLOR_ERROR << "[FAILED] Invalid role." << RESET << "\n";
        std::cout << "Press Enter to return..."; std::cin.get();
        return;
    }
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement("UPDATE User SET Role = ? WHERE UserID = ?");
        pstmt->setString(1, _role);
        pstmt->setString(2, id);
        int rowsAffected = pstmt->executeUpdate();

        if (rowsAffected > 0) {
            std::cout << COLOR_SUCCESS << "[SUCCESS] Role updated for UserID: " << id << RESET << "\n";
        } else {
            std::cout << COLOR_ERROR << "[FAILED] No user found with ID: " << id << RESET << "\n";
        }

        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << COLOR_ERROR << "[ERROR] Cannot change role." << RESET << "\n"; }
    
    std::cout << "Press Enter to return..."; std::cin.get();
}

void Admin::viewSaleReport() {
    std::cout << CLEAR_SCREEN;
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM Report ORDER BY ReportDate DESC");

        std::cout << COLOR_TITLE << "\n--- REVENUE REPORT ---\n" << RESET;
        std::cout << COLOR_SUBTITLE << std::left << std::setw(15) << "Date" << std::setw(15) << "Revenue (VND)" << "Orders" << RESET << "\n";
        std::cout << GRAY << "------------------------------------------" << RESET << "\n";
        
        while (res->next()) {
            std::cout << std::left << std::setw(15) << res->getString("ReportDate")
                      << std::setw(15) << std::fixed << std::setprecision(0) << res->getDouble("DailyRevenue")
                      << res->getInt("OrderCount") << "\n";
        }
        delete res; delete stmt;
        
        std::cout << "\nPress Enter to return...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } catch (sql::SQLException &e) { std::cerr << COLOR_ERROR << "DB Error: " << e.what() << RESET << "\n"; std::cin.get(); }
}

void Admin::manageMenu() {
    int choice;
    do {
        std::cout << CLEAR_SCREEN;
        std::cout << COLOR_TITLE << "--- MENU MANAGEMENT ---" << RESET;
        std::cout << "\n" << COLOR_HIGHLIGHT << "1." << RESET << " Display All Items";
        std::cout << "\n" << COLOR_HIGHLIGHT << "2." << RESET << " Add New Item";
        std::cout << "\n" << COLOR_HIGHLIGHT << "3." << RESET << " Update Item (Price/Qty)";
        std::cout << "\n" << COLOR_HIGHLIGHT << "4." << RESET << " Delete Item";
        std::cout << "\n" << COLOR_ERROR     << "0." << RESET << " Back";
        std::cout << "\nEnter choice: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: displayAllItems(); std::cout << "Press Enter..."; std::cin.get(); break;
            case 2: addMenuItem(); break;
            case 3: updateMenuItem(); break;
            case 4: deleteMenuItem(); break;
        }
    } while (choice != 0);
}

void Admin::displayAllItems() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM MenuItem ORDER BY Category, ItemCode");
        
        std::cout << COLOR_TITLE << "\nCURRENT MENU" << RESET;
        std::cout << "\n" << GRAY << std::string(75, '=') << RESET << "\n";
        std::cout << COLOR_SUBTITLE << std::left << std::setw(10) << "Code" << std::setw(25) << "Name" 
                  << std::setw(12) << "Price" << std::setw(10) << "Qty" << "Category" << RESET << "\n";
        std::cout << GRAY << std::string(75, '-') << RESET << "\n";

        while (res->next()) {
            std::cout << std::left << std::setw(10) << res->getString("ItemCode")
                      << std::setw(25) << res->getString("ItemName")
                      << std::setw(12) << res->getDouble("Price")
                      << std::setw(10) << res->getInt("InventoryQty")
                      << res->getString("Category") << "\n";
        }
        delete res; delete stmt;
        std::cout << GRAY << std::string(75, '=') << RESET << "\n";
    } catch (sql::SQLException &e) { std::cerr << COLOR_ERROR << "Error: " << e.what() << RESET << "\n"; }
}

void Admin::addMenuItem() {
    std::cout << CLEAR_SCREEN;
    displayAllItems();
    std::cout << "\n" << COLOR_SUBTITLE << "[ ADD NEW ITEM ]" << RESET << "\n";

    std::string code, name, desc, cat;
    double price; int qty;

    std::cout << "Item Code (e.g., M03): "; std::getline(std::cin, code);
    std::cout << "Item Name: "; std::getline(std::cin, name);
    std::cout << "Price: "; std::cin >> price;
    std::cout << "Initial Qty: "; std::cin >> qty;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Description: "; std::getline(std::cin, desc);
    std::cout << "Category: "; std::getline(std::cin, cat);

    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO MenuItem VALUES(?, ?, ?, ?, ?, ?)"
        );
        pstmt->setString(1, code);
        pstmt->setString(2, name);
        pstmt->setDouble(3, price);
        pstmt->setString(4, desc);
        pstmt->setInt(5, qty);
        pstmt->setString(6, cat);
        pstmt->executeUpdate();
        std::cout << COLOR_SUCCESS << "[SUCCESS] New item added to menu." << RESET << "\n";
        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << COLOR_ERROR << "[ERROR] Duplicate code or DB error." << RESET << "\n"; }
    
    std::cout << "Press Enter to return..."; std::cin.get();
}

void Admin::updateMenuItem() {
    std::cout << CLEAR_SCREEN;
    displayAllItems();
    std::cout << "\n" << COLOR_SUBTITLE << "[ UPDATE ITEM ]" << RESET << "\n";

    std::string code; double newPrice; int newQty;
    std::cout << "Enter Item Code to update: "; std::getline(std::cin, code);
    std::cout << "New Price: "; std::cin >> newPrice;
    std::cout << "New Inventory Qty: "; std::cin >> newQty;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE MenuItem SET Price = ?, InventoryQty = ? WHERE ItemCode = ?"
        );
        pstmt->setDouble(1, newPrice);
        pstmt->setInt(2, newQty);
        pstmt->setString(3, code);
        
        if (pstmt->executeUpdate() > 0) std::cout << COLOR_SUCCESS << "[SUCCESS] Item updated." << RESET << "\n";
        else std::cout << COLOR_ERROR << "[FAILED] Item code not found." << RESET << "\n";
        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << COLOR_ERROR << "[ERROR] Update failed." << RESET << "\n"; }
    
    std::cout << "Press Enter to return..."; std::cin.get();
}

void Admin::deleteMenuItem() {
    std::cout << CLEAR_SCREEN;
    displayAllItems();
    std::cout << "\n" << COLOR_ERROR << "[ DELETE ITEM ]" << RESET << "\n";

    std::string code;
    std::cout << "Enter Item Code to delete: "; std::getline(std::cin, code);
    
    char confirm;
    std::cout << "Are you sure? (y/n): "; std::cin >> confirm;
    std::cin.ignore();

    if (confirm != 'y' && confirm != 'Y') return;

    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement("DELETE FROM MenuItem WHERE ItemCode = ?");
        pstmt->setString(1, code);
        if (pstmt->executeUpdate() > 0) std::cout << COLOR_SUCCESS << "[SUCCESS] Item removed." << RESET << "\n";
        else std::cout << COLOR_ERROR << "[FAILED] Item not found." << RESET << "\n";
        delete pstmt;
    } catch (sql::SQLException &e) { 
        std::cout << COLOR_ERROR << "[ERROR] Cannot delete (Item might be linked to existing orders)." << RESET << "\n"; 
    }
    std::cout << "Press Enter to return..."; std::cin.get();
}

void Admin::generateDailyReport() {
    std::cout << CLEAR_SCREEN;
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        
        std::string query = "INSERT INTO Report (ReportDate, DailyRevenue, OrderCount) "
            "SELECT DATE(CreationTime), IFNULL(SUM(TotalAmount), 0), COUNT(OrderID) "
            "FROM Orders "
            "WHERE CookingStatus = 'READY' AND DATE(CreationTime) = CURDATE() "
            "GROUP BY DATE(CreationTime) "
            "ON DUPLICATE KEY UPDATE "
            "DailyRevenue = VALUES(DailyRevenue), OrderCount = VALUES(OrderCount)";

        sql::Statement* stmt = con->createStatement();
        stmt->execute(query);
        delete stmt;

        std::cout << COLOR_SUCCESS << "[SUCCESS] Daily Report for today has been updated!" << RESET << "\n";

        sql::Statement* viewStmt = con->createStatement();
        sql::ResultSet* res = viewStmt->executeQuery(
            "SELECT * FROM Report WHERE ReportDate = CURDATE()"
        );

        if (res->next()) {
            std::cout << COLOR_TITLE << "------------------------------------------" << RESET << "\n";
            std::cout << "Date: " << BOLD << res->getString("ReportDate") << RESET << "\n";
            std::cout << "Total Revenue: " << COLOR_HIGHLIGHT << std::fixed << std::setprecision(0) << res->getDouble("DailyRevenue") << " VND" << RESET << "\n";
            std::cout << "Total Orders: " << res->getInt("OrderCount") << "\n";
            std::cout << COLOR_TITLE << "------------------------------------------" << RESET << "\n";
        }
        delete res; delete viewStmt;

    } catch (sql::SQLException &e) {
        std::cerr << COLOR_ERROR << "Report Error: " << e.what() << RESET << "\n";
    }
    std::cout << "\nPress Enter to return..."; std::cin.get();
}

void Admin::getBestSellers() {
    std::cout << CLEAR_SCREEN;
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();

        std::string query = 
            "SELECT m.ItemName, SUM(od.Quantity) AS TotalSold "
            "FROM OrderDetail od "
            "JOIN MenuItem m ON od.ItemCode = m.ItemCode "
            "GROUP BY m.ItemName "
            "ORDER BY TotalSold DESC "
            "LIMIT 5";

        sql::ResultSet* res = stmt->executeQuery(query);

        std::cout << "\n" << COLOR_TITLE << std::string(45, '=') << RESET << "\n";
        std::cout << BOLD << "        TOP 5 BEST SELLING ITEMS        " << RESET << "\n";
        std::cout << COLOR_TITLE << std::string(45, '-') << RESET << "\n";
        std::cout << COLOR_SUBTITLE << std::left << std::setw(30) << "Item Name" << "Total Sold" << RESET << "\n";
        std::cout << COLOR_TITLE << std::string(45, '-') << RESET << "\n";

        bool hasData = false;
        while (res->next()) {
            hasData = true;
            std::cout << std::left << std::setw(30) << res->getString("ItemName")
                      << COLOR_HIGHLIGHT << res->getInt("TotalSold") << " units" << RESET << "\n";
        }

        if (!hasData) {
            std::cout << YELLOW << "No sales data available yet." << RESET << "\n";
        }
        std::cout << COLOR_TITLE << std::string(45, '=') << RESET << "\n";

        delete res; delete stmt;
        
        std::cout << "\nPress Enter to return...";
        std::cin.get();

    } catch (sql::SQLException &e) {
        std::cerr << COLOR_ERROR << "Best Seller Error: " << e.what() << RESET << "\n";
        std::cin.get();
    }
}