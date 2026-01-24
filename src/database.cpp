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

