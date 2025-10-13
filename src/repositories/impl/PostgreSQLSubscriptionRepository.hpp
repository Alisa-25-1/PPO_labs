#ifndef POSTGRESQL_SUBSCRIPTION_REPOSITORY_HPP
#define POSTGRESQL_SUBSCRIPTION_REPOSITORY_HPP

#include "../ISubscriptionRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include <memory>

class PostgreSQLSubscriptionRepository : public ISubscriptionRepository {
public:
    explicit PostgreSQLSubscriptionRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Subscription> findById(const UUID& id) override;
    std::vector<Subscription> findByClientId(const UUID& clientId) override;
    std::vector<Subscription> findActiveSubscriptions() override;
    std::vector<Subscription> findExpiringSubscriptions(int days = 7) override;
    bool save(const Subscription& subscription) override;
    bool update(const Subscription& subscription) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Subscription mapResultToSubscription(const pqxx::row& row) const;
    void validateSubscription(const Subscription& subscription) const;
    std::string subscriptionStatusToString(SubscriptionStatus status) const;
    SubscriptionStatus stringToSubscriptionStatus(const std::string& status) const;
};

#endif // POSTGRESQL_SUBSCRIPTION_REPOSITORY_HPP