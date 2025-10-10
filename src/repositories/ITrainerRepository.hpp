#pragma once
#include "../types/uuid.hpp"
#include "../models/Trainer.hpp"
#include <memory>
#include <optional>
#include <vector>

class ITrainerRepository {
public:
    virtual ~ITrainerRepository() = default;
    
    virtual std::optional<Trainer> findById(const UUID& id) = 0;
    virtual std::vector<Trainer> findBySpecialization(const std::string& specialization) = 0;
    virtual std::vector<Trainer> findActiveTrainers() = 0;
    virtual std::vector<Trainer> findAll() = 0;
    virtual bool save(const Trainer& trainer) = 0;
    virtual bool update(const Trainer& trainer) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};