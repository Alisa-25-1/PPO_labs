#include "PostgreSQLClientRepository.hpp"
#include <pqxx/pqxx>
#include "../../data/DateTimeUtils.hpp"

PostgreSQLClientRepository::PostgreSQLClientRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Client> PostgreSQLClientRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, email, phone, password_hash, registration_date, status "
            "FROM clients WHERE id = $1";
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto client = mapResultToClient(result[0]);
        dbConnection_->commitTransaction(work);
        return client;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find client by ID: ") + e.what());
    }
}

std::optional<Client> PostgreSQLClientRepository::findByEmail(const std::string& email) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, email, phone, password_hash, registration_date, status "
            "FROM clients WHERE email = $1";
        
        auto result = work.exec_params(query, email);
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto client = mapResultToClient(result[0]);
        dbConnection_->commitTransaction(work);
        return client;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find client by email: ") + e.what());
    }
}

bool PostgreSQLClientRepository::save(const Client& client) {
    validateClient(client);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "INSERT INTO clients (id, name, email, phone, password_hash, registration_date, status) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7)";
        
        work.exec_params(
            query,
            client.getId().toString(),
            client.getName(),
            client.getEmail(),
            client.getPhone(),
            "hashed_password", // В реальном приложении нужно хэшировать
            DateTimeUtils::formatTimeForPostgres(client.getRegistrationDate()),
            "ACTIVE" // Упрощенно
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save client: ") + e.what());
    }
}

bool PostgreSQLClientRepository::update(const Client& client) {
    validateClient(client);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "UPDATE clients SET name = $2, email = $3, phone = $4, status = $5 "
            "WHERE id = $1";
        
        auto result = work.exec_params(
            query,
            client.getId().toString(),
            client.getName(),
            client.getEmail(),
            client.getPhone(),
            "ACTIVE" // Упрощенно
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update client: ") + e.what());
    }
}

bool PostgreSQLClientRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "DELETE FROM clients WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove client: ") + e.what());
    }
}

bool PostgreSQLClientRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "SELECT 1 FROM clients WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check client existence: ") + e.what());
    }
}

Client PostgreSQLClientRepository::mapResultToClient(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::string email = row["email"].c_str();
    std::string phone = row["phone"].c_str();
    
    return Client(id, name, email, phone);
}

void PostgreSQLClientRepository::validateClient(const Client& client) const {
    if (!client.isValid()) {
        throw DataAccessException("Invalid client data");
    }
}