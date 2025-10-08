#pragma once
#include "../types/uuid.hpp"
#include "../models/Hall.hpp"
#include <memory>
#include <optional>
#include <vector>

class IHallRepository {
public:
    virtual ~IHallRepository() = default;
    
    virtual std::optional<Hall> findById(const UUID& id) = 0;
    virtual std::vector<Hall> findByBranchId(const UUID& branchId) = 0;
    virtual bool exists(const UUID& id) = 0;
    virtual std::vector<Hall> findAll() = 0;
};