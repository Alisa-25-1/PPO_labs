#include "PostgreSQLSubscriptionTypeRepository.hpp"
#include <pqxx/pqxx>

PostgreSQLSubscriptionTypeRepository::PostgreSQLSubscriptionTypeRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<SubscriptionType> PostgreSQLSubscriptionTypeRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, description, validity_days, visit_count, unlimited, price "
            "FROM subscription_types WHERE id = $1";
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto subscriptionType = mapResultToSubscriptionType(result[0]);
        dbConnection_->commitTransaction(work);
        return subscriptionType;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find subscription type by ID: ") + e.what());
    }
}

std::vector<SubscriptionType> PostgreSQLSubscriptionTypeRepository::findAllActive() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, description, validity_days, visit_count, unlimited, price "
            "FROM subscription_types WHERE unlimited = true OR visit_count > 0";
        
        auto result = work.exec(query);
        
        std::vector<SubscriptionType> subscriptionTypes;
        for (const auto& row : result) {
            subscriptionTypes.push_back(mapResultToSubscriptionType(row));
        }
        
        dbConnection_->commitTransaction(work);
        return subscriptionTypes;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find active subscription types: ") + e.what());
    }
}

std::vector<SubscriptionType> PostgreSQLSubscriptionTypeRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, name, description, validity_days, visit_count, unlimited, price "
            "FROM subscription_types";
        
        auto result = work.exec(query);
        
        std::vector<SubscriptionType> subscriptionTypes;
        for (const auto& row : result) {
            subscriptionTypes.push_back(mapResultToSubscriptionType(row));
        }
        
        dbConnection_->commitTransaction(work);
        return subscriptionTypes;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all subscription types: ") + e.what());
    }
}

bool PostgreSQLSubscriptionTypeRepository::save(const SubscriptionType& subscriptionType) {
    validateSubscriptionType(subscriptionType);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "INSERT INTO subscription_types (id, name, description, validity_days, visit_count, unlimited, price) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7)";
        
        work.exec_params(
            query,
            subscriptionType.getId().toString(),
            subscriptionType.getName(),
            subscriptionType.getDescription(),
            subscriptionType.getValidityDays(),
            subscriptionType.getVisitCount(),
            subscriptionType.isUnlimited(),
            subscriptionType.getPrice()
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save subscription type: ") + e.what());
    }
}

bool PostgreSQLSubscriptionTypeRepository::update(const SubscriptionType& subscriptionType) {
    validateSubscriptionType(subscriptionType);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "UPDATE subscription_types SET name = $2, description = $3, validity_days = $4, "
            "visit_count = $5, unlimited = $6, price = $7 "
            "WHERE id = $1";
        
        auto result = work.exec_params(
            query,
            subscriptionType.getId().toString(),
            subscriptionType.getName(),
            subscriptionType.getDescription(),
            subscriptionType.getValidityDays(),
            subscriptionType.getVisitCount(),
            subscriptionType.isUnlimited(),
            subscriptionType.getPrice()
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update subscription type: ") + e.what());
    }
}

bool PostgreSQLSubscriptionTypeRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "DELETE FROM subscription_types WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove subscription type: ") + e.what());
    }
}

bool PostgreSQLSubscriptionTypeRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "SELECT 1 FROM subscription_types WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check subscription type existence: ") + e.what());
    }
}

SubscriptionType PostgreSQLSubscriptionTypeRepository::mapResultToSubscriptionType(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::string description = row["description"].c_str();
    int validityDays = row["validity_days"].as<int>();
    int visitCount = row["visit_count"].as<int>();
    bool unlimited = row["unlimited"].as<bool>();
    double price = row["price"].as<double>();
    
    SubscriptionType subscriptionType(id, name, validityDays, visitCount, unlimited, price);
    subscriptionType.setDescription(description);
    return subscriptionType;
}

void PostgreSQLSubscriptionTypeRepository::validateSubscriptionType(const SubscriptionType& subscriptionType) const {
    if (!subscriptionType.isValid()) {
        throw DataAccessException("Invalid subscription type data");
    }
}