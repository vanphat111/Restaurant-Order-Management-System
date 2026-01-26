// admin.cpp

#include "admin.h"
#include <limits> // Thư viện bắt buộc cho numeric_limits

void Admin::showMenu() {
    int choice;
    do {
        std::cout << "\n==========================================";
        std::cout << "\n   ADMIN DASHBOARD - Admin: " << this->userName;
        std::cout << "\n==========================================";
        std::cout << "\n1. Manage System Users";
        std::cout << "\n2. View Revenue Reports";
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
            case 2: viewRevenueReport(); break;
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

void Admin::viewRevenueReport() {
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
