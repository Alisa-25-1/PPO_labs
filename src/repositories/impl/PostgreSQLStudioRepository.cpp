#include "PostgreSQLStudioRepository.hpp"
#include <pqxx/pqxx>

PostgreSQLStudioRepository::PostgreSQLStudioRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Studio> PostgreSQLStudioRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, description, contact_email "
            "FROM studios WHERE id = $1";
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto studio = mapResultToStudio(result[0]);
        dbConnection_->commitTransaction(work);
        return studio;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find studio by ID: ") + e.what());
    }
}

std::optional<Studio> PostgreSQLStudioRepository::findMainStudio() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        // Предполагаем, что основная студия - первая в таблице
        std::string query = 
            "SELECT id, name, description, contact_email "
            "FROM studios ORDER BY id LIMIT 1";
        
        auto result = work.exec(query);
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto studio = mapResultToStudio(result[0]);
        dbConnection_->commitTransaction(work);
        return studio;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find main studio: ") + e.what());
    }
}

std::vector<Studio> PostgreSQLStudioRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, description, contact_email "
            "FROM studios";
        
        auto result = work.exec(query);
        
        std::vector<Studio> studios;
        for (const auto& row : result) {
            studios.push_back(mapResultToStudio(row));
        }
        
        dbConnection_->commitTransaction(work);
        return studios;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all studios: ") + e.what());
    }
}

bool PostgreSQLStudioRepository::save(const Studio& studio) {
    validateStudio(studio);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "INSERT INTO studios (id, name, description, contact_email) "
            "VALUES ($1, $2, $3, $4)";
        
        work.exec_params(
            query,
            studio.getId().toString(),
            studio.getName(),
            studio.getDescription(),
            studio.getContactEmail()
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save studio: ") + e.what());
    }
}

bool PostgreSQLStudioRepository::update(const Studio& studio) {
    validateStudio(studio);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "UPDATE studios SET name = $2, description = $3, contact_email = $4 "
            "WHERE id = $1";
        
        auto result = work.exec_params(
            query,
            studio.getId().toString(),
            studio.getName(),
            studio.getDescription(),
            studio.getContactEmail()
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update studio: ") + e.what());
    }
}

bool PostgreSQLStudioRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "DELETE FROM studios WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove studio: ") + e.what());
    }
}

bool PostgreSQLStudioRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "SELECT 1 FROM studios WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check studio existence: ") + e.what());
    }
}

Studio PostgreSQLStudioRepository::mapResultToStudio(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::string description = row["description"].c_str();
    std::string contactEmail = row["contact_email"].c_str();
    
    Studio studio(id, name, contactEmail);
    studio.setDescription(description);
    return studio;
}

void PostgreSQLStudioRepository::validateStudio(const Studio& studio) const {
    if (!studio.isValid()) {
        throw DataAccessException("Invalid studio data");
    }
}