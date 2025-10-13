#include "PostgreSQLBranchRepository.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include "../../data/DateTimeUtils.hpp"

PostgreSQLBranchRepository::PostgreSQLBranchRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Branch> PostgreSQLBranchRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "address", "phone", "open_time", "close_time", "studio_id"})
            .from("branches")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto branch = mapResultToBranch(result[0]);
        dbConnection_->commitTransaction(work);
        return branch;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find branch by ID: ") + e.what());
    }
}

std::vector<Branch> PostgreSQLBranchRepository::findByStudioId(const UUID& studioId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "address", "phone", "open_time", "close_time", "studio_id"})
            .from("branches")
            .where("studio_id = $1")
            .build();
        
        auto result = work.exec_params(query, studioId.toString());
        
        std::vector<Branch> branches;
        for (const auto& row : result) {
            branches.push_back(mapResultToBranch(row));
        }
        
        dbConnection_->commitTransaction(work);
        return branches;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find branches by studio ID: ") + e.what());
    }
}

std::vector<Branch> PostgreSQLBranchRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "address", "phone", "open_time", "close_time", "studio_id"})
            .from("branches")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Branch> branches;
        for (const auto& row : result) {
            branches.push_back(mapResultToBranch(row));
        }
        
        dbConnection_->commitTransaction(work);
        return branches;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all branches: ") + e.what());
    }
}

bool PostgreSQLBranchRepository::save(const Branch& branch) {
    validateBranch(branch);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"name", "$2"},
            {"address", "$3"},
            {"phone", "$4"},
            {"open_time", "$5"},
            {"close_time", "$6"},
            {"studio_id", "$7"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("branches")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            branch.getId().toString(),
            branch.getName(),
            branch.getAddress(),
            branch.getPhone(),
            std::to_string(static_cast<int>(branch.getWorkingHours().openTime.count())),
            std::to_string(static_cast<int>(branch.getWorkingHours().closeTime.count())),
            branch.getStudioId().toString()
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save branch: ") + e.what());
    }
}

bool PostgreSQLBranchRepository::update(const Branch& branch) {
    validateBranch(branch);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"name", "$2"},
            {"address", "$3"},
            {"phone", "$4"},
            {"open_time", "$5"},
            {"close_time", "$6"},
            {"studio_id", "$7"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("branches")
            .set(values)
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(
            query,
            branch.getId().toString(),
            branch.getName(),
            branch.getAddress(),
            branch.getPhone(),
            std::to_string(static_cast<int>(branch.getWorkingHours().openTime.count())),
            std::to_string(static_cast<int>(branch.getWorkingHours().closeTime.count())),
            branch.getStudioId().toString()
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update branch: ") + e.what());
    }
}

bool PostgreSQLBranchRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("branches")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove branch: ") + e.what());
    }
}

bool PostgreSQLBranchRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("branches")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check branch existence: ") + e.what());
    }
}

Branch PostgreSQLBranchRepository::mapResultToBranch(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::string address = row["address"].c_str();
    std::string phone = row["phone"].c_str();
    
    auto openTime = std::chrono::hours(row["open_time"].as<int>());
    auto closeTime = std::chrono::hours(row["close_time"].as<int>());
    WorkingHours workingHours(openTime, closeTime);
    
    UUID studioId = UUID::fromString(row["studio_id"].c_str());
    
    return Branch(id, name, address, phone, workingHours, studioId);
}

void PostgreSQLBranchRepository::validateBranch(const Branch& branch) const {
    if (!branch.isValid()) {
        throw DataAccessException("Invalid branch data");
    }
}