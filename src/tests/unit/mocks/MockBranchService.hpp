#pragma once

#include <gmock/gmock.h>
#include "../../../services/IBranchService.hpp"

class MockBranchService : public IBranchService {
public:
    MOCK_METHOD(std::vector<Branch>, getAllBranches, (), (override));
    MOCK_METHOD(std::optional<Branch>, getBranchById, (const UUID& branchId), (override));
    MOCK_METHOD(std::vector<DanceHall>, getHallsByBranch, (const UUID& branchId), (override));
    MOCK_METHOD(std::string, getBranchName, (const UUID& branchId), (override));
    MOCK_METHOD(std::optional<Branch>, getBranchForHall, (const UUID& hallId), (override));
    MOCK_METHOD(std::chrono::minutes, getTimezoneOffsetForBranch, (const UUID& branchId), (override));
};