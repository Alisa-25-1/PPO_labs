#ifndef POSTGRESQL_BRANCH_REPOSITORY_HPP
#define POSTGRESQL_BRANCH_REPOSITORY_HPP

#include "../IBranchRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <memory>

class PostgreSQLBranchRepository : public IBranchRepository {
public:
    explicit PostgreSQLBranchRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Branch> findById(const UUID& id) override;
    std::vector<Branch> findByStudioId(const UUID& studioId) override;
    std::vector<Branch> findAll() override;
    bool save(const Branch& branch) override;
    bool update(const Branch& branch) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Branch mapResultToBranch(const pqxx::row& row) const;
    WorkingHours mapResultToWorkingHours(const pqxx::row& row) const;
    void validateBranch(const Branch& branch) const;
};

#endif // POSTGRESQL_BRANCH_REPOSITORY_HPP