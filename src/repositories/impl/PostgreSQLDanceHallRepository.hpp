#ifndef POSTGRESQL_DANCEHALL_REPOSITORY_HPP
#define POSTGRESQL_DANCEHALL_REPOSITORY_HPP

#include "../IDanceHallRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include <memory>

class PostgreSQLDanceHallRepository : public IDanceHallRepository {
public:
    explicit PostgreSQLDanceHallRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<DanceHall> findById(const UUID& id) override;
    std::vector<DanceHall> findByBranchId(const UUID& branchId) override;
    bool exists(const UUID& id) override;
    std::vector<DanceHall> findAll() override;
    bool save(const DanceHall& hall) override;
    bool update(const DanceHall& hall) override;
    bool remove(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    DanceHall mapResultToDanceHall(const pqxx::row& row) const;
    void validateDanceHall(const DanceHall& hall) const;
};

#endif // POSTGRESQL_DANCEHALL_REPOSITORY_HPP