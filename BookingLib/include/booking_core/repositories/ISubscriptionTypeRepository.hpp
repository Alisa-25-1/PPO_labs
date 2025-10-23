#pragma once
#include "../types/uuid.hpp"
#include "../models/SubscriptionType.hpp"
#include <memory>
#include <optional>
#include <vector>

class ISubscriptionTypeRepository {
public:
    virtual ~ISubscriptionTypeRepository() = default;
    
    virtual std::optional<SubscriptionType> findById(const UUID& id) = 0;
    virtual std::vector<SubscriptionType> findAllActive() = 0;
    virtual std::vector<SubscriptionType> findAll() = 0;
    virtual bool save(const SubscriptionType& subscriptionType) = 0;
    virtual bool update(const SubscriptionType& subscriptionType) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};