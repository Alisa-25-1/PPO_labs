#include "ResilientDatabaseConnection.hpp"
#include "../services/DatabaseHealthService.hpp"
#include <iostream>

ResilientDatabaseConnection::ResilientDatabaseConnection(const std::string& connectionString)
    : DatabaseConnection(connectionString) {
    setRetryPolicy(3, std::chrono::milliseconds(2000));
}

void ResilientDatabaseConnection::setRetryPolicy(int maxRetries, std::chrono::milliseconds retryDelay) {
    maxRetries_ = maxRetries;
    retryDelay_ = retryDelay;
}

pqxx::connection& ResilientDatabaseConnection::getConnection() {
    // Проверяем глобальное состояние БД
    if (!DatabaseHealthService::shouldRetryConnection()) {
        throw std::runtime_error("Database is marked as unhealthy. Please wait before retrying.");
    }
    
    for (int attempt = 1; attempt <= maxRetries_; ++attempt) {
        try {
            auto& conn = DatabaseConnection::getConnection();
            
            // Проверяем, что соединение действительно работает
            pqxx::work testWork(conn);
            testWork.exec("SELECT 1");
            testWork.commit();
            
            DatabaseHealthService::markDatabaseHealthy();
            successCount_++;
            return conn;
            
        } catch (const std::exception& e) {
            retryCount_++;
            std::cerr << "⚠️ Connection attempt " << attempt << "/" << maxRetries_ 
                      << " failed: " << e.what() << std::endl;
            
            if (attempt == maxRetries_) {
                // Последняя попытка неудачна - помечаем БД как нездоровую
                DatabaseHealthService::markDatabaseUnhealthy();
                std::cerr << "💥 All connection attempts failed. Marking database as unhealthy." << std::endl;
                throw;
            } else {
                std::cout << "🔄 Retrying in " << retryDelay_.count() << "ms..." << std::endl;
                std::this_thread::sleep_for(retryDelay_);
            }
        }
    }
    
    throw std::runtime_error("Failed to establish database connection after " + 
                           std::to_string(maxRetries_) + " attempts");
}

pqxx::work ResilientDatabaseConnection::beginTransaction() {
    // Используем ту же логику ретраев, но для транзакций
    for (int attempt = 1; attempt <= maxRetries_; ++attempt) {
        try {
            return DatabaseConnection::beginTransaction(); 
        } catch (const std::exception& e) {
            std::cerr << "⚠️ Transaction attempt " << attempt << "/" << maxRetries_ 
                      << " failed: " << e.what() << std::endl;
            
            if (attempt < maxRetries_) {
                std::cout << "🔄 Retrying transaction in " << retryDelay_.count() << "ms..." << std::endl;
                std::this_thread::sleep_for(retryDelay_);
            } else {
                DatabaseHealthService::markDatabaseUnhealthy();
                throw;
            }
        }
    }
    
    throw std::runtime_error("Failed to begin transaction after " + 
                           std::to_string(maxRetries_) + " attempts");
}

bool ResilientDatabaseConnection::executeWithRetry(const std::function<void()>& operation) {
    if (!DatabaseHealthService::shouldRetryConnection()) {
        return false;
    }
    
    for (int attempt = 1; attempt <= maxRetries_; ++attempt) {
        try {
            operation();
            DatabaseHealthService::markDatabaseHealthy();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "⚠️ Operation attempt " << attempt << "/" << maxRetries_ 
                      << " failed: " << e.what() << std::endl;
            
            if (attempt == maxRetries_) {
                DatabaseHealthService::markDatabaseUnhealthy();
                return false;
            } else {
                std::cout << "🔄 Retrying operation in " << retryDelay_.count() << "ms..." << std::endl;
                std::this_thread::sleep_for(retryDelay_);
            }
        }
    }
    return false;
}