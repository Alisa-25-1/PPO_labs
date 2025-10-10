#include "PostgreSQLHallRepository.hpp"
#include <pqxx/pqxx>

PostgreSQLHallRepository::PostgreSQLHallRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Hall> PostgreSQLHallRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, description, capacity, floor_type, equipment, branch_id "
            "FROM halls WHERE id = $1";
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto hall = mapResultToHall(result[0]);
        dbConnection_->commitTransaction(work);
        return hall;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find hall by ID: ") + e.what());
    }
}

std::vector<Hall> PostgreSQLHallRepository::findByBranchId(const UUID& branchId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, description, capacity, floor_type, equipment, branch_id "
            "FROM halls WHERE branch_id = $1";
        
        auto result = work.exec_params(query, branchId.toString());
        
        std::vector<Hall> halls;
        for (const auto& row : result) {
            halls.push_back(mapResultToHall(row));
        }
        
        dbConnection_->commitTransaction(work);
        return halls;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find halls by branch ID: ") + e.what());
    }
}

bool PostgreSQLHallRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "SELECT 1 FROM halls WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check hall existence: ") + e.what());
    }
}

std::vector<Hall> PostgreSQLHallRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, description, capacity, floor_type, equipment, branch_id "
            "FROM halls";
        
        auto result = work.exec(query);
        
        std::vector<Hall> halls;
        for (const auto& row : result) {
            halls.push_back(mapResultToHall(row));
        }
        
        dbConnection_->commitTransaction(work);
        return halls;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all halls: ") + e.what());
    }
}

Hall PostgreSQLHallRepository::mapResultToHall(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::string description = row["description"].c_str();
    int capacity = row["capacity"].as<int>();
    std::string floorType = row["floor_type"].c_str();
    std::string equipment = row["equipment"].c_str();
    UUID branchId = UUID::fromString(row["branch_id"].c_str());
    
    Hall hall(id, name, capacity, branchId);
    hall.setDescription(description);
    hall.setFloorType(floorType);
    hall.setEquipment(equipment);
    
    return hall;
}

void PostgreSQLHallRepository::validateHall(const Hall& hall) const {
    if (!hall.isValid()) {
        throw DataAccessException("Invalid hall data");
    }
}

bool PostgreSQLHallRepository::save(const Hall& hall) {
    validateHall(hall);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "INSERT INTO halls (id, name, description, capacity, floor_type, equipment, branch_id) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7)";
        
        work.exec_params(
            query,
            hall.getId().toString(),
            hall.getName(),
            hall.getDescription(),
            hall.getCapacity(),
            hall.getFloorType(),
            hall.getEquipment(),
            hall.getBranchId().toString()
        );

             
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save hall: ") + e.what());
    }
}

bool PostgreSQLHallRepository::update(const Hall& hall) {
    validateHall(hall);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "UPDATE halls SET name = $2, description = $3, capacity = $4, "
            "floor_type = $5, equipment = $6, branch_id = $7 "
            "WHERE id = $1";
        
        auto result = work.exec_params(
            query,
            hall.getId().toString(),
            hall.getName(),
            hall.getDescription(),
            hall.getCapacity(),
            hall.getFloorType(),
            hall.getEquipment(),
            hall.getBranchId().toString()
        );

        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update hall: ") + e.what());
    }
}