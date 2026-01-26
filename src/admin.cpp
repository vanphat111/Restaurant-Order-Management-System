// admin.cpp

#include "admin.h"
#include <limits>

void Admin::showMenu() {
    int choice;
    do {
        std::cout << "\n==========================================";
        std::cout << "\n   ADMIN DASHBOARD - Admin: " << this->userName;
        std::cout << "\n==========================================";
        std::cout << "\n1. Manage System Users";
        std::cout << "\n2. View Revenue Reports";
        std::cout << "\n3. Generate Daily Report";
        std::cout << "\n4. Get Best Seller";
        std::cout << "\n5. Manage Menu";
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
            case 1: manageUsers(); break;
            case 2: viewSaleReport(); break;
            case 3: generateDailyReport(); break;
            case 4: getBestSellers(); break;
            case 5: manageMenu(); break;
            case 0: break;
            default: std::cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void Admin::manageUsers() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT UserID, UserName, Role FROM User");

        std::cout << "\n--- CURRENT SYSTEM USERS ---\n";
        std::cout << std::left << std::setw(10) << "ID" << std::setw(15) << "Username" << "Role\n";
        while (res->next()) {
            std::cout << std::left << std::setw(10) << res->getString("UserID") 
                      << std::setw(15) << res->getString("UserName") 
                      << res->getString("Role") << "\n";
        }
        delete res; delete stmt;

        std::cout << "\n1. Add User | 2. Delete User | 3. Change role | 0. Back: ";
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

    } catch (sql::SQLException &e) { std::cerr << "DB Error: " << e.what() << "\n"; }
}

void Admin::addUser() {
    std::string id, name, pass, role;
    std::cout << "New UserID: "; std::getline(std::cin, id);
    std::cout << "Username: "; std::getline(std::cin, name);
    std::cout << "Password: "; std::getline(std::cin, pass);
    std::cout << "Role [Admin/Waiter/Chef]: "; std::getline(std::cin, role);

    if (!(role == "Admin" || role == "Chef" || role == "Waiter")) {
        std::cout << "[FAILED] This system does not have a/an " << role << " role.\n";
        return;
    }

    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO User(UserID, UserName, Password, Role) VALUES(?, ?, ?, ?)"
        );
        pstmt->setString(1, id);
        pstmt->setString(2, name);
        pstmt->setString(3, pass);
        pstmt->setString(4, role);
        pstmt->executeUpdate();
        std::cout << "[SUCCESS] User added.\n";
        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << "[ERROR] Could not add user.\n"; }
}

void Admin::deleteUser() {
    std::string id;
    std::cout << "Enter UserID to delete: "; std::getline(std::cin, id);
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement("DELETE FROM User WHERE UserID = ?");
        pstmt->setString(1, id);
        pstmt->executeUpdate();
        std::cout << "[SUCCESS] User deleted.\n";
        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << "[ERROR] Cannot delete user.\n"; }
}

void Admin::changeRole() {
    std::string id, _role;
    std::cout << "Enter UserID to change role: "; std::getline(std::cin, id);
    std::cout << "Enter role [Admin/Chef/Waiter]: "; std::getline(std::cin, _role);

    if (!(_role == "Admin" || _role == "Chef" || _role == "Waiter")) {
        std::cout << "[FAILED] This system does not have a/an " << _role << " role.\n";
        return;
    }
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement("UPDATE User SET Role = ? WHERE UserID = ?");
        pstmt->setString(1, _role);
        pstmt->setString(2, id);
        int rowsAffected = pstmt->executeUpdate();

        if (rowsAffected > 0) {
            std::cout << "[SUCCESS] Role updated for UserID: " << id << "\n";
        } else {
            std::cout << "[FAILED] No user found with ID: " << id << "\n";
        }

        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << "[ERROR] Cannot change role.\n"; }
}

void Admin::viewSaleReport() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM Report ORDER BY ReportDate DESC");

        std::cout << "\n--- REVENUE REPORT ---\n";
        std::cout << std::left << std::setw(15) << "Date" << std::setw(15) << "Revenue" << "Orders\n";
        while (res->next()) {
            std::cout << std::left << std::setw(15) << res->getString("ReportDate")
                      << std::setw(15) << res->getDouble("DailyRevenue")
                      << res->getInt("OrderCount") << "\n";
        }
        delete res; delete stmt;
        
        std::cout << "\nPress Enter to return...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } catch (sql::SQLException &e) { std::cerr << "DB Error: " << e.what() << "\n"; }
}

void Admin::manageMenu() {
    int choice;
    do {
        std::cout << "\n--- MENU MANAGEMENT ---";
        std::cout << "\n1. Display All Items";
        std::cout << "\n2. Add New Item";
        std::cout << "\n3. Update Item (Price/Qty)";
        std::cout << "\n4. Delete Item";
        std::cout << "\n0. Back";
        std::cout << "\nEnter choice: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: displayAllItems(); break;
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
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM MenuItem");
        std::cout << "CURRENT MENU";
        std::cout << "\n" << std::string(75, '=') << "\n";
        std::cout << std::left << std::setw(10) << "Code" << std::setw(20) << "Name" 
                  << std::setw(12) << "Price" << std::setw(10) << "Qty" << "Category\n";
        std::cout << std::string(75, '-') << "\n";

        while (res->next()) {
            std::cout << std::left << std::setw(10) << res->getString("ItemCode")
                      << std::setw(20) << res->getString("ItemName")
                      << std::setw(12) << res->getDouble("Price")
                      << std::setw(10) << res->getInt("InventoryQty")
                      << res->getString("Category") << "\n";
        }
        delete res; delete stmt;
        std::cout << std::string(75, '=') << "\n";
    } catch (sql::SQLException &e) { std::cerr << "Error: " << e.what() << "\n"; }
}

void Admin::addMenuItem() {
    std::string code, name, desc, cat;
    double price; int qty;

    displayAllItems();

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
        std::cout << "[SUCCESS] New item added to menu.\n";
        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << "[ERROR] Duplicate code or DB error.\n"; }
}

void Admin::updateMenuItem() {
    displayAllItems();

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
        
        if (pstmt->executeUpdate() > 0) std::cout << "[SUCCESS] Item updated.\n";
        else std::cout << "[FAILED] Item code not found.\n";
        delete pstmt;
    } catch (sql::SQLException &e) { std::cout << "[ERROR] Update failed.\n"; }
}

void Admin::deleteMenuItem() {
    displayAllItems();

    std::string code;
    std::cout << "Enter Item Code to delete: "; std::getline(std::cin, code);
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement("DELETE FROM MenuItem WHERE ItemCode = ?");
        pstmt->setString(1, code);
        if (pstmt->executeUpdate() > 0) std::cout << "[SUCCESS] Item removed.\n";
        else std::cout << "[FAILED] Item not found.\n";
        delete pstmt;
    } catch (sql::SQLException &e) { 
        std::cout << "[ERROR] Cannot delete (Item might be linked to existing orders).\n"; 
    }
}

void Admin::generateDailyReport() {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        
        std::string query = 
            "INSERT INTO Report (ReportDate, DailyRevenue, OrderCount) "
            "SELECT DATE(CreationTime), SUM(TotalAmount), COUNT(OrderID) "
            "FROM Orders "
            "WHERE CookingStatus = 'READY' AND DATE(CreationTime) = CURDATE() "
            "ON DUPLICATE KEY UPDATE "
            "DailyRevenue = VALUES(DailyRevenue), OrderCount = VALUES(OrderCount)";

        sql::Statement* stmt = con->createStatement();
        stmt->execute(query);
        delete stmt;

        std::cout << "[SUCCESS] Daily Report for today has been updated!\n";

        sql::Statement* viewStmt = con->createStatement();
        sql::ResultSet* res = viewStmt->executeQuery(
            "SELECT * FROM Report WHERE ReportDate = CURDATE()"
        );

        if (res->next()) {
            std::cout << "------------------------------------------\n";
            std::cout << "Date: " << res->getString("ReportDate") << "\n";
            std::cout << "Total Revenue: " << res->getDouble("DailyRevenue") << " VND\n";
            std::cout << "Total Orders: " << res->getInt("OrderCount") << "\n";
            std::cout << "------------------------------------------\n";
        }
        delete res; delete viewStmt;

    } catch (sql::SQLException &e) {
        std::cerr << "Report Error: " << e.what() << "\n";
    }
}

void Admin::getBestSellers() {
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

        std::cout << "\n" << std::string(45, '=') << "\n";
        std::cout << "        TOP 5 BEST SELLING ITEMS        \n";
        std::cout << std::string(45, '-') << "\n";
        std::cout << std::left << std::setw(30) << "Item Name" << "Total Sold\n";
        std::cout << std::string(45, '-') << "\n";

        bool hasData = false;
        while (res->next()) {
            hasData = true;
            std::cout << std::left << std::setw(30) << res->getString("ItemName")
                      << res->getInt("TotalSold") << " units\n";
        }

        if (!hasData) {
            std::cout << "No sales data available yet.\n";
        }
        std::cout << std::string(45, '=') << "\n";

        delete res; delete stmt;
        
        std::cout << "\nPress Enter to return...";
        std::cin.get();

    } catch (sql::SQLException &e) {
        std::cerr << "Best Seller Error: " << e.what() << "\n";
    }
}
