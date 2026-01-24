// database.cpp

#include "database.h"

DatabaseManager* DatabaseManager::instance = nullptr;

DatabaseManager::DatabaseManager(std::string ip, std::string username, std::string password) {
    try {
        sql::mysql::MySQL_Driver *driver;
        driver = sql::mysql::get_mysql_driver_instance();
        
        con = driver->connect("tcp://" + ip, username, password);
        con->setSchema("restaurant_db");
    } catch (sql::SQLException &e) {
        throw std::runtime_error("Database connection failed: " + std::string(e.what()));
    }
}

DatabaseManager* DatabaseManager::getInstance(std::string ip, std::string user, std::string pass) {
    if (instance == nullptr) {
        instance = new DatabaseManager(ip, user, pass);
    }
    return instance;
}

sql::Connection* DatabaseManager::getConnection() {
    return con;
}

DatabaseManager::~DatabaseManager() {
    if (con) {
        delete con;
    }
}

std::string DatabaseManager::authenticate(std::string user, std::string pass) {
    sql::Connection* con = DatabaseManager::getInstance()->getConnection();
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "SELECT Role FROM User WHERE UserName = ? AND Password = ?"
    );
    pstmt->setString(1, user);
    pstmt->setString(2, pass);
    sql::ResultSet* res = pstmt->executeQuery();

    std::string role = "none";
    if (res->next()) {
        role = res->getString("Role");
    }
    delete res; delete pstmt;
    return role; 
}
