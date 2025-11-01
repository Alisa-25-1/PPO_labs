#pragma once

#include "../types/uuid.hpp"
#include "../models/Branch.hpp"
#include "../models/DanceHall.hpp"
#include <chrono>
#include <vector>
#include <optional>

class IBranchService {
public:
    virtual ~IBranchService() = default;
    
    virtual std::vector<Branch> getAllBranches() = 0;
    virtual std::optional<Branch> getBranchById(const UUID& branchId) = 0;
    virtual std::vector<DanceHall> getHallsByBranch(const UUID& branchId) = 0;
    virtual std::string getBranchName(const UUID& branchId) = 0;
    virtual std::optional<Branch> getBranchForHall(const UUID& hallId) = 0;
    virtual std::chrono::minutes getTimezoneOffsetForBranch(const UUID& branchId) = 0;
};