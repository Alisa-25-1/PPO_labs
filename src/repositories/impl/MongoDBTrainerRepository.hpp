#ifndef MONGODB_TRAINER_REPOSITORY_HPP
#define MONGODB_TRAINER_REPOSITORY_HPP

#include "../ITrainerRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

// Предварительное объявление вместо инклюда
class MongoDBRepositoryFactory;

class MongoDBTrainerRepository : public ITrainerRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBTrainerRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Trainer> findById(const UUID& id) override;
    std::vector<Trainer> findBySpecialization(const std::string& specialization) override;
    std::vector<Trainer> findActiveTrainers() override;
    std::vector<Trainer> findAll() override;
    bool save(const Trainer& trainer) override;
    bool update(const Trainer& trainer) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    Trainer mapDocumentToTrainer(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapTrainerToDocument(const Trainer& trainer) const;
    void validateTrainer(const Trainer& trainer) const;
};

#endif // MONGODB_TRAINER_REPOSITORY_HPP