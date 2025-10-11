#ifndef POSTGRESQL_TRAINER_REPOSITORY_HPP
#define POSTGRESQL_TRAINER_REPOSITORY_HPP

#include "../ITrainerRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <memory>

class PostgreSQLTrainerRepository : public ITrainerRepository {
public:
    explicit PostgreSQLTrainerRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Trainer> findById(const UUID& id) override;
    std::vector<Trainer> findBySpecialization(const std::string& specialization) override;
    std::vector<Trainer> findActiveTrainers() override;
    std::vector<Trainer> findAll() override;
    bool save(const Trainer& trainer) override;
    bool update(const Trainer& trainer) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Trainer mapResultToTrainer(const pqxx::row& row) const;
    std::vector<std::string> getTrainerSpecializations(const UUID& trainerId) const;
    void validateTrainer(const Trainer& trainer) const;
};

#endif // POSTGRESQL_TRAINER_REPOSITORY_HPP