#ifndef POSTGRESQL_SUBSCRIPTION_TYPE_REPOSITORY_HPP
#define POSTGRESQL_SUBSCRIPTION_TYPE_REPOSITORY_HPP

#include "../ISubscriptionTypeRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <memory>

class PostgreSQLSubscriptionTypeRepository : public ISubscriptionTypeRepository {
public:
    explicit PostgreSQLSubscriptionTypeRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<SubscriptionType> findById(const UUID& id) override;
    std::vector<SubscriptionType> findAllActive() override;
    std::vector<SubscriptionType> findAll() override;
    bool save(const SubscriptionType& subscriptionType) override;
    bool update(const SubscriptionType& subscriptionType) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    SubscriptionType mapResultToSubscriptionType(const pqxx::row& row) const;
    void validateSubscriptionType(const SubscriptionType& subscriptionType) const;
};

#endif // POSTGRESQL_SUBSCRIPTION_TYPE_REPOSITORY_HPP