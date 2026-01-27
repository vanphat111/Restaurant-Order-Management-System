// user.cpp

#include "user.h"
#include "chef.h" // Add class chức vụ ở đây
#include "admin.h"
#include "waiter.h"

User* User::login(std::string _userName, std::string _password) {
    sql::Connection* con = DatabaseManager::getInstance()->getConnection();
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "SELECT Role FROM User WHERE UserName = ? AND Password = ?"
    );
    pstmt->setString(1, _userName);
    pstmt->setString(2, _password);
    sql::ResultSet* res = pstmt->executeQuery();

    User* currentUser = nullptr;
    if (res->next()) {
        std::string role = res->getString("Role");
        if (role == "Chef") currentUser = new Chef(_userName); // Kế thừa role ngay đây
        else if (role == "Admin") currentUser = new Admin(_userName);
        else if (role == "Waiter") currentUser = new Waiter(_userName);

    }

    delete res; delete pstmt;
    return currentUser;
}

bool User::recoverPassword(std::string _userName, std::string _newPass) {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE User SET Password = ? WHERE UserName = ?"
        );

        pstmt->setString(1, _newPass);
        pstmt->setString(2, _userName);

        int rowsAffected = pstmt->executeUpdate();
        delete pstmt;

        return rowsAffected > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Error SQL: " << e.what() << std::endl;
        return false;
    }
}

