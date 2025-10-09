#ifndef POSTGRESQL_HALL_REPOSITORY_HPP
#define POSTGRESQL_HALL_REPOSITORY_HPP

#include "../IHallRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <memory>

class PostgreSQLHallRepository : public IHallRepository {
public:
    explicit PostgreSQLHallRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Hall> findById(const UUID& id) override;
    std::vector<Hall> findByBranchId(const UUID& branchId) override;
    bool exists(const UUID& id) override;
    std::vector<Hall> findAll() override;
    bool save(const Hall& hall) override;
    bool update(const Hall& hall) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Hall mapResultToHall(const pqxx::row& row) const;
    void validateHall(const Hall& hall) const;
};

#endif // POSTGRESQL_HALL_REPOSITORY_HPP