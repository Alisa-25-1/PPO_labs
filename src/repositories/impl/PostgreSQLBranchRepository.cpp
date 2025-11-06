#include "PostgreSQLBranchRepository.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include "../../data/DateTimeUtils.hpp"

PostgreSQLBranchRepository::PostgreSQLBranchRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Branch> PostgreSQLBranchRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        // Сначала получаем основные данные филиала
        SqlQueryBuilder branchQueryBuilder;
        std::string branchQuery = branchQueryBuilder
            .select({"id", "name", "phone", "open_time", "close_time", "studio_id", "address_id"})
            .from("branches")
            .where("id = $1")
            .build();
        
        auto branchResult = work.exec_params(branchQuery, id.toString());
        
        if (branchResult.empty()) {
            return std::nullopt;
        }
        
        // Получаем адрес
        UUID addressId = UUID::fromString(branchResult[0]["address_id"].c_str());
        auto address = findAddressById(addressId, work);
        if (!address) {
            throw DataAccessException("Address not found for branch");
        }
        
        auto branch = mapResultToBranch(branchResult[0], *address);
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
            .select({"id", "name", "phone", "open_time", "close_time", "studio_id", "address_id"})
            .from("branches")
            .where("studio_id = $1")
            .build();
        
        auto result = work.exec_params(query, studioId.toString());
        
        std::vector<Branch> branches;
        for (const auto& row : result) {
            UUID addressId = UUID::fromString(row["address_id"].c_str());
            auto address = findAddressById(addressId, work);
            if (address) {
                branches.push_back(mapResultToBranch(row, *address));
            }
        }
        
        dbConnection_->commitTransaction(work);
        return branches;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find branches by studio ID: ") + e.what());
    }
}

std::optional<BranchAddress> PostgreSQLBranchRepository::findAddressById(const UUID& addressId, pqxx::work& work) {
    try {
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "country", "city", "street", "building", "apartment", "postal_code", "timezone_offset"})
            .from("addresses")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, addressId.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        return mapResultToAddress(result[0]);
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find address by ID: ") + e.what());
    }
}

std::vector<Branch> PostgreSQLBranchRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "phone", "open_time", "close_time", "studio_id", "address_id"})
            .from("branches")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Branch> branches;
        for (const auto& row : result) {
            UUID addressId = UUID::fromString(row["address_id"].c_str());
            auto address = findAddressById(addressId, work);
            if (address) {
                branches.push_back(mapResultToBranch(row, *address));
            }
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
        
        // Сначала сохраняем адрес
        if (!saveAddress(branch.getAddress(), work)) {
            throw DataAccessException("Failed to save address");
        }
        
        // Затем сохраняем филиал
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"name", "$2"},
            {"phone", "$3"},
            {"open_time", "$4"},
            {"close_time", "$5"},
            {"studio_id", "$6"},
            {"address_id", "$7"}
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
            branch.getPhone(),
            std::to_string(static_cast<int>(branch.getWorkingHours().openTime.count())),
            std::to_string(static_cast<int>(branch.getWorkingHours().closeTime.count())),
            branch.getStudioId().toString(),
            branch.getAddress().getId().toString()
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
        
        // Обновляем адрес
        if (!updateAddress(branch.getAddress(), work)) {
            throw DataAccessException("Failed to update address");
        }
        
        // Обновляем филиал
        std::map<std::string, std::string> values = {
            {"name", "$2"},
            {"phone", "$3"},
            {"open_time", "$4"},
            {"close_time", "$5"},
            {"studio_id", "$6"}
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
        
        // Сначала находим филиал чтобы получить address_id
        auto branch = findById(id);
        if (!branch) {
            return false;
        }
        
        // Удаляем филиал
        SqlQueryBuilder branchQueryBuilder;
        std::string branchQuery = branchQueryBuilder
            .deleteFrom("branches")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(branchQuery, id.toString());
        
        // Удаляем адрес (если больше никто не использует)
        // TODO: Добавить проверку использования адреса другими сущностями
        SqlQueryBuilder addressQueryBuilder;
        std::string addressQuery = addressQueryBuilder
            .deleteFrom("addresses")
            .where("id = $1")
            .build();
            
        work.exec_params(addressQuery, branch->getAddress().getId().toString());
        
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

Branch PostgreSQLBranchRepository::mapResultToBranch(const pqxx::row& row, const BranchAddress& address) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::string phone = row["phone"].c_str();
    
    auto openTime = std::chrono::hours(row["open_time"].as<int>());
    auto closeTime = std::chrono::hours(row["close_time"].as<int>());
    WorkingHours workingHours(openTime, closeTime);
    
    UUID studioId = UUID::fromString(row["studio_id"].c_str());
    
    return Branch(id, name, phone, workingHours, studioId, address);
}

BranchAddress PostgreSQLBranchRepository::mapResultToAddress(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string country = row["country"].c_str();
    std::string city = row["city"].c_str();
    std::string street = row["street"].c_str();
    std::string building = row["building"].c_str();
    std::string apartment = row["apartment"].c_str();
    std::string postalCode = row["postal_code"].c_str();
    auto timezoneOffset = std::chrono::minutes(row["timezone_offset"].as<int>());
    
    BranchAddress address(id, country, city, street, building, timezoneOffset);
    if (!apartment.empty()) {
        address.setApartment(apartment);
    }
    if (!postalCode.empty()) {
        address.setPostalCode(postalCode);
    }
    
    return address;
}

void PostgreSQLBranchRepository::validateBranch(const Branch& branch) const {
    if (!branch.isValid()) {
        throw DataAccessException("Invalid branch data");
    }
}

bool PostgreSQLBranchRepository::saveAddress(const BranchAddress& address, pqxx::work& work) {
    try {
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"country", "$2"},
            {"city", "$3"},
            {"street", "$4"},
            {"building", "$5"},
            {"apartment", "$6"},
            {"postal_code", "$7"},
            {"timezone_offset", "$8"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("addresses")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            address.getId().toString(),
            address.getCountry(),
            address.getCity(),
            address.getStreet(),
            address.getBuilding(),
            address.getApartment(),
            address.getPostalCode(),
            std::to_string(static_cast<int>(address.getTimezoneOffset().count()))
        );
        
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save address: ") + e.what());
    }
}

bool PostgreSQLBranchRepository::updateAddress(const BranchAddress& address, pqxx::work& work) {
    try {
        std::map<std::string, std::string> values = {
            {"country", "$2"},
            {"city", "$3"},
            {"street", "$4"},
            {"building", "$5"},
            {"apartment", "$6"},
            {"postal_code", "$7"},
            {"timezone_offset", "$8"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("addresses")
            .set(values)
            .where("id = $1")
            .build();
        
        work.exec_params(
            query,
            address.getId().toString(),
            address.getCountry(),
            address.getCity(),
            address.getStreet(),
            address.getBuilding(),
            address.getApartment(),
            address.getPostalCode(),
            std::to_string(static_cast<int>(address.getTimezoneOffset().count()))
        );
        
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update address: ") + e.what());
    }
}