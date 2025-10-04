#pragma once
#include "../../repositories/IHallRepository.hpp"
#include <gmock/gmock.h>

class MockHallRepository : public IHallRepository {
public:
    MOCK_METHOD(std::optional<Hall>, findById, (int id), (override));
    MOCK_METHOD(std::vector<Hall>, findByBranchId, (int branchId), (override));
    MOCK_METHOD(bool, exists, (int id), (override));
    MOCK_METHOD(std::vector<Hall>, findAll, (), (override));
};