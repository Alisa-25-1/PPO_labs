#pragma once
#include "../models/Hall.hpp"
#include <memory>
#include <optional>
#include <vector>

class IHallRepository {
public:
    virtual ~IHallRepository() = default;
    
    virtual std::optional<Hall> findById(int id) = 0;
    virtual std::vector<Hall> findByBranchId(int branchId) = 0;
    virtual bool exists(int id) = 0;
    virtual std::vector<Hall> findAll() = 0;
};