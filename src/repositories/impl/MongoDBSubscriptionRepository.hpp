#ifndef MONGODB_SUBSCRIPTION_REPOSITORY_HPP
#define MONGODB_SUBSCRIPTION_REPOSITORY_HPP

#include "../ISubscriptionRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

// Предварительное объявление
class MongoDBRepositoryFactory;

class MongoDBSubscriptionRepository : public ISubscriptionRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBSubscriptionRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Subscription> findById(const UUID& id) override;
    std::vector<Subscription> findByClientId(const UUID& clientId) override;
    std::vector<Subscription> findActiveSubscriptions() override;
    std::vector<Subscription> findExpiringSubscriptions(int days = 7) override;
    bool save(const Subscription& subscription) override;
    bool update(const Subscription& subscription) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    Subscription mapDocumentToSubscription(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapSubscriptionToDocument(const Subscription& subscription) const;
    void validateSubscription(const Subscription& subscription) const;
    std::string subscriptionStatusToString(SubscriptionStatus status) const;
    SubscriptionStatus stringToSubscriptionStatus(const std::string& status) const;
};

#endif // MONGODB_SUBSCRIPTION_REPOSITORY_HPP