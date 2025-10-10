#pragma once
#include "../types/uuid.hpp"
#include "../models/Studio.hpp"
#include <memory>
#include <optional>
#include <vector>

class IStudioRepository {
public:
    virtual ~IStudioRepository() = default;
    
    virtual std::optional<Studio> findById(const UUID& id) = 0;
    virtual std::optional<Studio> findMainStudio() = 0;
    virtual std::vector<Studio> findAll() = 0;
    virtual bool save(const Studio& studio) = 0;
    virtual bool update(const Studio& studio) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};