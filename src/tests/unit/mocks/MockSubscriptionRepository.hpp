#pragma once
#include "../../../repositories/ISubscriptionRepository.hpp"
#include <gmock/gmock.h>

class MockSubscriptionRepository : public ISubscriptionRepository {
public:
    MOCK_METHOD(std::optional<Subscription>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Subscription>, findByClientId, (const UUID& clientId), (override));
    MOCK_METHOD(std::vector<Subscription>, findActiveSubscriptions, (), (override));
    MOCK_METHOD(std::vector<Subscription>, findExpiringSubscriptions, (int days), (override));
    MOCK_METHOD(std::vector<Subscription>, findAll, (), (override));
    MOCK_METHOD(bool, save, (const Subscription& subscription), (override));
    MOCK_METHOD(bool, update, (const Subscription& subscription), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};