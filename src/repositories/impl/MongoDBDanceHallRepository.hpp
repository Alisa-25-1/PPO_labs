#ifndef MONGODB_DANCEHALL_REPOSITORY_HPP
#define MONGODB_DANCEHALL_REPOSITORY_HPP

#include "../IDanceHallRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

class MongoDBRepositoryFactory;

class MongoDBDanceHallRepository : public IDanceHallRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBDanceHallRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<DanceHall> findById(const UUID& id) override;
    std::vector<DanceHall> findByBranchId(const UUID& branchId) override;
    bool exists(const UUID& id) override;
    std::vector<DanceHall> findAll() override;
    bool save(const DanceHall& hall) override;
    bool update(const DanceHall& hall) override;
    bool remove(const UUID& id) override;

private:
    DanceHall mapDocumentToDanceHall(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapDanceHallToDocument(const DanceHall& hall) const;
    void validateDanceHall(const DanceHall& hall) const;
};

#endif // MONGODB_DANCEHALL_REPOSITORY_HPP