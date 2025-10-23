#ifndef RESILIENTDATABASECONNECTION_HPP
#define RESILIENTDATABASECONNECTION_HPP

#include "DatabaseConnection.hpp"
#include <chrono>
#include <thread>
#include <atomic>

class ResilientDatabaseConnection : public DatabaseConnection {
public:
    explicit ResilientDatabaseConnection(const std::string& connectionString);
    ~ResilientDatabaseConnection() override = default;

    // Политика повторных попыток
    void setRetryPolicy(int maxRetries, std::chrono::milliseconds retryDelay);
    
    // Переопределяем методы с устойчивостью к ошибкам
    pqxx::connection& getConnection() override;
    pqxx::work beginTransaction() override;
    
    // Дополнительные методы для мониторинга
    int getRetryCount() const { return retryCount_; }
    int getSuccessCount() const { return successCount_; }

private:
    int maxRetries_ = 3;
    std::chrono::milliseconds retryDelay_ = std::chrono::milliseconds(1000);
    std::atomic<int> retryCount_{0};
    std::atomic<int> successCount_{0};
    
    bool executeWithRetry(const std::function<void()>& operation);
};

#endif // RESILIENTDATABASECONNECTION_HPP