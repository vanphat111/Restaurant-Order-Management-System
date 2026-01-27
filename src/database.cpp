// database.cpp

#include "database.h"
#include <cstdlib>

DatabaseManager* DatabaseManager::instance = nullptr;

DatabaseManager::DatabaseManager(std::string ip, std::string username, std::string password) {
    try {
        sql::mysql::MySQL_Driver *driver;
        driver = sql::mysql::get_mysql_driver_instance();
        
        std::string connectionString = ip;
        if (connectionString.find("tcp://") == std::string::npos) {
            connectionString = "tcp://" + connectionString;
        }

        con = driver->connect(connectionString, username, password);
        con->setSchema("restaurant_db");
        
        std::cout << "[DEBUG] Connected to database at: " << connectionString << std::endl;

    } catch (sql::SQLException &e) {
        throw std::runtime_error("Database connection failed: " + std::string(e.what()));
    }
}

DatabaseManager* DatabaseManager::getInstance(std::string ip, std::string user, std::string pass) {
    if (instance == nullptr) {
        
        std::string final_ip = ip;
        if (final_ip.empty()) {
            const char* env_host = std::getenv("DB_HOST");
            final_ip = env_host ? env_host : "127.0.0.1:3306";
        }

        std::string final_user = user;
        if (final_user.empty()) {
            const char* env_user = std::getenv("DB_USER");
            final_user = env_user ? env_user : "root";
        }

        std::string final_pass = pass;
        if (final_pass.empty()) {
            const char* env_pass = std::getenv("DB_PASS");
            final_pass = env_pass ? env_pass : "1234";
        }

        instance = new DatabaseManager(final_ip, final_user, final_pass);
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