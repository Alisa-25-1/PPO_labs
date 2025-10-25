#include "BranchService.hpp"
#include <iostream>

BranchService::BranchService(
    std::shared_ptr<IBranchRepository> branchRepo,
    std::shared_ptr<IDanceHallRepository> hallRepo
) : branchRepository_(std::move(branchRepo)),
    hallRepository_(std::move(hallRepo)) {}

std::vector<Branch> BranchService::getAllBranches() {
    try {
        return branchRepository_->findAll();
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get branches: " + std::string(e.what()));
    }
}

std::optional<Branch> BranchService::getBranchById(const UUID& branchId) {
    try {
        return branchRepository_->findById(branchId);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get branch by ID: " + std::string(e.what()));
    }
}

std::vector<DanceHall> BranchService::getHallsByBranch(const UUID& branchId) {
    try {
        return hallRepository_->findByBranchId(branchId);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get halls by branch: " + std::string(e.what()));
    }
}

std::string BranchService::getBranchName(const UUID& branchId) {
    try {
        auto branch = getBranchById(branchId);
        if (branch) {
            return branch->getName();
        }
        return "Неизвестный филиал";
    } catch (const std::exception& e) {
        return "Ошибка получения названия";
    }
}

std::optional<Branch> BranchService::getBranchForHall(const UUID& hallId) {
    try {
        // Получаем зал
        auto hall = hallRepository_->findById(hallId);
        if (!hall) {
            return std::nullopt;
        }
        
        // Получаем филиал зала
        return branchRepository_->findById(hall->getBranchId());
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении филиала для зала: " << e.what() << std::endl;
        return std::nullopt;
    }
}