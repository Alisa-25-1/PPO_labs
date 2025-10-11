#ifndef DATABASECONNECTION_HPP
#define DATABASECONNECTION_HPP

#include <pqxx/pqxx>
#include <memory>
#include <string>

class DatabaseConnection {
public:
    explicit DatabaseConnection(const std::string& connectionString);
    virtual ~DatabaseConnection(); 

    // Запрещаем копирование
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    // Получение соединения
    virtual pqxx::connection& getConnection();  
    virtual bool isConnected() const;

    // Транзакции
    virtual pqxx::work beginTransaction();
    virtual void commitTransaction(pqxx::work& transaction);
    virtual void rollbackTransaction(pqxx::work& transaction);

private:
    std::unique_ptr<pqxx::connection> connection_;
    std::string connectionString_;
};

#endif // DATABASECONNECTION_HPP