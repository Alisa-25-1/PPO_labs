#pragma once
#include "../../../repositories/ISubscriptionTypeRepository.hpp"
#include <gmock/gmock.h>

class MockSubscriptionTypeRepository : public ISubscriptionTypeRepository {
public:
    MOCK_METHOD(std::optional<SubscriptionType>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<SubscriptionType>, findAllActive, (), (override));
    MOCK_METHOD(std::vector<SubscriptionType>, findAll, (), (override));
    MOCK_METHOD(bool, save, (const SubscriptionType& subscriptionType), (override));
    MOCK_METHOD(bool, update, (const SubscriptionType& subscriptionType), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};