#ifndef MONGODB_STUDIO_REPOSITORY_HPP
#define MONGODB_STUDIO_REPOSITORY_HPP

#include "../IStudioRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

// Предварительное объявление вместо инклюда
class MongoDBRepositoryFactory;

class MongoDBStudioRepository : public IStudioRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBStudioRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Studio> findById(const UUID& id) override;
    std::optional<Studio> findMainStudio() override;
    std::vector<Studio> findAll() override;
    bool save(const Studio& studio) override;
    bool update(const Studio& studio) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    Studio mapDocumentToStudio(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapStudioToDocument(const Studio& studio) const;
    void validateStudio(const Studio& studio) const;
};

#endif // MONGODB_STUDIO_REPOSITORY_HPP