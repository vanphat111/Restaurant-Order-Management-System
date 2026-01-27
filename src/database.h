// database.h

#pragma once

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <string>

class DatabaseManager {
private:
    static DatabaseManager* instance;
    sql::Connection* con;
    DatabaseManager(std::string ip, std::string username, std::string password);

public:
    static DatabaseManager* getInstance(std::string ip = "", std::string user = "", std::string pass = "");
    sql::Connection* getConnection();
    ~DatabaseManager();
};
