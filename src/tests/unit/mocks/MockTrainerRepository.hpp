#pragma once
#include "../../../repositories/ITrainerRepository.hpp"
#include <gmock/gmock.h>

class MockTrainerRepository : public ITrainerRepository {
public:
    MOCK_METHOD(std::optional<Trainer>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Trainer>, findBySpecialization, (const std::string& specialization), (override));
    MOCK_METHOD(std::vector<Trainer>, findActiveTrainers, (), (override));
    MOCK_METHOD(std::vector<Trainer>, findAll, (), (override));
    MOCK_METHOD(bool, save, (const Trainer& trainer), (override));
    MOCK_METHOD(bool, update, (const Trainer& trainer), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};