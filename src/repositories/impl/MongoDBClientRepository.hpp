#ifndef MONGODB_CLIENT_REPOSITORY_HPP
#define MONGODB_CLIENT_REPOSITORY_HPP

#include "../IClientRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp" 
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

class MongoDBRepositoryFactory;

class MongoDBClientRepository : public IClientRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBClientRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Client> findById(const UUID& id) override;
    std::optional<Client> findByEmail(const std::string& email) override;
    std::vector<Client> findAll() override;
    bool save(const Client& client) override;
    bool update(const Client& client) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    Client mapDocumentToClient(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapClientToDocument(const Client& client) const;
    bool existsByEmail(const std::string& email);
};

#endif // MONGODB_CLIENT_REPOSITORY_HPP