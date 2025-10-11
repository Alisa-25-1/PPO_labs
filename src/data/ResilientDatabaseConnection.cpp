#include "ResilientDatabaseConnection.hpp"
#include <iostream>
#include <stdexcept>

ResilientDatabaseConnection::ResilientDatabaseConnection(const std::string& connectionString)
    : DatabaseConnection(connectionString) {}

void ResilientDatabaseConnection::setRetryPolicy(int maxRetries, std::chrono::milliseconds retryDelay) {
    maxRetries_ = maxRetries;
    retryDelay_ = retryDelay;
}

pqxx::connection& ResilientDatabaseConnection::getConnection() {
    retryCount_ = 0;
    
    for (int attempt = 1; attempt <= maxRetries_; ++attempt) {
        try {
            auto& conn = DatabaseConnection::getConnection();
            successCount_++;
            return conn;
        } catch (const std::exception& e) {
            retryCount_++;
            std::cerr << "⚠️ Connection attempt " << attempt << "/" << maxRetries_ 
                      << " failed: " << e.what() << std::endl;
            
            if (attempt < maxRetries_) {
                std::cout << "🔄 Retrying in " << retryDelay_.count() << "ms..." << std::endl;
                std::this_thread::sleep_for(retryDelay_);
            } else {
                std::cerr << "💥 All connection attempts failed" << std::endl;
                throw;
            }
        }
    }
    
    throw std::runtime_error("Failed to establish database connection after " + 
                           std::to_string(maxRetries_) + " attempts");
}

pqxx::work ResilientDatabaseConnection::beginTransaction() {
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
                throw;
            }
        }
    }
    
    throw std::runtime_error("Failed to begin transaction after " + 
                           std::to_string(maxRetries_) + " attempts");
}

bool ResilientDatabaseConnection::executeWithRetry(const std::function<void()>& operation) {
    for (int attempt = 1; attempt <= maxRetries_; ++attempt) {
        try {
            operation();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "⚠️ Operation attempt " << attempt << "/" << maxRetries_ 
                      << " failed: " << e.what() << std::endl;
            
            if (attempt < maxRetries_) {
                std::cout << "🔄 Retrying operation in " << retryDelay_.count() << "ms..." << std::endl;
                std::this_thread::sleep_for(retryDelay_);
            } else {
                return false;
            }
        }
    }
    return false;
}