// database.h

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <string>

class DatabaseManager {
private:
    static DatabaseManager* instance;
    sql::Connection* con;
    DatabaseManager(std::string ip, std::string username, std::string password);

public:
    static DatabaseManager* getInstance(std::string ip = "127.0.0.1:3306", std::string user = "root", std::string pass = "1234");
    sql::Connection* getConnection();
    ~DatabaseManager();
};
