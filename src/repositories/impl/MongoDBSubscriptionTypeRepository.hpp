#ifndef MONGODB_SUBSCRIPTION_TYPE_REPOSITORY_HPP
#define MONGODB_SUBSCRIPTION_TYPE_REPOSITORY_HPP

#include "../ISubscriptionTypeRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

// Предварительное объявление
class MongoDBRepositoryFactory;

class MongoDBSubscriptionTypeRepository : public ISubscriptionTypeRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBSubscriptionTypeRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<SubscriptionType> findById(const UUID& id) override;
    std::vector<SubscriptionType> findAllActive() override;
    std::vector<SubscriptionType> findAll() override;
    bool save(const SubscriptionType& subscriptionType) override;
    bool update(const SubscriptionType& subscriptionType) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    SubscriptionType mapDocumentToSubscriptionType(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapSubscriptionTypeToDocument(const SubscriptionType& subscriptionType) const;
    void validateSubscriptionType(const SubscriptionType& subscriptionType) const;
};

#endif // MONGODB_SUBSCRIPTION_TYPE_REPOSITORY_HPP