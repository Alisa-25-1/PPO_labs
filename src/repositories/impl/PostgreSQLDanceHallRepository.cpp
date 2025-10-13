#include "PostgreSQLDanceHallRepository.hpp"
#include <pqxx/pqxx>
#include "../../data/SqlQueryBuilder.hpp"

PostgreSQLDanceHallRepository::PostgreSQLDanceHallRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<DanceHall> PostgreSQLDanceHallRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "description", "capacity", "floor_type", "equipment", "branch_id"})
            .from("dance_halls")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto hall = mapResultToDanceHall(result[0]);
        dbConnection_->commitTransaction(work);
        return hall;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find hall by ID: ") + e.what());
    }
}

std::vector<DanceHall> PostgreSQLDanceHallRepository::findByBranchId(const UUID& branchId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "description", "capacity", "floor_type", "equipment", "branch_id"})
            .from("dance_halls")
            .where("branch_id = $1")
            .build();
        
        auto result = work.exec_params(query, branchId.toString());
        
        std::vector<DanceHall> halls;
        for (const auto& row : result) {
            halls.push_back(mapResultToDanceHall(row));
        }
        
        dbConnection_->commitTransaction(work);
        return halls;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find halls by branch ID: ") + e.what());
    }
}

bool PostgreSQLDanceHallRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("dance_halls")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check hall existence: ") + e.what());
    }
}

std::vector<DanceHall> PostgreSQLDanceHallRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "description", "capacity", "floor_type", "equipment", "branch_id"})
            .from("dance_halls")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<DanceHall> halls;
        for (const auto& row : result) {
            halls.push_back(mapResultToDanceHall(row));
        }
        
        dbConnection_->commitTransaction(work);
        return halls;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all halls: ") + e.what());
    }
}

bool PostgreSQLDanceHallRepository::save(const DanceHall& hall) {
    validateDanceHall(hall);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"name", "$2"},
            {"description", "$3"},
            {"capacity", "$4"},
            {"floor_type", "$5"},
            {"equipment", "$6"},
            {"branch_id", "$7"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("dance_halls")
            .values(values)
            .build();
        
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

bool PostgreSQLDanceHallRepository::update(const DanceHall& hall) {
    validateDanceHall(hall);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"name", "$2"},
            {"description", "$3"},
            {"capacity", "$4"},
            {"floor_type", "$5"},
            {"equipment", "$6"},
            {"branch_id", "$7"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("dance_halls")
            .set(values)
            .where("id = $1")
            .build();
        
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

bool PostgreSQLDanceHallRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("dance_halls")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove hall: ") + e.what());
    }
}

DanceHall PostgreSQLDanceHallRepository::mapResultToDanceHall(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::string description = row["description"].c_str();
    int capacity = row["capacity"].as<int>();
    std::string floorType = row["floor_type"].c_str();
    std::string equipment = row["equipment"].c_str();
    UUID branchId = UUID::fromString(row["branch_id"].c_str());
    
    DanceHall hall(id, name, capacity, branchId);
    hall.setDescription(description);
    hall.setFloorType(floorType);
    hall.setEquipment(equipment);
    
    return hall;
}

void PostgreSQLDanceHallRepository::validateDanceHall(const DanceHall& hall) const {
    if (!hall.isValid()) {
        throw DataAccessException("Invalid hall data");
    }
}