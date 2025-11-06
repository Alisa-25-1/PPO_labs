#pragma once

#include "IBranchService.hpp"  
#include "../repositories/IBranchRepository.hpp"
#include "../repositories/IDanceHallRepository.hpp"
#include "../models/Branch.hpp"
#include "../models/DanceHall.hpp"
#include "../types/uuid.hpp"
#include <memory>
#include <vector>

class BranchService : public IBranchService {  // НАСЛЕДОВАНИЕ ОТ ИНТЕРФЕЙСА
private:
    std::shared_ptr<IBranchRepository> branchRepository_;
    std::shared_ptr<IDanceHallRepository> hallRepository_;

public:
    BranchService(
        std::shared_ptr<IBranchRepository> branchRepo,
        std::shared_ptr<IDanceHallRepository> hallRepo
    );

    std::vector<Branch> getAllBranches() override;
    std::optional<Branch> getBranchById(const UUID& branchId) override;
    std::vector<DanceHall> getHallsByBranch(const UUID& branchId) override;
    std::string getBranchName(const UUID& branchId) override;
    std::optional<Branch> getBranchForHall(const UUID& hallId) override;
    std::chrono::minutes getTimezoneOffsetForBranch(const UUID& branchId) override;
};