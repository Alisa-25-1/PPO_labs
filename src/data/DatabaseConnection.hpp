#ifndef DATABASECONNECTION_HPP
#define DATABASECONNECTION_HPP

#include <pqxx/pqxx>
#include <memory>
#include <string>

class DatabaseConnection {
public:
    explicit DatabaseConnection(const std::string& connectionString);
    ~DatabaseConnection();

    // Запрещаем копирование
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    // Получение соединения
    pqxx::connection& getConnection();
    bool isConnected() const;

    // Транзакции
    pqxx::work beginTransaction();
    void commitTransaction(pqxx::work& transaction);
    void rollbackTransaction(pqxx::work& transaction);

private:
    std::unique_ptr<pqxx::connection> connection_;
    std::string connectionString_;
};

#endif // DATABASECONNECTION_HPP