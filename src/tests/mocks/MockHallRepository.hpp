#pragma once
#include "../../repositories/IHallRepository.hpp"
#include <gmock/gmock.h>

class MockHallRepository : public IHallRepository {
public:
    MOCK_METHOD(std::optional<Hall>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Hall>, findByBranchId, (const UUID& branchId), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Hall>, findAll, (), (override));
};