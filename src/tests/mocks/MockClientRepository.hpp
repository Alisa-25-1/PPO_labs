#pragma once
#include "../../repositories/IClientRepository.hpp"
#include <gmock/gmock.h>

class MockClientRepository : public IClientRepository {
public:
    MOCK_METHOD(std::optional<Client>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::optional<Client>, findByEmail, (const std::string& email), (override));
    MOCK_METHOD(bool, save, (const Client& client), (override));
    MOCK_METHOD(bool, update, (const Client& client), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};