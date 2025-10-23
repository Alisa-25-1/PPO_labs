#pragma once
#include "../types/uuid.hpp"
#include "../models/Branch.hpp"
#include <memory>
#include <optional>
#include <vector>

class IBranchRepository {
public:
    virtual ~IBranchRepository() = default;
    
    virtual std::optional<Branch> findById(const UUID& id) = 0;
    virtual std::vector<Branch> findByStudioId(const UUID& studioId) = 0;
    virtual std::vector<Branch> findAll() = 0;
    virtual bool save(const Branch& branch) = 0;
    virtual bool update(const Branch& branch) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};