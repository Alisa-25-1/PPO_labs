#pragma once
#include "../types/uuid.hpp"
#include "../models/Subscription.hpp"
#include <memory>
#include <optional>
#include <vector>

class ISubscriptionRepository {
public:
    virtual ~ISubscriptionRepository() = default;
    
    virtual std::optional<Subscription> findById(const UUID& id) = 0;
    virtual std::vector<Subscription> findByClientId(const UUID& clientId) = 0;
    virtual std::vector<Subscription> findActiveSubscriptions() = 0;
    virtual std::vector<Subscription> findExpiringSubscriptions(int days = 7) = 0;
    virtual bool save(const Subscription& subscription) = 0;
    virtual bool update(const Subscription& subscription) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};