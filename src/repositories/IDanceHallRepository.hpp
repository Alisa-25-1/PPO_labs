#pragma once
#include "../types/uuid.hpp"
#include "../models/DanceHall.hpp"
#include <memory>
#include <optional>
#include <vector>

class IDanceHallRepository {
public:
    virtual ~IDanceHallRepository() = default;
    
    virtual std::optional<DanceHall> findById(const UUID& id) = 0; 
    virtual std::vector<DanceHall> findByBranchId(const UUID& branchId) = 0;  
    virtual bool exists(const UUID& id) = 0;
    virtual std::vector<DanceHall> findAll() = 0; 
    virtual bool save(const DanceHall& hall) = 0;  
    virtual bool update(const DanceHall& hall) = 0; 
};