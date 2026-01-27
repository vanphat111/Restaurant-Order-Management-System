// user.cpp

#include "user.h"
#include "chef.h" // Add class chức vụ ở đây
#include "admin.h"
#include "waiter.h"

std::string User::hashPassword(const std::string& plainText) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, plainText.c_str(), plainText.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

User* User::login(std::string _userName, std::string _password) {
    sql::Connection* con = DatabaseManager::getInstance()->getConnection();
    
    std::string hashedPassword = hashPassword(_password); 

    sql::PreparedStatement* pstmt = con->prepareStatement(
        "SELECT Role FROM User WHERE UserName = ? AND Password = ?"
    );
    pstmt->setString(1, _userName);
    pstmt->setString(2, hashedPassword);
    
    sql::ResultSet* res = pstmt->executeQuery();

    User* currentUser = nullptr;
    if (res->next()) {
        std::string role = res->getString("Role");
        if (role == "Chef") currentUser = new Chef(_userName);
        else if (role == "Admin") currentUser = new Admin(_userName);
        else if (role == "Waiter") currentUser = new Waiter(_userName);
    }

    delete res; delete pstmt;
    return currentUser;
}

bool User::recoverPassword(std::string _userName, std::string _newPass) {
    try {
        sql::Connection* con = DatabaseManager::getInstance()->getConnection();
        
        std::string hashedPassword = hashPassword(_newPass);

        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE User SET Password = ? WHERE UserName = ?"
        );

        pstmt->setString(1, hashedPassword);
        pstmt->setString(2, _userName);

        int rowsAffected = pstmt->executeUpdate();
        delete pstmt;

        return rowsAffected > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Error SQL: " << e.what() << std::endl;
        return false;
    }
}
