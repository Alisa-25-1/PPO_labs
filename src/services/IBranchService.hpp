#pragma once

#include "../models/Branch.hpp"
#include "../models/DanceHall.hpp"
#include "../types/uuid.hpp"
#include <vector>
#include <optional>
#include <memory>

class IBranchService {
public:
    virtual ~IBranchService() = default;
    
    virtual std::vector<Branch> getAllBranches() = 0;
    virtual std::optional<Branch> getBranchById(const UUID& branchId) = 0;
    virtual std::vector<DanceHall> getHallsByBranch(const UUID& branchId) = 0;
    virtual std::string getBranchName(const UUID& branchId) = 0;
    virtual std::optional<Branch> getBranchForHall(const UUID& hallId) = 0;
};