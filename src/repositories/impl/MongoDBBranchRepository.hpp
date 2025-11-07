#ifndef MONGODB_BRANCH_REPOSITORY_HPP
#define MONGODB_BRANCH_REPOSITORY_HPP

#include "../IBranchRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

class MongoDBRepositoryFactory;

class MongoDBBranchRepository : public IBranchRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBBranchRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Branch> findById(const UUID& id) override;
    std::vector<Branch> findByStudioId(const UUID& studioId) override;
    std::vector<Branch> findAll() override;
    bool save(const Branch& branch) override;
    bool update(const Branch& branch) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    Branch mapDocumentToBranch(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapBranchToDocument(const Branch& branch) const;
    bsoncxx::document::value mapAddressToDocument(const BranchAddress& address) const;
    BranchAddress mapDocumentToAddress(const bsoncxx::document::view& addressDoc) const;
    void validateBranch(const Branch& branch) const;
};

#endif // MONGODB_BRANCH_REPOSITORY_HPP