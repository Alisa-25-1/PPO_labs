#include "DatabaseConnection.hpp"
#include <stdexcept>

DatabaseConnection::DatabaseConnection(const std::string& connectionString) 
    : connectionString_(connectionString) {
    try {
        connection_ = std::make_unique<pqxx::connection>(connectionString_);
        if (!connection_->is_open()) {
            throw std::runtime_error("Failed to connect to database");
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Database connection failed: ") + e.what());
    }
}

DatabaseConnection::~DatabaseConnection() {
    if (connection_ && connection_->is_open()) {
        connection_->close();
    }
}

pqxx::connection& DatabaseConnection::getConnection() {
    if (!connection_ || !connection_->is_open()) {
        try {
            connection_ = std::make_unique<pqxx::connection>(connectionString_);
            if (!connection_->is_open()) {
                throw std::runtime_error("Failed to reconnect to database");
            }
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Database reconnection failed: ") + e.what());
        }
    }
    return *connection_;
}

bool DatabaseConnection::isConnected() const {
    return connection_ && connection_->is_open();
}

pqxx::work DatabaseConnection::beginTransaction() {
    return pqxx::work(getConnection()); 
}

void DatabaseConnection::commitTransaction(pqxx::work& transaction) {
    transaction.commit();
}

void DatabaseConnection::rollbackTransaction(pqxx::work& transaction) {
    transaction.abort();
}